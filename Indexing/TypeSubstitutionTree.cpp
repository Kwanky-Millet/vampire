/*
 * This file is part of the source code of the software program
 * Vampire. It is protected by applicable
 * copyright laws.
 *
 * This source code is distributed under the licence found here
 * https://vprover.github.io/license.html
 * and in the source directory
 */
/**
 * @file TypeSubstitutionTree.cpp
 * Implements class TypeSubstitutionTree.
 */

#include "Lib/Environment.hpp"
#include "Lib/Metaiterators.hpp"
#include "Lib/Random.hpp"
#include "Lib/SmartPtr.hpp"
#include "Lib/TimeCounter.hpp"

#include "Kernel/TermIterators.hpp"
#include "Kernel/Matcher.hpp"
#include "Kernel/Signature.hpp"
#include "Kernel/Term.hpp"
#include "Kernel/SortHelper.hpp"
#include "Kernel/ApplicativeHelper.hpp"

#include "Shell/Options.hpp"


namespace Indexing
{

using namespace Lib;
using namespace Kernel;

template<class LeafData_>
TypeSubstitutionTree<LeafData_>::TypeSubstitutionTree()
: SubstitutionTree(env.signature->functions(), Options::UnificationWithAbstraction::OFF)
{
}

#define QRS_QUERY_BANK 0
#define QRS_RESULT_BANK 1

template<class LeafData_>
struct TypeSubstitutionTree<LeafData_>::VarUnifFn
{
  VarUnifFn(TermList queryTerm, TermList sort)
  : _queryTerm(queryTerm), _sort(sort) {
    _subst=RobSubstitutionSP(new RobSubstitution());
  }

  TermQueryResult<LeafData> operator() (TermQueryResult<LeafData> tqr) {
    TermList tqrSort = tqr.sort();
    _subst->reset();

    ASS(_sort.isVar() || tqrSort.isVar());
    ALWAYS(_subst->unify(_sort, QRS_QUERY_BANK, tqrSort, QRS_RESULT_BANK));
    
    bool isTypeSub = false;
    if(_queryTerm.isVar() || tqr.key().isVar()){
      ALWAYS(_subst->unify(_queryTerm, QRS_QUERY_BANK, tqr.key(), QRS_RESULT_BANK));
    } else {
      isTypeSub = true;
    }

    return TermQueryResult<LeafData>(tqr,
    ResultSubstitution::fromSubstitution(_subst.ptr(),
      QRS_QUERY_BANK,QRS_RESULT_BANK), isTypeSub);
  }

private:
  RobSubstitutionSP _subst;
  TermList _queryTerm;
  TermList _sort;
};

template<class LeafData_>
struct TypeSubstitutionTree<LeafData_>::ToTypeSubFn
{

  ToTypeSubFn(TermList queryTerm)
  : _queryTerm(queryTerm) {}

