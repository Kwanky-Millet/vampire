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
 * @file InequalityResolution.cpp
 * Implements class InequalityResolution.
 */

#include "InequalityResolution.hpp"
#include "Saturation/SaturationAlgorithm.hpp"
#include "Shell/Statistics.hpp"

#define TODO ASSERTION_VIOLATION

namespace Inferences {
namespace IRC {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INDEXING STUFF
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void InequalityResolution::attach(SaturationAlgorithm* salg) 
{
  CALL("InequalityResolution::attach");
  ASS(!_index);

  GeneratingInferenceEngine::attach(salg);
  _index=static_cast<InequalityResolutionIndex*> (
	  _salg->getIndexManager()->request(IRC_INEQUALITY_RESOLUTION_SUBST_TREE) );
  _index->setShared(_shared);
}

void InequalityResolution::detach() 
{
  CALL("InequalityResolution::detach");
  ASS(_salg);

  _index=0;
  _salg->getIndexManager()->release(IRC_INEQUALITY_RESOLUTION_SUBST_TREE);
  GeneratingInferenceEngine::detach();
}

#if VDEBUG
void InequalityResolution::setTestIndices(Stack<Indexing::Index*> const& indices)
{
  _index = (InequalityResolutionIndex*) indices[0]; 
  _index->setShared(_shared);
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// actual rule
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// C1 \/ +j s1 + t1 ≳1 0             C2 \/ −k s2 + t2 ≳2 0 
// =======================================================
// (C1 \/ C2 \/ k t1 + j t2 > 0)σ \/ Cnst
//
// where
// • uwa(s1,s2) = ⟨σ,Cnst⟩
// • ≳1,≳2 ∈ {>,≥}
// • > ∈ {≳1,≳2}
// • s1,s2 /∈ Vars
// • s1σ /⪯ terms(t1)σ
// • s2σ /⪯ terms(t2)σ
// • ( j s1 + t1 ≥ 0)σ /⪯ C1σ
// • (−k s2 + t2 ≥ 0)σ /⪯ C2σ
// • (−k s2 + t2 ≥ 0)σ /⪯ C2σ
//
////////////////////////////////////////////////////
//// for non-strict inequalities to incorporate totality
////////
//
// C1 \/ +j s1 + t1 ≥ 0               C2 \/ −k s2 + t2 ≥ 0 
// ======================================================= (Tight IRC)
// (C1 \/ C2 \/ k t1 + j t2 > 0 \/  j s1 + t1 ≈ 0)σ \/ Cnst
//
// where
// • uwa(s1,s2) = ⟨σ,Cnst⟩
// • s1,s2 /∈ Vars
// • s1σ /⪯ terms(t1)σ
// • s2σ /⪯ terms(t2)σ
// • ( j s1 + t1 ≥ 0)σ /⪯ C1σ
// • (−k s2 + t2 ≥ 0)σ /⪯ C2σ
// • (−k s2 + t2 ≥ 0)σ /⪯ C2σ
//
////////////////////////////////////////////////////
//// for strict-only inequalities and INTEGERS 
////////
//
//     a < b  /\      b < c  ->      a + 1 < c
// b - a > 0  /\  c - b > 0  ->  c - a - 1 > 0
//
// C1 \/ +j s1 + t1 > 0               C2 \/ −k s2 + t2 ≥ 0 
// ======================================================= (Int IRC)
// (C1 \/ C2 \/ k t1 + j t2 - 1 > 0)σ \/ Cnst
//
// where
// • uwa(s1,s2) = ⟨σ,Cnst⟩
// • s1,s2 /∈ Vars
// • s1σ /⪯ terms(t1)σ
// • s2σ /⪯ terms(t2)σ
// • ( j s1 + t1 ≥ 0)σ /⪯ C1σ
// • (−k s2 + t2 ≥ 0)σ /⪯ C2σ
// • (−k s2 + t2 ≥ 0)σ /⪯ C2σ

template<class NumTraits, class Subst, class CnstIter> Option<Clause*> InequalityResolution::applyRule(
    Clause* hyp1, Literal* lit1, IrcLiteral<NumTraits> l1, Monom<NumTraits> j_s1,
    Clause* hyp2, Literal* lit2, IrcLiteral<NumTraits> l2, Monom<NumTraits> k_s2,
    Subst sigma, CnstIter cnst, unsigned nCnst
    ) const 
{

  MeasureTime time(env.statistics->ircIr);
  auto j = j_s1.numeral;
  auto k = k_s2.numeral;
  auto nothing = [&]() { 
    time.applicationCancelled();
    return Option<Clause*>(); 
  };
  auto int_irc = std::is_same<NumTraits, IntTraits>::value 
                          && l1.symbol() == IrcPredicate::GREATER 
                          && l2.symbol() == IrcPredicate::GREATER;
  auto tight_irc = l1.symbol() == IrcPredicate::GREATER_EQ 
                && l2.symbol() == IrcPredicate::GREATER_EQ;
  if (tight_irc) env.statistics->ircIr.cntTight++;
  if (int_irc) env.statistics->ircIr.cntInt++;

  Stack<Literal*> conclusion(hyp1->size() + hyp2->size() + nCnst - (tight_irc ? 0 : 1));

  auto C1_sigma = iterTraits(hyp1->getLiteralIterator())
    .filter([&](auto lit) { return lit != lit1; })
    .map   ([&](auto lit) { return sigma(lit, 0); });

  auto C2_sigma = iterTraits(hyp2->getLiteralIterator())
    .filter([&](auto lit) { return lit != lit2; })
    .map   ([&](auto lit) { return sigma(lit, 1); });

  conclusion.loadFromIterator(getConcatenatedIterator(C1_sigma, C2_sigma));

  auto t1 = l1.term().iterSummands().filter([&](auto t) { return t != j_s1; });
  auto t2 = l2.term().iterSummands().filter([&](auto t) { return t != k_s2; });

  auto k_t1 = t1.clone().map([&](auto t) { return  sigma((-k * t).denormalize(), 0); });
  auto j_t2 = t2.clone().map([&](auto t) { return  sigma(( j * t).denormalize(), 1); });

  // checks wether `forall r ∈ rs . lσ /⪯ rσ`
  auto isStrictlyMax = [&](auto l, auto rs, unsigned varBank) -> bool
  {
    auto l_sigma = sigma(l->denormalize(), varBank);
    while (rs.hasNext()) {
      auto r_sigma = sigma(rs.next()->denormalize(), varBank);
      auto cmp = _shared->ordering->compare(l_sigma, r_sigma);
      if (cmp == Ordering::EQUAL || cmp == Ordering::LESS) 
        return false;
    }
    return true;
  };

  if ( !isStrictlyMax(j_s1.factors, t1.map([&](auto t) { return t.factors; }), 0)
    || !isStrictlyMax(k_s2.factors, t2.map([&](auto t) { return t.factors; }), 1)
      ) return nothing();

  auto k_t1__j_t2 = int_irc
    ? NumTraits::sum(getConcatenatedIterator(k_t1, j_t2, getSingletonIterator(NumTraits::constantTl(-1))))
    : NumTraits::sum(getConcatenatedIterator(k_t1, j_t2));

  // adding (k t1 + j t2     > 0)σ
  //     or (k t1 + j t2 - 1 > 0)σ
  conclusion.push( NumTraits::greater(true, k_t1__j_t2, NumTraits::zero()));

  if (tight_irc) {
    // adding (js1 + t1 ≈ 0)σ
    conclusion.push(NumTraits::eq(true, sigma(l1.term().denormalize(), 0), NumTraits::zero()));
  }

  conclusion.loadFromIterator(cnst);

  Inference inf(GeneratingInference2(Kernel::InferenceRule::IRC_INEQUALITY_RESOLUTION, hyp1, hyp2));
  return Option<Clause*>(Clause::fromStack(conclusion, inf));
}

using Lhs = InequalityResolution::Lhs;
using Rhs = InequalityResolution::Rhs;
auto insert(Lhs) { ASSERTION_VIOLATION }
auto insert(Rhs) { ASSERTION_VIOLATION }

auto getUnifications(Rhs const&) -> IterTraits<VirtualIterator<std::tuple<Lhs, UwaResult>>> 
{
  ASSERTION_VIOLATION
}


auto getUnifications(Lhs const&) -> IterTraits<VirtualIterator<std::tuple<Rhs, UwaResult>>> 
{
  ASSERTION_VIOLATION
}

ClauseIterator InequalityResolution::generateClauses(Clause* premise) 
{
  Stack<Clause*> out;

  for (auto lhs : iterLhs(premise)) {
    for (auto rhs_sigma : getUnifications(lhs)) {
      auto& rhs = std::get<0>(rhs_sigma);
      auto& sigma = std::get<1>(rhs_sigma);
      auto res = applyRule(lhs, 0, rhs, 1, sigma);
      if (res.isSome()) {
        out.push(res.unwrap());
      }
    }
    insert(std::move(lhs));
  }

  for (auto rhs : iterRhs(premise)) {
    for (auto lhs_sigma : getUnifications(rhs)) {
      auto& lhs = std::get<0>(lhs_sigma);
      auto& sigma = std::get<1>(lhs_sigma);
      auto res = applyRule(lhs, 1, rhs, 0, sigma);
      if (res.isSome()) {
        out.push(res.unwrap());
      }
    }
    insert(std::move(rhs));
  }
  return pvi(ownedArrayishIterator(std::move(out)));
}

template<class NumTraits> 
ClauseIterator InequalityResolution::generateClauses(Clause* hyp1, Literal* lit1, IrcLiteral<NumTraits> l1, Monom<NumTraits> j_s1) const
{
        return pvi(iterTraits(_index->getUnificationsWithConstraints(j_s1.factors->denormalize(), true))
            .filterMap([=](TermQueryResult unif) {
              auto hyp2 = unif.clause;
              auto lit2 = unif.literal;
              auto l2 = _shared->renormalize(lit2)
                .unwrap()
                .template unwrap<decltype(l1)>();

              auto s2 = _shared->normalize(TypedTermList(unif.term, NumTraits::sort()))
                .downcast<NumTraits>().unwrap()->tryMonom().unwrap()
                .factors;

              Monom<NumTraits> k_s2 = l2.term()
                .iterSummands()
                .find([&](auto monom) { return monom.factors == s2; })
                .unwrap();

              if (!l2.isInequality())
                return Option<Clause*>();

              if (j_s1.numeral.isNegative() == k_s2.numeral.isNegative())
                return Option<Clause*>();

              auto swap = j_s1.numeral.isNegative();

              // TODO check maximality conditions here (?)

              Stack<UnificationConstraint> _constr;
              auto& constr = unif.constraints.isEmpty() ? _constr : *unif.constraints;
              auto constrIter = UwaResult::cnstLiterals(*unif.substitution, constr);
              
              auto sigma = [&](auto t, int varBank) { 
                ASS(varBank == 0 || varBank == 1)
                return unif.substitution->applyTo(t, swap ? 1 - varBank 
                                                          :     varBank); 
              };
              return swap 
                ?  applyRule(hyp2, lit2, l2, k_s2, hyp1, lit1, l1, j_s1, sigma, constrIter, constr.size() )
                :  applyRule(hyp1, lit1, l1, j_s1, hyp2, lit2, l2, k_s2, sigma, constrIter, constr.size() );
            })
        );
      // })
      // );
}

// Fourier Motzkin normal:
//
// C₁ \/ +j s + t₁ >₁ 0         C₂ \/ -k s' + t₂ >₂ 0 
// --------------------------------------------------
//           (C₁ \/ C₂ \/ k t₁ + j t₂ > 0)σ \/ Cnst
//
// where 
// - (σ, Cnst) = uwa(s, s')
// - C₁σ ≺ (+j s  + t₁ >₁ 0)σ
// - C₂σ ≺ (-k s' + t₂ >₂ 0)σ
// - s, s' are not variables
// - {>} ⊆ {>₁,>₂} ⊆ {>,≥}
//
// Fourier Motzkin tight:
//
// C₁ \/ +j s + t₁ ≥ 0                 C₂ \/ -k s' + t₂ ≥ 0 
// --------------------------------------------------------
// (C₁ \/ C₂ \/ k t₁ + j t₂ > 0 \/ -k s' + t₂ ≈ 0)σ \/ Cnst
//
// where 
// - (σ, Cnst) = uwa(s, s')
// - (+j s  + t₁ >₁ 0)σ /≺ C₁σ
// - (-k s' + t₂ >₂ 0)σ /≺ C₂σ
// - s, s' are not variables
//
Option<Clause*> InequalityResolution::applyRule(
    Lhs const& lhs, unsigned lhsVarBank,
    Rhs const& rhs, unsigned rhsVarBank,
    UwaResult& uwa
    ) const 
{

  return lhs.numTraits().apply([&](auto numTraits) {
    using NumTraits = decltype(numTraits);


    ASS(lhs.sign() == Sign::Pos)
    ASS(rhs.sign() == Sign::Neg)
    ASS(lhs.literal()->functor() == NumTraits::geqF()
     || lhs.literal()->functor() == NumTraits::greaterF())
    ASS(rhs.literal()->functor() == NumTraits::geqF()
     || rhs.literal()->functor() == NumTraits::greaterF())

    bool tight = lhs.literal()->functor() == NumTraits::geqF()
              && rhs.literal()->functor() == NumTraits::geqF();

    Stack<Literal*> out(lhs.clause()->size() + rhs.clause()->size() - 1 + (tight ? 1 : 0) + uwa.cnst.size());

    auto mainLiteralMaximal = [this](auto& selected, UwaResult& uwa, unsigned varBank, auto applyMax) {
      auto main = uwa.sigma.apply(selected.literal(), varBank);
      for (auto lit_ : selected.contextLiterals()) {
        auto lit = uwa.sigma.apply(lit_, varBank);
        switch (_shared->ordering->compare(main, lit)) {
          case Ordering::LESS: 
          case Ordering::EQUAL: 
            return false;
          default:
            applyMax(lit);
        }
      }
      return true;
    };
 
    // (+j s  + t₁ >₁ 0)σ /≺ C₁σ
    if (!mainLiteralMaximal(lhs, uwa, lhsVarBank, [&](auto lit) { out.push(lit); })) 
      return Option<Clause*>();
    // (-k s' + t₂ >₂ 0)σ /≺ C₂σ
    if (!mainLiteralMaximal(rhs, uwa, lhsVarBank, [&](auto lit) { out.push(lit); })) 
      return Option<Clause*>();

    // s, s' are not variables
    if (lhs.monom().isVar() || rhs.monom().isVar()) 
      return Option<Clause*>();


    auto j = NumTraits::constantTl(lhs.numeral().unwrap<typename NumTraits::ConstantType>());
    auto k = NumTraits::constantTl(rhs.numeral().unwrap<typename NumTraits::ConstantType>());
    auto t1σ = NumTraits::sum(rhs.contextTerms()
                 .map([&](auto t) { return uwa.sigma.apply(t, lhsVarBank); }));
    auto t2σ = NumTraits::sum(rhs.contextTerms()
                 .map([&](auto t) { return uwa.sigma.apply(t, lhsVarBank); }));

    auto resolventTerm // -> (k t₁ + j t₂)σ
        = NumTraits::add(
            NumTraits::mul(k, t1σ),
            NumTraits::mul(j, t2σ)
        );

    out.push(NumTraits::greater(true, resolventTerm, NumTraits::zero()));

    if (tight) {
      auto rhsSum = // -> (-k s' + t₂)σ
        uwa.sigma.apply(rhs.literal(), rhsVarBank)->termArg(0);
      out.push(NumTraits::eq(true, rhsSum, NumTraits::zero()));
    }

    out.loadFromIterator(uwa.cnstLiterals());

    Inference inf(GeneratingInference2(Kernel::InferenceRule::IRC_INEQUALITY_RESOLUTION, lhs.clause(), rhs.clause()));
    return Option<Clause*>(Clause::fromStack(out, inf));
  });
}

} // namespace IRC 
} // namespace Inferences 
