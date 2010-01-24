/**
 * @file vampire.cpp. Implements the top-level procedures of Vampire.
 */

#include <string>
#include <iostream>
#include <ostream>
#include <fstream>
#include <csignal>

#include "Debug/Tracer.hpp"

#include "Lib/Exception.hpp"
#include "Lib/Environment.hpp"
#include "Lib/Int.hpp"
#include "Lib/Random.hpp"
#include "Lib/Set.hpp"
#include "Lib/TimeCounter.hpp"
#include "Lib/Timer.hpp"

#include "Lib/List.hpp"
#include "Lib/Vector.hpp"
#include "Lib/System.hpp"
#include "Lib/Metaiterators.hpp"

#include "Kernel/Signature.hpp"
#include "Kernel/Clause.hpp"
#include "Kernel/Formula.hpp"
#include "Kernel/FormulaUnit.hpp"
#include "Kernel/InferenceStore.hpp"

#include "Indexing/TermSharing.hpp"
#include "Indexing/SubstitutionTree.hpp"
#include "Indexing/LiteralMiniIndex.hpp"

#include "Shell/CommandLine.hpp"
#include "Shell/Grounding.hpp"
#include "Shell/Interpolants.hpp"
#include "Shell/LaTeX.hpp"
#include "Shell/LispLexer.hpp"
#include "Shell/LispParser.hpp"
#include "Shell/Options.hpp"
#include "Shell/Property.hpp"
#include "Shell/Preprocess.hpp"
#include "Shell/Refutation.hpp"
#include "Shell/TheoryFinder.hpp"
#include "Shell/SimplifyProver.hpp"
#include "Shell/Statistics.hpp"
#include "Shell/TPTPLexer.hpp"
#include "Shell/TPTP.hpp"
#include "Shell/TPTPParser.hpp"

#include "Saturation/SaturationAlgorithm.hpp"

#include "SAT/DIMACS.hpp"
#include "SAT/SATClause.hpp"


#if CHECK_LEAKS
#include "Lib/MemoryLeak.hpp"
#endif

#define SPIDER 0
#define SAVE_SPIDER_PROPERTIES 0

using namespace Shell;
using namespace SAT;
using namespace Saturation;
using namespace Inferences;

UnitList* globUnitList=0;

ClauseIterator getInputClauses()
{
  Property property;

  env.signature = new Kernel::Signature;
  UnitList* units;
  {
    TimeCounter tc1(TC_PARSING);

    string inputFile = env.options->inputFile();

    istream* input;
    if(inputFile=="") {
      input=&cin;
    } else {
      input=new ifstream(inputFile.c_str());
    }


    switch (env.options->inputSyntax()) {
    case Options::IS_SIMPLIFY:
    {
      Shell::LispLexer lexer(*input);
      Shell::LispParser parser(lexer);
      LispParser::Expression* expr = parser.parse();
      SimplifyProver simplify;
      units = simplify.units(expr);
    }
    break;
    case Options::IS_TPTP:
    {
      TPTPLexer lexer(*input);
      TPTPParser parser(lexer);
      units = parser.units();
    }
    break;
    }

    if(inputFile!="") {
      delete static_cast<ifstream*>(input);
      input=0;
    }
  }

  TimeCounter tc2(TC_PREPROCESSING);

  property.scan(units);
  Preprocess prepro(property,*env.options);
  prepro.preprocess(units);
  globUnitList=units;

  return pvi( getStaticCastIterator<Clause*>(UnitList::Iterator(units)) );
}

void doProving()
{
  CALL("doProving()");
  try {
    ClauseIterator clauses=getInputClauses();
    SaturationAlgorithmSP salg=SaturationAlgorithm::createFromOptions();
    salg->addInputClauses(clauses);

    SaturationResult sres(salg->saturate());
    sres.updateStatistics();
  }
  catch(MemoryLimitExceededException) {
    env.statistics->terminationReason=Statistics::MEMORY_LIMIT;
    env.statistics->refutation=0;
    size_t limit=Allocator::getMemoryLimit();
    //add extra 1 MB to allow proper termination
    Allocator::setMemoryLimit(limit+1000000);
  }
  catch(TimeLimitExceededException) {
    env.statistics->terminationReason=Statistics::TIME_LIMIT;
    env.statistics->refutation=0;
  }
}

void outputResult()
{
  CALL("outputResult()");

  switch (env.statistics->terminationReason) {
  case Statistics::REFUTATION:
    env.out << "Refutation found. Thanks to "
	    << env.options->thanks() << "!\n";
    if (env.options->proof() != Options::PROOF_OFF) {
//	Shell::Refutation refutation(env.statistics->refutation,
//		env.options->proof() == Options::PROOF_ON);
//	refutation.output(env.out);
      InferenceStore::instance()->outputProof(env.out, env.statistics->refutation);
    }
    if(env.options->showInterpolant()) {
      ASS(env.statistics->refutation->isClause());
      Formula* interpolant=Interpolants::getInterpolant(static_cast<Clause*>(env.statistics->refutation));
      env.out << "Interpolant: " << interpolant->toString() << endl;
    }
    if(env.options->latexOutput()!="off") {
      ofstream latexOut(env.options->latexOutput().c_str());

      LaTeX formatter;
      latexOut<<formatter.refutationToString(env.statistics->refutation);
    }
    break;
  case Statistics::TIME_LIMIT:
    env.out << "Time limit reached!\n";
    break;
  case Statistics::MEMORY_LIMIT:
#if VDEBUG
    Allocator::reportUsageByClasses();
#endif
    env.out << "Memory limit exceeded!\n";
    break;
  case Statistics::UNKNOWN:
    if(env.options->complete()) {
      ASS_G(env.statistics->discardedNonRedundantClauses, 0);
      env.out << "Refutation not found, some non-redundant clauses were discarded!\n";
    } else {
      env.out << "Refutation not found with incomplete strategy!\n";
    }
    break;
  default:
    env.out << "Refutation not found!\n";
    break;
  }
  env.statistics->print();
  if(env.options->timeStatistics()) {
    TimeCounter::printReport();
  }

}

