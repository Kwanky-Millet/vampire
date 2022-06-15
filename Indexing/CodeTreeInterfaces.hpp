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
 * @file CodeTreeInterfaces.hpp
 * Defines classes of indexing structures that use code trees.
 */

#ifndef __CodeTreeInterfaces__
#define __CodeTreeInterfaces__

#include "Forwards.hpp"

#include "TermCodeTree.hpp"
#include "ClauseCodeTree.hpp"

#include "Index.hpp"
#include "TermIndexingStructure.hpp"
#include "LiteralIndexingStructure.hpp"

#include "Lib/Allocator.hpp"

namespace Indexing
{

using namespace Kernel;
using namespace Lib;


/**
 * Term indexing structure using code trees to retrieve generalizations
 */

class CodeTreeTIS : public TermIndexingStructure<DefaultTermLeafData>
{
  using TermQueryResultIterator = Indexing::TermQueryResultIterator<DefaultTermLeafData>;
  using TermQueryResult         = Indexing::TermQueryResult<DefaultTermLeafData>;
public:

  CLASS_NAME(CodeTreeTIS);
  USE_ALLOCATOR(CodeTreeTIS);

    //TODO add final override
  virtual void insert(DefaultTermLeafData data)
  { _insert(data.term, data.literal, data.clause); }
    //TODO add final override
  virtual void remove(DefaultTermLeafData data)
  { _remove(data.term, data.literal, data.clause); }

  void _insert(TermList t, Literal* lit, Clause* cls);
  void _remove(TermList t, Literal* lit, Clause* cls);

  TermQueryResultIterator getGeneralizations(TermList t, bool retrieveSubstitutions = true) final override;
  bool generalizationExists(TermList t) final override;

  virtual std::ostream& output(std::ostream& out) const final override
  { return out << _ct; }

private:
  class ResultIterator;

  TermCodeTree _ct;
};
/*
class CodeTreeLIS : public LiteralIndexingStructure
{
public:
  void insert(Literal* lit, Clause* cls);
  void remove(Literal* lit, Clause* cls);

  SLQueryResultIterator getGeneralizations(Literal* lit,
	  bool complementary, bool retrieveSubstitutions = true);
private:
  struct LiteralInfo;
  class ResultIterator;

  TermCodeTree _ct;
};
*/

class CodeTreeSubsumptionIndex
: public ClauseSubsumptionIndex
{
public:
  CLASS_NAME(CodeTreeSubsumptionIndex);
  USE_ALLOCATOR(CodeTreeSubsumptionIndex);

  ClauseSResResultIterator getSubsumingOrSResolvingClauses(Clause* c, bool subsumptionResolution);
protected:
  //overrides Index::handleClause
  void handleClause(Clause* c, bool adding);
private:
  class ClauseSResIterator;

  ClauseCodeTree _ct;
};

};
#endif /*__CodeTreeInterfaces__*/
