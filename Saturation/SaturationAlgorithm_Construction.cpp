/**
 * @file SaturationAlgorithm_Construction.cpp
 * Implements class SaturationAlgorithm::createFromOptions.
 */

#include "Lib/Exception.hpp"

#include "Kernel/KBO.hpp"
#include "Kernel/LiteralSelector.hpp"
#include "Kernel/Signature.hpp"

#include "Shell/Options.hpp"

#include "Inferences/InferenceEngine.hpp"
#include "Inferences/BackwardDemodulation.hpp"
#include "Inferences/BackwardSubsumptionResolution.hpp"
#include "Inferences/BDDMarkingSubsumption.hpp"
#include "Inferences/BinaryResolution.hpp"
#include "Inferences/CTFwSubsAndRes.hpp"
#include "Inferences/EqualityFactoring.hpp"
#include "Inferences/EqualityResolution.hpp"
#include "Inferences/Factoring.hpp"
#include "Inferences/ForwardDemodulation.hpp"
#include "Inferences/ForwardLiteralRewriting.hpp"
#include "Inferences/ForwardSubsumptionAndResolution.hpp"
#include "Inferences/GlobalSubsumption.hpp"
#include "Inferences/InterpretedSimplifier.hpp"
#include "Inferences/RefutationSeekerFSE.hpp"
#include "Inferences/SLQueryForwardSubsumption.hpp"
#include "Inferences/SLQueryBackwardSubsumption.hpp"
#include "Inferences/Superposition.hpp"
#include "Inferences/URResolution.hpp"

#include "Test/CheckedFwSimplifier.hpp"


#include "AWPassiveClauseContainer.hpp"
#include "SaturationAlgorithm.hpp"

#include "Discount.hpp"
#include "LRS.hpp"
#include "Otter.hpp"

#include "BSplitter.hpp"
#include "SWBSplitterWithBDDs.hpp"
#include "SWBSplitterWithoutBDDs.hpp"

#include "ConsequenceFinder.hpp"
#include "SymElOutput.hpp"