/**
 * Read a problem and output profiling information about it.
 * @since 03/08/2008 Torrevieja
 */
void profileMode()
{
  CALL("profileMode()");

  Property property;
  env.signature = new Kernel::Signature;
  UnitList* units;
  string inputFile = env.options->inputFile();
  istream* input;
  if(inputFile=="") {
    input=&cin;
  }
  else {
    input=new ifstream(inputFile.c_str());
  }

  TPTPLexer lexer(*input);
  TPTPParser parser(lexer);
  units = parser.units();
  if(inputFile!="") {
    delete static_cast<ifstream*>(input);
    input=0;
  }

  property.scan(units);
  TheoryFinder tf(units,&property);
  Preprocess prepro(property,*env.options);
  tf.search();
  cout << property.categoryString() << ' '
       << property.props() << ' '
       << property.atoms() << "\n";
} // profileMode

void vampireMode()
{
  CALL("vampireMode()");
  env.out<<env.options->testId()<<" on "<<env.options->problemName()<<endl;
  doProving();
  outputResult();
} // vampireMode


void spiderMode()
{
  CALL("spiderMode()");
  bool noException=true;
  try {
    doProving();
  }
  catch(...) {
    noException=false;
  }

  if(noException) {
    switch (env.statistics->terminationReason) {
    case Statistics::REFUTATION:
      reportSpiderStatus('+');
      break;
    case Statistics::TIME_LIMIT:
    case Statistics::MEMORY_LIMIT:
    case Statistics::UNKNOWN:
      reportSpiderStatus('?');
      break;
    default:
      reportSpiderStatus('-');
      break;
    }
    env.statistics->print();
  }
  else {
    reportSpiderFail();
  }
} // spiderMode

void clausifyMode()
{
  CALL("clausifyMode()");

  ClauseIterator cit = getInputClauses();
  while (cit.hasNext()) {
    cout << TPTP::toString(cit.next()) << "\n";
  }
} // clausifyMode

void explainException (Exception& exception)
{
  exception.cry(env.out);
} // explainException

/**
 * The main function.
  * @since 03/12/2003 many changes related to logging
  *        and exception handling.
  * @since 10/09/2004, Manchester changed to use knowledge bases
  */
int main(int argc, char* argv [])
{
  CALL ("main");

  Timer::initTimer();

  System::setSignalHandlers();
   // create random seed for the random number generation
  Lib::Random::setSeed(123456);

  try {
    // read the command line and interpret it
    Options options;
    Shell::CommandLine cl(argc,argv);
    cl.interpret(options);
    Allocator::setMemoryLimit(options.memoryLimit()*1048576);
    Lib::Random::setSeed(options.randomSeed());

    Timer timer;
    timer.start();
    env.timer = &timer;
    Indexing::TermSharing sharing;
    env.sharing = &sharing;
    env.options = &options;
    Shell::Statistics statistics;
    env.statistics = &statistics;

    switch (options.mode())
    {
    case Options::MODE_GROUNDING:
      USER_ERROR("Use the vground executable for grounding.");
      break;
    case Options::MODE_SPIDER:
      spiderMode();
      break;
    case Options::MODE_VAMPIRE:
      vampireMode();
      break;
    case Options::MODE_CASC:
      USER_ERROR("CASC mode is not implemented");
      break;
    case Options::MODE_CLAUSIFY:
      clausifyMode();
      break;
    case Options::MODE_PROFILE:
      profileMode();
      break;
    case Options::MODE_RULE:
      USER_ERROR("Rule mode is not implemented");
      break;
#if VDEBUG
    default:
      ASSERTION_VIOLATION;
#endif
    }
#if CHECK_LEAKS
    if (globUnitList) {
      MemoryLeak leak;
      leak.release(globUnitList);
    }
    delete env.signature;
    env.signature = 0;
#endif
  }
#if VDEBUG
  catch (Debug::AssertionViolationException& exception) {
#if CHECK_LEAKS
    MemoryLeak::cancelReport();
#endif
  }
#endif
  catch (UserErrorException& exception) {
#if CHECK_LEAKS
    MemoryLeak::cancelReport();
#endif
    explainException(exception);
  }
  catch (Exception& exception) {
#if CHECK_LEAKS
    MemoryLeak::cancelReport();
#endif
    explainException(exception);
    env.statistics->print();
  }
  catch (std::bad_alloc& _) {
#if CHECK_LEAKS
    MemoryLeak::cancelReport();
#endif
    env.out << "Insufficient system memory" << '\n';
  }
//   delete env.allocator;

  /*cout<<"good:\t"<<LiteralMiniIndex::goodPred<<endl;
  cout<<"bad:\t"<<LiteralMiniIndex::badPred<<endl;*/
  return EXIT_SUCCESS;
} // main

