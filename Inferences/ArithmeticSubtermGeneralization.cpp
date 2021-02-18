
  /*
   * File ArithmeticSubtermGeneralization.cpp.
   *
   * This file is part of the source code of the software program
   * Vampire. It is protected by applicable
   * copyright laws.
   *
   * This source code is distributed under the licence found here
   * https://vprover.github.io/license.html
   * and in the source directory
   */

#include "Debug/Assertion.hpp"
#include "Inferences/ArithmeticSubtermGeneralization.hpp"
#include "Kernel/Clause.hpp"
#include "Kernel/PolynomialNormalizer.hpp"
#include "Lib/IntUnionFind.hpp"
#include "Lib/Array.hpp"
#include "Lib/Set.hpp"
#include "Kernel/Ordering.hpp"
#include "Shell/Statistics.hpp"

#define DEBUG(...) //DBG(__VA_ARGS__)

namespace Inferences {

/** iterator over all subterms of a clause in polynomial normal form */
static const auto iterTerms = [](Clause* cl) 
{
  return iterTraits(cl->iterLits())
    .flatMap([](Literal* lit) { return iterArgsPnf(lit); }) 
    .flatMap([](PolyNf arg) { return arg.iterSubterms();  });
};

/**
 * Type to erase the NumTraits type parameter from some other template class, by dynamically 
 * storing information which NumTraits object it was instantiated with.
 */
template<template<class> class NumberObject>
class AnyNumber : public Coproduct<NumberObject<IntTraits>, NumberObject<RatTraits>, NumberObject<RealTraits> > 
{

public:
  using Super = Coproduct<NumberObject<IntTraits>, NumberObject<RatTraits>, NumberObject<RealTraits>>;

  AnyNumber(NumberObject<IntTraits> &self) : Super(self) {}
  AnyNumber(NumberObject<IntTraits> const& self) : Super(self) {}
  AnyNumber(NumberObject<IntTraits> && self) : Super(move(self)) {}

  AnyNumber(NumberObject<RatTraits> &self) : Super(self) {}
  AnyNumber(NumberObject<RatTraits> const& self) : Super(self) {}
  AnyNumber(NumberObject<RatTraits> && self) : Super(move(self)) {}

  AnyNumber(NumberObject<RealTraits> &self) : Super(self) {}
  AnyNumber(NumberObject<RealTraits> const& self) : Super(self) {}
  AnyNumber(NumberObject<RealTraits> && self) : Super(move(self)) {}

  template<class NumTraits> Option<NumberObject<NumTraits> const&> downcast() const& { return Super::template as<NumberObject<NumTraits>>(); }
  template<class NumTraits> Option<NumberObject<NumTraits>      &> downcast()      & { return Super::template as<NumberObject<NumTraits>>(); }
  template<class NumTraits> Option<NumberObject<NumTraits>     &&> downcast()     && { return Super::template as<NumberObject<NumTraits>>(); }
  
