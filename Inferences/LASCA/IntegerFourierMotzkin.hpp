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
 * @file IntegerFourierMotzkin.hpp
 * Defines class IntegerFourierMotzkin
 *
 */

#ifndef __LASCA_IntegerFourierMotzkin__
#define __LASCA_IntegerFourierMotzkin__

#include "FourierMotzkin.hpp"
#include "Coherence.hpp"

namespace Inferences {
namespace LASCA {

using namespace Kernel;
using namespace Indexing; using namespace Saturation;

template<class NumTraits>
struct IntegerFourierMotzkinConf
{
  IntegerFourierMotzkinConf(std::shared_ptr<LascaState> shared) 
    : _shared(std::move(shared))
  {  }

  using Premise0 = FourierMotzkin::Lhs;
  using Premise1 = FourierMotzkin::Rhs;
  using Premise2 = typename Coherence<NumTraits>::Lhs;

  auto applyRule(
      Premise0 const& prem0, unsigned varBank0,
      Premise1 const& prem1, unsigned varBank1,
      Premise2 const& prem2, unsigned varBank2,
      AbstractingUnifier& uwa
      ) const 
  { return applyRule_(prem0, varBank0, 
                      prem1, varBank1,
                      prem2, varBank2, uwa).intoIter(); }

  // prem0:  s + t0 > 0
  // prem1: -s + t1 > 0
  // prem2: isInt(j s + u)
  // =========================
  // ⌈j t0 − u⌉ + ⌈j t1 + u⌉ − 2 > 0 ∨ js + u + ⌈jt0 − u⌉ − 1 ≈ 0
  Option<Clause*> applyRule_(
      Premise0 const& prem0, unsigned varBank0,
      Premise1 const& prem1, unsigned varBank1,
      Premise2 const& prem2, unsigned varBank2,
      AbstractingUnifier& uwa) const
  {
    auto sigma2 = [&](auto t)  { return uwa.subs().apply(t, varBank2); };
    return applyRule__(prem0, varBank0,
                       prem1, varBank1,
                       prem2.j(),
                       sigma2(prem2.u()),
                       prem2.contextLiterals().map([&](auto l) { return sigma2(l); }),
                       uwa,
                       [&](auto lits) {
                         // TODO not use UnitList here. That's slow
                         return Clause::fromIterator(
                            std::move(lits),
                            Inference(GeneratingInferenceMany(Kernel::InferenceRule::LASCA_INTEGER_FOURIER_MOTZKIN, UnitList::fromIterator(iterItems(prem0.clause(), prem1.clause(), prem2.clause()))))
                         );
                       });
  }

  // prem0: C0 \/ s + t0 > 0
  // prem1: C1 \/ -s + t1 > 0
  // prem2: C2 \/ isInt(j s + u)
  // =========================
  // ⌈j t0 − u⌉ + ⌈j t1 + u⌉ − 2 > 0 ∨ js + u + ⌈jt0 − u⌉ − 1 ≈ 0
  template<class C2, class MkClause>
  static Option<Clause*> applyRule__(
      Premise0 const& prem0, unsigned varBank0,
      Premise1 const& prem1, unsigned varBank1,
      typename NumTraits::ConstantType j,  // <- the constant j
      TermList u_s, // <- the term u\sigma
      C2 c2_s, // <- iterator over the literals C2\sigma
      AbstractingUnifier& uwa,
      MkClause mkClause)
  { 
    ASS(prem0.numeral<NumTraits>().isPositive())
    ASS(prem1.numeral<NumTraits>().isNegative())
    auto sigma0 = [&](auto t)  { return uwa.subs().apply(t, varBank0); };
    auto sigma1 = [&](auto t)  { return uwa.subs().apply(t, varBank1); };
    auto s_s  = sigma0(prem0.selectedTerm());
    auto t0_s = sigma0(prem0.notSelectedTerm());
    auto t1_s = sigma1(prem1.notSelectedTerm());
    ASS(j.isPositive())
    // auto s_sigma = sigma0(prem0.selectedTerm());

    auto ceil = [](auto x) { return NumTraits::minus(NumTraits::floor(NumTraits::minus(x))); };
    auto sum = [](auto... xs) { return NumTraits::sum(iterItems(TermList(xs)...)); };
    auto mul = [](auto l, auto r) { return NumTraits::mul(NumTraits::constantTl(l), r); };

    auto t0_strengthened = prem0.lascaPredicate().unwrap() == LascaPredicate::GREATER_EQ 
      //     s + t0 >= 0
      ? t0_s
      //     s + t0 > 0
      // <-> j s + u > - j t0 + u
      // <-> j s + u >= ⌊- j t0 + u⌋ + 1
      // <-> j s + u + ⌈j t0 - u⌉ - 1 >= 0
      // <-> s + (u + ⌈j t0 - u⌉ - 1)/j >= 0
      //         ^^^^^^^^^^^^^^^^^^^^^^--> t0_strengthened
      : mul(1/j, sum( u_s 
                    , ceil(sum(mul(j, t0_s), NumTraits::minus(u_s)))
                    , NumTraits::constantTl(-1)));

    auto t1_strengthened = prem1.lascaPredicate().unwrap() == LascaPredicate::GREATER_EQ 
      //     -s + t1 >= 0
      ? t1_s
      //     -s + t1 > 0
      // <->  j t1 + u  > j s + u
      // <-> ⌈j t1 + u⌉ - 1 >= j s + u
      // <-> -j s - u + ⌈j t1 + u⌉ - 1 >= 0
      // <-> -s + (- u + ⌈j t1 + u⌉ - 1)/j >= 0
      //          ^^^^^^^^^^^^^^^^^^^^^^^^--> t1_strengthened
      : mul(1/j, sum( NumTraits::minus(u_s) 
                    , ceil(sum(mul(j, t1_s), u_s))
                    , NumTraits::constantTl(-1)));
      // : sum(ceil(sum(mul(j, t0_s), NumTraits::minus(u_s))), NumTraits::constantTl(-1));

    return some(mkClause(
          concatIters(
            prem0.contextLiterals().map([&](auto l) { return sigma0(l); }),
            prem1.contextLiterals().map([&](auto l) { return sigma1(l); }),
            std::move(c2_s),
            arrayIter(uwa.computeConstraintLiterals()),
            iterItems(
              NumTraits::greater(true, sum(t0_strengthened, t1_strengthened), NumTraits::constantTl(0)),
              NumTraits::eq(true, sum(s_s, t0_strengthened), NumTraits::constantTl(0))
            )
          )));
  }

  std::shared_ptr<LascaState> _shared;
};

template<class NumTraits>
struct IntegerFourierMotzkin : public TriInf<IntegerFourierMotzkinConf<NumTraits>>  {
  IntegerFourierMotzkin(std::shared_ptr<LascaState> state) 
    : TriInf<IntegerFourierMotzkinConf<NumTraits>>(state, IntegerFourierMotzkinConf<NumTraits>(state)) {}
};

} // namespace LASCA 
} // namespace Inferences 


#endif /*__LASCA_IntegerFourierMotzkin__*/