namespace Saturation
{

using namespace Lib;
using namespace Kernel;
using namespace Shell;
using namespace Inferences;

namespace Construction {

GeneratingInferenceEngineSP createGIE()
{
  CALL("Saturation::Construction::createGIE");

  CompositeGIE* res=new CompositeGIE();

  res->addFront(GeneratingInferenceEngineSP(new EqualityFactoring()));
  res->addFront(GeneratingInferenceEngineSP(new EqualityResolution()));
  res->addFront(GeneratingInferenceEngineSP(new Superposition()));
  res->addFront(GeneratingInferenceEngineSP(new Factoring()));
  if(env.options->binaryResolution()) {
    res->addFront(GeneratingInferenceEngineSP(new BinaryResolution()));
  }
  if(env.options->unitResultingResolution()) {
    res->addFront(GeneratingInferenceEngineSP(new URResolution()));
  }


  return GeneratingInferenceEngineSP(res);
}


void addFSEs(SaturationAlgorithm* alg)
{
  CALL("Saturation::Construction::addFSEs");

//  alg->addForwardSimplifierToFront(ForwardSimplificationEngineSP(new RefutationSeekerFSE()));

  if(env.options->globalSubsumption()) {
    alg->addForwardSimplifierToFront(ForwardSimplificationEngineSP(new GlobalSubsumption()));
  }

  if(env.options->interpretedSimplification()) {
    alg->addForwardSimplifierToFront(ForwardSimplificationEngineSP(new InterpretedSimplifier()));
  }

  if(env.options->forwardLiteralRewriting()) {
    alg->addForwardSimplifierToFront(ForwardSimplificationEngineSP(new ForwardLiteralRewriting()));
  }

  switch(env.options->forwardDemodulation()) {
  case Options::DEMODULATION_ALL:
  case Options::DEMODULATION_PREORDERED:
    alg->addForwardSimplifierToFront(ForwardSimplificationEngineSP(new ForwardDemodulation()));
    break;
  case Options::DEMODULATION_OFF:
    break;
#if VDEBUG
  default:
    ASSERTION_VIOLATION;
#endif
  }

  if(env.options->forwardSubsumption()) {

    if(env.options->forwardSubsumptionResolution()) {
      alg->addForwardSimplifierToFront(ForwardSimplificationEngineSP(
//	  new ForwardSubsumptionAndResolution(true) ));
//	  new ForwardSubsumptionAndResolution(false) ));
	  new CTFwSubsAndRes(true) ));
//	  new Test::CheckedFwSimplifier(new CTFwSubsAndRes(true), new ForwardSubsumptionAndResolution(true)) ));
    }
    else {
      alg->addForwardSimplifierToFront(ForwardSimplificationEngineSP(
	  new CTFwSubsAndRes(false) ));
//	  new ForwardSubsumptionAndResolution(false) ));
//	  new Test::CheckedFwSimplifier(new CTFwSubsAndRes(false), new ForwardSubsumptionAndResolution(false)) ));
    }
//    alg->addForwardSimplifierToFront(ForwardSimplificationEngineSP(
//	    new ForwardSubsumptionAndResolution(
//		    env.options->forwardSubsumptionResolution()) ));
//    alg->addForwardSimplifierToFront(ForwardSimplificationEngineSP(
//	    new SLQueryForwardSubsumption() ));
  } else if(env.options->forwardSubsumptionResolution()) {
    USER_ERROR("Forward subsumption resolution requires forward subsumption to be enabled.");
  }
}


void addBSEs(SaturationAlgorithm* alg)
{
  CALL("Saturation::Construction::addBSEs");

  switch(env.options->backwardDemodulation()) {
  case Options::DEMODULATION_ALL:
  case Options::DEMODULATION_PREORDERED:
    alg->addBackwardSimplifierToFront(BackwardSimplificationEngineSP(new BackwardDemodulation()));
    break;
  case Options::DEMODULATION_OFF:
    break;
#if VDEBUG
  default:
    ASSERTION_VIOLATION;
#endif
  }

  if(env.options->backwardSubsumption()!=Options::SUBSUMPTION_OFF) {
    bool byUnitsOnly=env.options->backwardSubsumption()==Options::SUBSUMPTION_UNIT_ONLY;
    alg->addBackwardSimplifierToFront(BackwardSimplificationEngineSP(new SLQueryBackwardSubsumption(byUnitsOnly)));
  }
  if(env.options->backwardSubsumptionResolution()!=Options::SUBSUMPTION_OFF) {
    bool byUnitsOnly=env.options->backwardSubsumptionResolution()==Options::SUBSUMPTION_UNIT_ONLY;
    alg->addBackwardSimplifierToFront(BackwardSimplificationEngineSP(new BackwardSubsumptionResolution(byUnitsOnly)));
  }
}

PassiveClauseContainer* createPassiveContainer()
{
  CALL("Saturation::Construction::createPassiveContainer");

  AWPassiveClauseContainer* res=new AWPassiveClauseContainer();
  res->setAgeWeightRatio(env.options->ageRatio(),env.options->weightRatio());
  return res;
}

};

using namespace Construction;

SaturationAlgorithmSP SaturationAlgorithm::createFromOptions()
{
  CALL("SaturationAlgorithm::createFromOptions");

  bool epr=true;
  unsigned func=env.signature->functions();
  for(unsigned fi=0;fi<func;fi++) {
    if(env.signature->functionArity(fi)) {
      epr=false;
      break;
    }
  }
  Ordering::create(epr);

  PassiveClauseContainer* passive=createPassiveContainer();
  LiteralSelector* selector=LiteralSelector::getSelector(env.options->selection());

  SaturationAlgorithm* res;
  switch(env.options->saturationAlgorithm()) {
  case Shell::Options::DISCOUNT:
    res=new Discount(passive, selector);
    break;
  case Shell::Options::LRS:
    res=new LRS(passive, selector);
    break;
  case Shell::Options::OTTER:
    res=new Otter(passive, selector);
    break;
  default:
    NOT_IMPLEMENTED;
  }

  res->setGeneratingInferenceEngine(createGIE());
  res->setImmediateSimplificationEngine(createISE());
  addFSEs(res);
  addBSEs(res);

  if(env.options->mode()==Options::MODE_CONSEQUENCE_ELIMINATION) {
    res->_consFinder=new ConsequenceFinder();
  }
  if(env.options->showSymbolElimination()) {
    res->_symEl=new SymElOutput();
  }

  if(env.options->splitting()==Options::SM_BACKTRACKING) {
    res->_splitter=new BSplitter();
  }
  else if(env.options->splitting()==Options::SM_NOBACKTRACKING) {
    if(env.options->propositionalToBDD()) {
      res->_splitter=new SWBSplitterWithBDDs();
    }
    else {
      res->_splitter=new SWBSplitterWithoutBDDs();
    }
  }


  if(env.options->bddMarkingSubsumption()) {
    res->_bddMarkingSubsumption=new BDDMarkingSubsumption();
  }

  return SaturationAlgorithmSP(res);
}

}