  TermQueryResult<LeafData> operator() (TermQueryResult<LeafData> tqr) {
    if(!_queryTerm.isVar() && !tqr.key().isVar()){
      tqr.isTypeSub = true;
    } else {
      RobSubstitution* subst = tqr.substitution->tryGetRobSubstitution();
      ALWAYS(subst->unify(_queryTerm, QRS_QUERY_BANK, tqr.key(), QRS_RESULT_BANK));      
    }
    return tqr;
  }

private:
  TermList _queryTerm;
};

/**
 * According to value of @b insert, insert or remove term.
 */
template<class LeafData_>
void TypeSubstitutionTree<LeafData_>::handleTerm(LeafData ld, bool insert)
{
  CALL("TypeSubstitutionTree::handleTerm");
  auto sort = ld.key();

  if(sort.isOrdinaryVar()) {
    if(insert) {
      _vars.insert(ld);
    } else {
      // why is this case needed?
      _vars.remove(ld);
    }
  }  else {
    ASS(sort.isTerm());
    Term* term=sort.term();

    Renaming normalizer;
    normalize(normalizer, ld);
    // normalizer.normalizeVariables(ld.key());

    Term* normSort=normalizer.apply(term);

    BindingMap svBindings;
    this->getBindings(normSort, svBindings);

    unsigned rootNodeIndex=getRootNodeIndex(normSort);

    if(insert) {
      SubstitutionTree::insert(&this->_nodes[rootNodeIndex], svBindings, ld);
    } else {
      SubstitutionTree::remove(&this->_nodes[rootNodeIndex], svBindings, ld);
    }
  }
}

//TODO use sorts and delete non-shared
template<class LeafData_>
TermQueryResultIterator<LeafData_> TypeSubstitutionTree<LeafData_>::getUnifications(TermList sort, TermList trm,
	  bool retrieveSubstitutions)
{
  CALL("TypeSubstitutionTree::getUnifications");
 
  //cout << "getting unification partners for " + sort.toString() << endl;
  //Debug::Tracer::printStack(cout);


  auto it1 = !_vars.isEmpty() ? pvi(getMappingIterator(ldIteratorToTQRIterator(typename LDSkipList::RefIterator(_vars), sort, false), VarUnifFn(trm, sort))) :
             TermQueryResultIterator::getEmpty();

  if(sort.isOrdinaryVar()) { //TODO return vars as well?
    auto it2 = getMappingIterator(getAllUnifyingIterator(sort,false), VarUnifFn(trm, sort));
    return pvi(getConcatenatedIterator(it1, it2)); 
  } else {
    ASS(sort.isTerm());
    auto it2 =  getMappingIterator(
	  getResultIterator<UnificationsIterator>(sort.term(), retrieveSubstitutions), ToTypeSubFn(trm));
    return pvi(getConcatenatedIterator(it1, it2));     
  }
}

/**
 * Functor, that transforms &b QueryResult struct into
 * @b TermQueryResult.
 */
template<class LeafData_>
struct TypeSubstitutionTree<LeafData_>::TermQueryResultFn
{
  TermQueryResult<LeafData> operator() (const QueryResult& qr) {
    return TermQueryResult<LeafData>(*qr.first.first, qr.first.second,qr.second);
  }
};

template<class LeafData_>
template<class Iterator>
TermQueryResultIterator<LeafData_> TypeSubstitutionTree<LeafData_>::getResultIterator(Term* trm,
	  bool retrieveSubstitutions)
{
  CALL("TypeSubstitutionTree::getResultIterator");

  //cout << "getResultIterator " << trm->toString() << endl;

  TermQueryResultIterator result = TermQueryResultIterator::getEmpty();
  
  Node* root = this->_nodes[getRootNodeIndex(trm)];

  if(root){
    if(root->isLeaf()) {
      LDIterator ldit=static_cast<Leaf*>(root)->allChildren();
      result = ldIteratorToTQRIterator(ldit,TermList(trm),retrieveSubstitutions);
    }
    else{
      VirtualIterator<QueryResult> qrit=vi( new Iterator(this, root, trm, retrieveSubstitutions,false,false,false) );
      result = pvi( getMappingIterator(qrit, TermQueryResultFn()) );
    }
  }

  return result;
}

template<class LeafData_>
struct TypeSubstitutionTree<LeafData_>::LDToTermQueryResultFn
{
  TermQueryResult<LeafData> operator() (const LeafData& ld) {
    return TermQueryResult<LeafData>(ld);
  }
};

template<class LeafData_>
struct TypeSubstitutionTree<LeafData_>::LDToTermQueryResultWithSubstFn
{
  LDToTermQueryResultWithSubstFn()
  {
    _subst=RobSubstitutionSP(new RobSubstitution());
  }
  TermQueryResult<LeafData> operator() (const LeafData& ld) {
    return TermQueryResult<LeafData>(ld,
    ResultSubstitution::fromSubstitution(_subst.ptr(),
	    QRS_QUERY_BANK,QRS_RESULT_BANK));
  }
private:
  RobSubstitutionSP _subst;
};

template<class LeafData_>
struct TypeSubstitutionTree<LeafData_>::LeafToLDIteratorFn
{
  LDIterator operator() (Leaf* l) {
    CALL("TypeSubstitutionTree::LeafToLDIteratorFn()");
    return l->allChildren();
  }
};

template<class LeafData_>
struct TypeSubstitutionTree<LeafData_>::UnifyingContext
{
  UnifyingContext(TermList queryTerm)
  : _queryTerm(queryTerm) {}
  bool enter(TermQueryResult<LeafData> qr)
  {
    //if(_withConstraints){ cout << "enter " << qr.key() << endl; }

    ASS(qr.substitution);
    RobSubstitution* subst=qr.substitution->tryGetRobSubstitution();
    ASS(subst);
    bool unified = subst->unify(_queryTerm, QRS_QUERY_BANK, qr.key(), QRS_RESULT_BANK);
    return unified;
  }
  void leave(TermQueryResult<LeafData> qr)
  {
    RobSubstitution* subst=qr.substitution->tryGetRobSubstitution();
    ASS(subst);
    subst->reset();
  }
private:
  TermList _queryTerm;
};

template<class LeafData_>
template<class LDIt>
TermQueryResultIterator<LeafData_> TypeSubstitutionTree<LeafData_>::ldIteratorToTQRIterator(LDIt ldIt,
	TermList queryTerm, bool retrieveSubstitutions)
{
  CALL("TypeSubstitutionTree::ldIteratorToTQRIterator");
  // only call withConstraints if we are also getting substitions, the other branch doesn't handle constraints
  //ASS(retrieveSubstitutions); 

  if(retrieveSubstitutions) {
    return pvi( getContextualIterator(
	    getMappingIterator(
		    ldIt,
		    LDToTermQueryResultWithSubstFn()),
	    UnifyingContext(queryTerm)) );
  } else {
    return pvi( getMappingIterator(
	    ldIt,
	    LDToTermQueryResultFn()) );
  }
}

template<class LeafData_>
TermQueryResultIterator<LeafData_> TypeSubstitutionTree<LeafData_>::getAllUnifyingIterator(TermList trm,
	  bool retrieveSubstitutions)
{
  CALL("TypeSubstitutionTree::getAllUnifyingIterator");

  //if(withConstraints){ cout << "getAllUnifyingIterator for " << trm.toString() << endl; }

  ASS(trm.isVar());

  auto it1 = getFlattenedIterator(getMappingIterator(vi( new LeafIterator(this) ), LeafToLDIteratorFn()));

  // If we are searching withConstraints it means that we have already added in
  // the results related to _vars, we are only interested in non-unifying leaves

  return ldIteratorToTQRIterator(
    getConcatenatedIterator(it1,typename LDSkipList::RefIterator(_vars)),
    trm, retrieveSubstitutions);
  
}


template<class LeafData_>
std::ostream& TypeSubstitutionTree<LeafData_>::output(std::ostream& out) const 
{ return out << *static_cast<SubstitutionTree const*>(this); }

}