  friend bool operator<(AnyNumber const& lhs, AnyNumber const& rhs)
  { return std::less<Super>{}(lhs,rhs); }
};


/** iterator over all subterms of a clause that are polynoms */
static const auto iterPolynoms = [](Clause* cl) {
  return iterTerms(cl)
    .filterMap([](PolyNf subterm) 
        { return subterm.template as<AnyPoly>().toOwned(); });
};

/** iterator over all subterms of a clause that are variables */
static const auto iterVars = [](Clause* cl) {
  return iterTerms(cl)
    .filterMap([](PolyNf subterm) 
        { return subterm.template as<Variable>().toOwned(); });
};

template<class EvalFn>
SimplifyingGeneratingInference1::Result generalizeBottomUp(Clause* cl, EvalFn eval) 
{
  CALL("generalizeBottomUp")
  /* apply the selectedGen generalization */
  bool anyChange = false;
  bool oneLess = false;
  bool allLessEq = true;

  auto stack = iterTraits(cl->iterLits())
    .map([&](Literal* lit) -> Literal* {
        CALL("generalizeBottomUp(Clause* cl, EvalFn)@closure 1")
        unsigned j = 0;
        auto args = argIter(lit)
          .map([&](TermList term) -> TermList { 
              CALL("generalizeBottomUp(Clause* cl, EvalFn)@closure 2")
              auto norm = PolyNf::normalize(TypedTermList(term, SortHelper::getArgSort(lit, j++)));
              auto res = evaluateBottomUp(norm, eval);
              if (res != norm) {
                anyChange = true;
                DEBUG("generalized: ", norm, " -> ", res);
                return res.denormalize();
              } else {
                return term;
              }
          })
          .template collect<Stack>();

        auto generalizedLit = Literal::create(lit, args.begin());

        if (eval.eval.doOrderingCheck) {

          auto ord = Ordering::tryGetGlobalOrdering();
          ASS(ord)
          auto cmp = ord->compare(generalizedLit, lit);
          switch(cmp) {
            case Ordering::LESS:
              oneLess = true;
              break;
            case Ordering::LESS_EQ:
            case Ordering::EQUAL:
              break;
            case Ordering::GREATER:
            case Ordering::GREATER_EQ:
            case Ordering::INCOMPARABLE:
              allLessEq = false;
              DEBUG("ordering violation: ", cmp)
              DEBUG("original   : ", *lit)
              DEBUG("generalized: ", *generalizedLit)
              break;
          }
        }
        return generalizedLit;
    })
    .template collect<Stack>();

  ASS (anyChange) 
  Inference inf(SimplifyingInference1(Kernel::InferenceRule::ARITHMETIC_SUBTERM_GENERALIZATION, cl));
  bool redundant = allLessEq && oneLess;
  env.statistics->asgCnt++;
  if (!redundant) {
    env.statistics->asgViolations++;
  }
  return SimplifyingGeneratingInference1::Result{
    .simplified = Clause::fromStack(stack, inf), 
    .premiseRedundant = redundant
  };
}

template<class Generalization>
struct ArithmeticSubtermGeneralization
{
  static SimplifyingGeneratingInference1::Result simplify(Clause* cl, bool doCheckOrdering);
};

/** type to represent the top element in a lattice */
struct Top {};

/** type to represent the bottom element in a lattice */
struct Bot {};

ostream& operator<<(ostream& out, Bot self) { return out << "bot"; }
ostream& operator<<(ostream& out, Top self) { return out << "top"; }
bool operator==(Top,Top) { return true; }
bool operator==(Bot,Bot) { return true; }

/** bottom up evaluate an object of type AnyPoly */
template<class Eval>
struct EvaluateAnyPoly
{
  Eval eval;
  using Arg    = PolyNf;
  using Result = PolyNf;

  PolyNf operator()(PolyNf term, PolyNf* evaluatedArgs) 
  {
    CALL("EvaluateAnyPoly::operator()")
    auto out = term.match(
        [&](Perfect<FuncTerm> t) -> PolyNf
        { return perfect(FuncTerm(t->function(), evaluatedArgs)); },

        [&](Variable v) 
        { return v; },

        [&](AnyPoly p) 
        { return PolyNf(eval(p, evaluatedArgs)); }
        );
    return out;
  }
};


/** polymorphic closure. helper class for EvaluatePolynom */
template<class Eval>
struct EvalPolynomClsr {
  Eval& eval;
  PolyNf* evaluatedArgs;

  template<class NumTraits>
  AnyPoly operator()(Perfect<Polynom<NumTraits>> poly)
  { return AnyPoly(eval(poly, evaluatedArgs)); }
};


/** bottomup evaluates a Polynom */
template<class Eval>
struct EvaluatePolynom
{
  Eval eval;
  using Arg    = PolyNf;
  using Result = PolyNf;

  AnyPoly operator()(AnyPoly poly, PolyNf* evaluatedArgs)
  { 
    CALL("EvaluatePolynom::operator()(AnyPoly, PolyNf*)")
    return poly.apply(EvalPolynomClsr<Eval>{eval, evaluatedArgs}); 
  }

  PolyNf operator()(PolyNf term, PolyNf* evaluatedArgs) 
  {
    CALL("EvaluatePolynom::operator()")
    return EvaluateAnyPoly<EvaluatePolynom>{*this}(term, evaluatedArgs);
  }
};

/** bottomup evaluates a Monom */
template<class Eval>
struct EvaluateMonom
{
  Eval eval;
  using Arg    = PolyNf;
  using Result = PolyNf;

  template<class NumTraits>
  Perfect<Polynom<NumTraits>> operator()(Perfect<Polynom<NumTraits>> poly, PolyNf* evaluatedArgs)
  { 
    CALL("EvaluateMonom::operator()(AnyPoly, PolyNf*)")

    using Polynom   = Kernel::Polynom<NumTraits>;
    using Monom  = Kernel::Monom<NumTraits>;

    auto offs = 0;
    return perfect(Polynom(
                poly->iterSummands()
                 .map([&](Monom m) -> Monom { 
                   CALL("EvaluateMonom::clsr01")

                   auto result = eval(m, &evaluatedArgs[offs]);
                   offs += m.factors->nFactors();
                   return result;
               })
            .template collect<Stack>()));
  }

