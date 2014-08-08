/**
 * @file DPTester.cpp
 * Implements class DPTester.
 */

#include "Debug/Tracer.hpp"

#include "DP/SimpleCongruenceClosure.hpp"

#include "Lib/Environment.hpp"
#include "Lib/ScopedPtr.hpp"
#include "Lib/Stack.hpp"

#include "Kernel/Clause.hpp"
#include "Kernel/Problem.hpp"

#include "Shell/Options.hpp"
#include "Shell/Preprocess.hpp"
#include "Shell/UIHelper.hpp"

#include "DPTester.hpp"

namespace VUtils
{


int DPTester::perform(int argc, char** argv)
{
  CALL("DPTester::perform");

  vstring fname = argv[2];
  env.options->setInputFile(fname);

  cout << "solving "<<fname<<endl;

  env.options->setUnusedPredicateDefinitionRemoval(false);
  ScopedPtr<Problem> prb(UIHelper::getInputProblem(*env.options));

  Preprocess(*env.options).preprocess(*prb);

  LiteralStack lits;

  ClauseIterator cit = prb->clauseIterator();
  while(cit.hasNext()) {
    Clause* cl = cit.next();
    if(cl->length()!=1) {
      USER_ERROR("non-unit clause: "+cl->toString());
    }
    Literal* lit = (*cl)[0];
    lits.push(lit);
    cout << (*lit) << endl;
  }

  ScopedPtr<DecisionProcedure> dp(new SimpleCongruenceClosure());

  dp->addLiterals(pvi(LiteralStack::Iterator(lits)));

  switch(dp->getStatus()) {
  case DecisionProcedure::SATISFIABLE:
    cout << "SAT" << endl;
    break;
  case DecisionProcedure::UNSATISFIABLE:
    cout << "UNSAT" << endl;
    break;
  case DecisionProcedure::UNKNOWN:
    cout << "UNKNOWN" << endl;
    break;
  }


  return 0;
}


}
