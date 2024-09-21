/*
 * This file is part of the source code of the software program
 * Vampire. It is protected by applicable
 * copyright laws.
 *
 * This source code is distributed under the licence found here
 * https://vprover.github.io/license.html
 * and in the source directory
 */

#ifndef __LPOComparator__
#define __LPOComparator__

#include "Forwards.hpp"

#include "OrderingComparator.hpp"

namespace Kernel {

using namespace Lib;
using namespace std;

class LPOComparator
: public OrderingComparator
{
public:
  LPOComparator(const Ordering& ord, TermList lhs, TermList rhs)
    : OrderingComparator(ord, lhs, rhs) {}

  /** Executes the runtime specialized instructions with concrete substitution. */
  bool check(const SubstApplicator* applicator) override;

private:
  static void majoChain(Branch* branch, TermList tl1, Term* t, unsigned i, Branch success, Branch fail);
  static void alphaChain(Branch* branch, Term* s, unsigned i, TermList tl2, Branch success, Branch fail);
  static void expand(const Ordering& ord, Branch& branch, const Stack<TermPairRes>& cache);
};

}
#endif