  PolyNf operator()(PolyNf term, PolyNf* evaluatedArgs) 
  {
    CALL("EvaluateMonom::operator()")
    return EvaluatePolynom<EvaluateMonom>{*this}(term, evaluatedArgs);
  }
};

template<class A>
class FlatMeetLattice 
{
  using Inner = Coproduct<A, Bot>;
  Inner _inner;
  using Monom = Kernel::Monom<RealTraits>;
  using Const = RealConstantType;
  using MonomFactors = Kernel::MonomFactors<RealTraits>;

private:
  FlatMeetLattice(Bot b) : _inner(b) {}
public:
  static FlatMeetLattice bot() { return FlatMeetLattice(Bot{}); }

  FlatMeetLattice(A c) : _inner(c) {}

  FlatMeetLattice meet(FlatMeetLattice rhs) 
  {
    auto& lhs = *this;

    if (lhs._inner.template is<Bot>()) return bot();
    if (rhs._inner.template is<Bot>()) return bot();

    return meet(lhs._inner.template unwrap<A>(), rhs._inner.template unwrap<A>());
  }

  bool isBot() const 
  {return _inner.template is<Bot>(); }

  A const& unwrap() const
  { return _inner.template unwrap<A>(); }


  A      & unwrap()
  { return _inner.template unwrap<A>(); }

  friend ostream& operator<<(ostream& out, FlatMeetLattice const& self) 
  { return out << self._inner; }

private:
  static FlatMeetLattice meet(A lhs, A rhs) {
    if(lhs == rhs) return FlatMeetLattice(lhs);
    else return bot();
  }
};

template<class C>
Stack<C> intersectSortedStack(Stack<C>&& l, Stack<C>&& r) 
{
  CALL("intersectSortedStack")
  // DEBUG("lhs: ", l)
  // DEBUG("rhs: ", r)

  if (l.size() == 0) return std::move(l);
  if (r.size() == 0) return std::move(r);

  auto outOffs = 0;
  auto& out = l.size() <= r.size() ? l : r;
  unsigned loffs = 0;
  unsigned roffs = 0;
  while (loffs < l.size() && roffs < r.size()) {
    if (l[loffs] == r[roffs]) {
      out[outOffs++] = l[loffs];
      loffs++;
      roffs++;
    } else if(l[loffs] < r[roffs]) {
      loffs++;
    } else {
      roffs++;
    }
  }
  
  out.truncate(outOffs);
  //DEBUG("out: ", out);
  return std::move(out);
}


#include "ArithmeticSubtermGeneralization/NumeralMultiplicationGeneralizationImpl.cpp"
#include "ArithmeticSubtermGeneralization/AdditionGeneralizationImpl.cpp"
#include "ArithmeticSubtermGeneralization/VariableMultiplicationGeneralizationImpl.cpp"
#include "ArithmeticSubtermGeneralization/VariablePowerGeneralizationImpl.cpp"

SimplifyingGeneratingInference1::Result AdditionGeneralization::simplify(Clause* cl, bool doOrderingCheck) 
{ 
  CALL("AdditionGeneralization::simplify")
  return AdditionGeneralizationImpl::applyRule(cl,doOrderingCheck);
}

AdditionGeneralization::~AdditionGeneralization()  {}


SimplifyingGeneratingInference1::Result NumeralMultiplicationGeneralization::simplify(Clause* cl, bool doOrderingCheck) 
{ 
  CALL("NumeralMultiplicationGeneralization::simplify")
  return NumeralMultiplicationGeneralizationImpl::applyRule(cl, doOrderingCheck);
}

NumeralMultiplicationGeneralization::~NumeralMultiplicationGeneralization()  {}
SimplifyingGeneratingInference1::Result VariableMultiplicationGeneralization::simplify(Clause* cl, bool doOrderingCheck) 
{ 
  CALL("VariableMultiplicationGeneralization::simplify")
  return VariableMultiplicationGeneralizationImpl::applyRule(cl, doOrderingCheck);
}

VariableMultiplicationGeneralization::~VariableMultiplicationGeneralization()  { }


SimplifyingGeneratingInference1::Result VariablePowerGeneralization::simplify(Clause* cl, bool doOrderingCheck) 
{ 
  CALL("VariablePowerGeneralization::simplify")
  return VariablePowerGeneralizationImpl::applyRule(cl, doOrderingCheck);
}

VariablePowerGeneralization::~VariablePowerGeneralization()  {}

Stack<SimplifyingGeneratingInference1*> allArithmeticSubtermGeneralizations()
{ 
  return Stack<SimplifyingGeneratingInference1*> {
      new VariableMultiplicationGeneralization(),
      new VariablePowerGeneralization(),
      new NumeralMultiplicationGeneralization(),
      new AdditionGeneralization()
  };
}

} // Inferences
