/*
 * This file is part of the source code of the software program
 * Vampire. It is protected by applicable
 * copyright laws.
 *
 * This source code is distributed under the licence found here
 * https://vprover.github.io/license.html
 * and in the source directory
 */

#ifndef __ALASCA_Inferences_TautologyDeletion__
#define __ALASCA_Inferences_TautologyDeletion__

#include "Forwards.hpp"

#include "Kernel/ALASCA.hpp"

#define DEBUG(...) // DBG(__VA_ARGS__)

namespace Inferences {
namespace ALASCA {

using namespace Kernel;
using namespace Indexing;
using namespace Saturation;

class InequalityTautologyDetection
: public SimplifyingGeneratingInference
{
public:
  USE_ALLOCATOR(InequalityTautologyDetection);

  InequalityTautologyDetection(std::shared_ptr<AlascaState> shared) 
    : _shared(std::move(shared)) {}
  virtual ~InequalityTautologyDetection() {}

  virtual ClauseGenerationResult generateSimplify(Clause* premise) override 
  {
    Map<AnyAlascaLiteral, bool> lits;
    TIME_TRACE("alasca tautology detection")
    for (auto lit : iterTraits(premise->iterLits())) {
      
      if (auto norm = _shared->norm().tryNormalizeInterpreted(lit)) {
        lits.insert(*norm, true);
        auto opposite = norm->apply([&](auto lit) { return AnyAlascaLiteral(lit.negation()); });
        if (lits.find(opposite)) {
          // std::cout << "bla" << std::endl;
          return ClauseGenerationResult {
            .clauses = ClauseIterator::getEmpty(),
            .premiseRedundant = true,
          };
        }
      }
    }

    return ClauseGenerationResult {
        .clauses = ClauseIterator::getEmpty(),
        .premiseRedundant = false,
      };
  }
private:
  std::shared_ptr<AlascaState> _shared;
};



#undef DEBUG
} // namespaceALASCA 
} // namespace Inferences

#endif /*__ALASCA_Inferences_TautologyDeletion__*/
