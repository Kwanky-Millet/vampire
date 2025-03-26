################################################################
# sources only required for unit tests
################################################################
set(TESTING_SOURCES
    Test/BuilderPattern.hpp
    Test/GenerationTester.hpp
    Test/SimplificationTester.hpp
    Test/SyntaxSugar.cpp
    Test/SyntaxSugar.hpp
    Test/TermIndexTester.hpp
    Test/TestUtils.cpp
    Test/TestUtils.hpp
    Test/UnitTesting.cpp
    Test/UnitTesting.hpp
)

################################################################
# unit tests
################################################################
set(UNIT_TESTS
    UnitTests/tALASCA_Abstraction.cpp
    UnitTests/tALASCA_Coherence.cpp
    UnitTests/tALASCA_CoherenceNormalization.cpp
    UnitTests/tALASCA_Demodulation.cpp
    UnitTests/tALASCA_EqFactoring.cpp
    UnitTests/tALASCA_FloorElimination.cpp
    UnitTests/tALASCA_FourierMotzkin.cpp
    UnitTests/tALASCA_InequalityFactoring.cpp
    UnitTests/tALASCA_IntegerFourierMotzkin.cpp
    UnitTests/tALASCA_LaKbo.cpp
    UnitTests/tALASCA_Normalization.cpp
    UnitTests/tALASCA_Superposition.cpp
    UnitTests/tALASCA_TautologyDeletion.cpp
    UnitTests/tALASCA_TermFactoring.cpp
    UnitTests/tALASCA_VIRAS.cpp
    UnitTests/tALASCA_VariableElimination.cpp
    UnitTests/tArithCompare.cpp
    UnitTests/tArithmeticSubtermGeneralization.cpp
    UnitTests/tBinaryHeap.cpp
    UnitTests/tBottomUpEvaluation.cpp
    UnitTests/tCoproduct.cpp
    UnitTests/tDHMap.cpp
    UnitTests/tDHMultiset.cpp
    UnitTests/tDeque.cpp
    UnitTests/tDisagreement.cpp
    UnitTests/tDynamicHeap.cpp
    UnitTests/tFunctionDefinitionHandler.cpp
    UnitTests/tFunctionDefinitionRewriting.cpp
    UnitTests/tGaussianElimination.cpp
    UnitTests/tInduction.cpp
    UnitTests/tIntegerConstantType.cpp
    UnitTests/tInterpretedFunctions.cpp
    UnitTests/tIterator.cpp
    UnitTests/tKBO.cpp
    UnitTests/tLPO.cpp
    UnitTests/tList.cpp
    UnitTests/tOption.cpp
    UnitTests/tOptionConstraints.cpp
    UnitTests/tPushUnaryMinus.cpp
    UnitTests/tQKbo.cpp
    UnitTests/tQuotientE.cpp
    UnitTests/tRatioKeeper.cpp
    UnitTests/tRebalance.cpp
    UnitTests/tSATSolver.cpp
    UnitTests/tSATSubsumptionResolution.cpp
    UnitTests/tSKIKBO.cpp
    UnitTests/tSafeRecursion.cpp
    UnitTests/tSet.cpp
    UnitTests/tSkipList.cpp
    UnitTests/tStack.cpp
    UnitTests/tSyntaxSugar.cpp
    UnitTests/tTermAlgebra.cpp
    UnitTests/tTermIndex.cpp
    UnitTests/tUnificationWithAbstraction.cpp
)

################################################################
# unit tests (only if Z3 enabled)
################################################################
set(UNIT_TESTS_Z3
    UnitTests/tTheoryInstAndSimp.cpp
    UnitTests/tZ3Interfacing.cpp
)

################################################################
# everything else
################################################################
set(SOURCES
    CASC/PortfolioMode.cpp
    CASC/PortfolioMode.hpp
    CASC/Schedules.cpp
    CASC/Schedules.hpp
    DP/DecisionProcedure.hpp
    DP/ShortConflictMetaDP.cpp
    DP/ShortConflictMetaDP.hpp
    DP/SimpleCongruenceClosure.cpp
    DP/SimpleCongruenceClosure.hpp
    Debug/Assertion.cpp
    Debug/Assertion.hpp
    Debug/RuntimeStatistics.cpp
    Debug/RuntimeStatistics.hpp
    Debug/TimeProfiling.cpp
    Debug/TimeProfiling.hpp
    Debug/Tracer.cpp
    Debug/Tracer.hpp
    FMB/ClauseFlattening.cpp
    FMB/ClauseFlattening.hpp
    FMB/DefinitionIntroduction.hpp
    FMB/FiniteModel.cpp
    FMB/FiniteModel.hpp
    FMB/FiniteModelBuilder.cpp
    FMB/FiniteModelBuilder.hpp
    FMB/FiniteModelMultiSorted.cpp
    FMB/FiniteModelMultiSorted.hpp
    FMB/FunctionRelationshipInference.cpp
    FMB/FunctionRelationshipInference.hpp
    FMB/ModelCheck.hpp
    FMB/Monotonicity.cpp
    FMB/Monotonicity.hpp
    FMB/SortInference.cpp
    FMB/SortInference.hpp
    Indexing/AcyclicityIndex.cpp
    Indexing/AcyclicityIndex.hpp
    Indexing/ClauseCodeTree.cpp
    Indexing/ClauseCodeTree.hpp
    Indexing/ClauseVariantIndex.cpp
    Indexing/ClauseVariantIndex.hpp
    Indexing/CodeTree.cpp
    Indexing/CodeTree.hpp
    Indexing/CodeTreeInterfaces.cpp
    Indexing/CodeTreeInterfaces.hpp
    Indexing/Index.cpp
    Indexing/Index.hpp
    Indexing/IndexManager.cpp
    Indexing/IndexManager.hpp
    Indexing/InductionFormulaIndex.cpp
    Indexing/InductionFormulaIndex.hpp
    Indexing/LiteralIndex.cpp
    Indexing/LiteralIndex.hpp
    Indexing/LiteralIndexingStructure.hpp
    Indexing/LiteralMiniIndex.cpp
    Indexing/LiteralMiniIndex.hpp
    Indexing/LiteralSubstitutionTree.hpp
    Indexing/ResultSubstitution.cpp
    Indexing/ResultSubstitution.hpp
    Indexing/SubstitutionTree.hpp
    Indexing/TermCodeTree.cpp
    Indexing/TermCodeTree.hpp
    Indexing/TermIndex.cpp
    Indexing/TermIndex.hpp
    Indexing/TermIndexingStructure.hpp
    Indexing/TermSharing.cpp
    Indexing/TermSharing.hpp
    Indexing/TermSubstitutionTree.hpp
    Inferences/ALASCA/BwdDemodulation.cpp
    Inferences/ALASCA/BwdDemodulation.hpp
    Inferences/ALASCA/Demodulation.cpp
    Inferences/ALASCA/Demodulation.hpp
    Inferences/ALASCA/EqFactoring.cpp
    Inferences/ALASCA/EqFactoring.hpp
    Inferences/ALASCA/FourierMotzkin.cpp
    Inferences/ALASCA/FourierMotzkin.hpp
    Inferences/ALASCA/FwdDemodulation.cpp
    Inferences/ALASCA/FwdDemodulation.hpp
    Inferences/ALASCA/InequalityFactoring.cpp
    Inferences/ALASCA/InequalityFactoring.hpp
    Inferences/ALASCA/Normalization.cpp
    Inferences/ALASCA/Normalization.hpp
    Inferences/ALASCA/Superposition.cpp
    Inferences/ALASCA/Superposition.hpp
    Inferences/ALASCA/TermFactoring.cpp
    Inferences/ALASCA/TermFactoring.hpp
    Inferences/ALASCA/VIRAS.cpp
    Inferences/ALASCA/VIRAS.hpp
    Inferences/ALASCA/VariableElimination.cpp
    Inferences/ALASCA/VariableElimination.hpp
    Inferences/ArgCong.cpp
    Inferences/ArgCong.hpp
    Inferences/ArithmeticSubtermGeneralization.cpp
    Inferences/ArithmeticSubtermGeneralization.cpp
    Inferences/ArithmeticSubtermGeneralization.hpp
    Inferences/ArithmeticSubtermGeneralization.hpp
    Inferences/BackwardDemodulation.cpp
    Inferences/BackwardDemodulation.hpp
    Inferences/BackwardSubsumptionAndResolution.cpp
    Inferences/BackwardSubsumptionAndResolution.hpp
    Inferences/BackwardSubsumptionDemodulation.cpp
    Inferences/BinaryResolution.cpp
    Inferences/BinaryResolution.hpp
    Inferences/BoolEqToDiseq.cpp
    Inferences/BoolEqToDiseq.hpp
    Inferences/BoolSimp.cpp
    Inferences/BoolSimp.hpp
    Inferences/CNFOnTheFly.cpp
    Inferences/CNFOnTheFly.hpp
    Inferences/Cancellation.cpp
    Inferences/Cancellation.cpp
    Inferences/Cancellation.hpp
    Inferences/Cancellation.hpp
    Inferences/Cases.cpp
    Inferences/Cases.hpp
    Inferences/CasesSimp.cpp
    Inferences/CasesSimp.hpp
    Inferences/Choice.cpp
    Inferences/Choice.hpp
    Inferences/CodeTreeForwardSubsumptionAndResolution.cpp
    Inferences/CodeTreeForwardSubsumptionAndResolution.hpp
    Inferences/CombinatorDemodISE.cpp
    Inferences/CombinatorDemodISE.hpp
    Inferences/CombinatorNormalisationISE.cpp
    Inferences/CombinatorNormalisationISE.hpp
    Inferences/Condensation.cpp
    Inferences/Condensation.hpp
    Inferences/DefinitionIntroduction.cpp
    Inferences/DefinitionIntroduction.hpp
    Inferences/DemodulationHelper.cpp
    Inferences/DemodulationHelper.hpp
    Inferences/DistinctEqualitySimplifier.cpp
    Inferences/DistinctEqualitySimplifier.hpp
    Inferences/ElimLeibniz.cpp
    Inferences/ElimLeibniz.hpp
    Inferences/EqualityFactoring.cpp
    Inferences/EqualityFactoring.hpp
    Inferences/EqualityResolution.cpp
    Inferences/EqualityResolution.hpp
    Inferences/EquationalTautologyRemoval.cpp
    Inferences/EquationalTautologyRemoval.hpp
    Inferences/ExtensionalityResolution.cpp
    Inferences/ExtensionalityResolution.hpp
    Inferences/FOOLParamodulation.cpp
    Inferences/FOOLParamodulation.hpp
    Inferences/Factoring.cpp
    Inferences/Factoring.hpp
    Inferences/FastCondensation.cpp
    Inferences/FastCondensation.hpp
    Inferences/ForwardDemodulation.cpp
    Inferences/ForwardDemodulation.hpp
    Inferences/ForwardLiteralRewriting.cpp
    Inferences/ForwardLiteralRewriting.hpp
    Inferences/ForwardSubsumptionAndResolution.cpp
    Inferences/ForwardSubsumptionAndResolution.hpp
    Inferences/ForwardSubsumptionDemodulation.cpp
    Inferences/FunctionDefinitionRewriting.cpp
    Inferences/FunctionDefinitionRewriting.hpp
    Inferences/GaussianVariableElimination.cpp
    Inferences/GaussianVariableElimination.hpp
    Inferences/GlobalSubsumption.cpp
    Inferences/GlobalSubsumption.hpp
    Inferences/Induction.cpp
    Inferences/InductionHelper.cpp
    Inferences/InductionHelper.hpp
    Inferences/InferenceEngine.cpp
    Inferences/InferenceEngine.hpp
    Inferences/Injectivity.cpp
    Inferences/Injectivity.hpp
    Inferences/InnerRewriting.cpp
    Inferences/InnerRewriting.hpp
    Inferences/Instantiation.cpp
    Inferences/Instantiation.hpp
    Inferences/InterpretedEvaluation.cpp
    Inferences/InterpretedEvaluation.hpp
    Inferences/InvalidAnswerLiteralRemovals.cpp
    Inferences/InvalidAnswerLiteralRemovals.hpp
    Inferences/Narrow.cpp
    Inferences/Narrow.hpp
    Inferences/NegativeExt.cpp
    Inferences/NegativeExt.hpp
    Inferences/PolynomialEvaluation.cpp
    Inferences/PolynomialEvaluation.hpp
    Inferences/PrimitiveInstantiation.cpp
    Inferences/PrimitiveInstantiation.hpp
    Inferences/ProofExtra.cpp
    Inferences/ProofExtra.hpp
    Inferences/PushUnaryMinus.cpp
    Inferences/PushUnaryMinus.hpp
    Inferences/SubVarSup.cpp
    Inferences/SubVarSup.hpp
    Inferences/SubsumptionDemodulationHelper.cpp
    Inferences/Superposition.cpp
    Inferences/Superposition.hpp
    Inferences/TautologyDeletionISE.cpp
    Inferences/TautologyDeletionISE.hpp
    Inferences/TermAlgebraReasoning.cpp
    Inferences/TermAlgebraReasoning.hpp
    Inferences/TheoryInstAndSimp.cpp
    Inferences/TheoryInstAndSimp.hpp
    Inferences/URResolution.cpp
    Inferences/URResolution.hpp
    Kernel/ALASCA.hpp
    Kernel/ALASCA/Index.hpp
    Kernel/ALASCA/Normalization.hpp
    Kernel/ALASCA/Ordering.hpp
    Kernel/ALASCA/Preprocessor.hpp
    Kernel/ALASCA/SelectionPrimitves.cpp
    Kernel/ALASCA/SelectionPrimitves.hpp
    Kernel/ALASCA/Signature.cpp
    Kernel/ALASCA/Signature.hpp
    Kernel/ALASCA/State.cpp
    Kernel/ALASCA/State.hpp
    Kernel/ApplicativeHelper.cpp
    Kernel/ApplicativeHelper.hpp
    Kernel/BestLiteralSelector.hpp
    Kernel/BottomUpEvaluation.hpp
    Kernel/Clause.cpp
    Kernel/Clause.hpp
    Kernel/ClauseQueue.cpp
    Kernel/ClauseQueue.hpp
    Kernel/ColorHelper.hpp
    Kernel/Connective.hpp
    Kernel/ELiteralSelector.cpp
    Kernel/ELiteralSelector.hpp
    Kernel/EqHelper.cpp
    Kernel/EqHelper.hpp
    Kernel/FlatTerm.cpp
    Kernel/FlatTerm.hpp
    Kernel/Formula.cpp
    Kernel/Formula.hpp
    Kernel/FormulaTransformer.cpp
    Kernel/FormulaTransformer.hpp
    Kernel/FormulaUnit.cpp
    Kernel/FormulaUnit.hpp
    Kernel/FormulaVarIterator.cpp
    Kernel/FormulaVarIterator.hpp
    Kernel/Grounder.cpp
    Kernel/Grounder.hpp
    Kernel/Inference.cpp
    Kernel/Inference.hpp
    Kernel/InferenceStore.cpp
    Kernel/InferenceStore.hpp
    Kernel/InterpretedLiteralEvaluator.cpp
    Kernel/InterpretedLiteralEvaluator.hpp
    Kernel/KBO.cpp
    Kernel/KBO.hpp
    Kernel/LPO.cpp
    Kernel/LPO.hpp
    Kernel/LiteralComparators.hpp
    Kernel/LiteralSelector.cpp
    Kernel/LiteralSelector.hpp
    Kernel/LookaheadLiteralSelector.cpp
    Kernel/LookaheadLiteralSelector.hpp
    Kernel/MLMatcher.cpp
    Kernel/MLMatcher.hpp
    Kernel/MLMatcherSD.cpp
    Kernel/MLVariant.cpp
    Kernel/MLVariant.hpp
    Kernel/MainLoop.cpp
    Kernel/MainLoop.hpp
    Kernel/Matcher.cpp
    Kernel/Matcher.hpp
    Kernel/MaximalLiteralSelector.cpp
    Kernel/MaximalLiteralSelector.hpp
    Kernel/NumTraits.cpp
    Kernel/NumTraits.cpp
    Kernel/OperatorType.cpp
    Kernel/OperatorType.hpp
    Kernel/Ordering.cpp
    Kernel/Ordering.hpp
    Kernel/Ordering_Equality.cpp
    Kernel/PartialOrdering.cpp
    Kernel/PartialOrdering.hpp
    Kernel/Polynomial.cpp
    Kernel/Polynomial.hpp
    Kernel/PolynomialNormalizer.cpp
    Kernel/PolynomialNormalizer.hpp
    Kernel/Problem.cpp
    Kernel/Problem.hpp
    Kernel/QKbo.cpp
    Kernel/QKbo.hpp
    Kernel/RCClauseStack.hpp
    Kernel/Rebalancing.cpp
    Kernel/Rebalancing.cpp
    Kernel/Rebalancing.cpp
    Kernel/Rebalancing.hpp
    Kernel/Rebalancing/Inverters.cpp
    Kernel/Rebalancing/Inverters.hpp
    Kernel/Renaming.cpp
    Kernel/Renaming.hpp
    Kernel/RndLiteralSelector.cpp
    Kernel/RndLiteralSelector.hpp
    Kernel/RobSubstitution.cpp
    Kernel/RobSubstitution.hpp
    Kernel/SKIKBO.cpp
    Kernel/SKIKBO.hpp
    Kernel/Signature.cpp
    Kernel/Signature.cpp
    Kernel/Signature.hpp
    Kernel/Signature.hpp
    Kernel/SortHelper.cpp
    Kernel/SortHelper.hpp
    Kernel/SpassLiteralSelector.cpp
    Kernel/SpassLiteralSelector.hpp
    Kernel/SubformulaIterator.cpp
    Kernel/SubformulaIterator.hpp
    Kernel/SubstHelper.hpp
    Kernel/Substitution.cpp
    Kernel/Substitution.hpp
    Kernel/Term.cpp
    Kernel/Term.hpp
    Kernel/TermIterators.cpp
    Kernel/TermIterators.hpp
    Kernel/TermOrderingDiagram.cpp
    Kernel/TermOrderingDiagram.hpp
    Kernel/TermOrderingDiagramKBO.cpp
    Kernel/TermOrderingDiagramKBO.hpp
    Kernel/TermOrderingDiagramKBO.hpp
    Kernel/TermOrderingDiagramLPO.cpp
    Kernel/TermOrderingDiagramLPO.hpp
    Kernel/TermPartialOrdering.cpp
    Kernel/TermPartialOrdering.hpp
    Kernel/TermTransformer.cpp
    Kernel/TermTransformer.hpp
    Kernel/Theory.cpp
    Kernel/Theory.hpp
    Kernel/UnificationWithAbstraction.cpp
    Kernel/UnificationWithAbstraction.hpp
    Kernel/Unit.cpp
    Kernel/Unit.hpp
    Lib/Allocator.cpp
    Lib/Allocator.hpp
    Lib/Array.hpp
    Lib/ArrayMap.hpp
    Lib/BacktrackIterators.hpp
    Lib/Backtrackable.hpp
    Lib/BinaryHeap.hpp
    Lib/BitUtils.hpp
    Lib/Comparison.hpp
    Lib/Counter.hpp
    Lib/DArray.hpp
    Lib/DHMap.cpp
    Lib/DHMap.hpp
    Lib/DHMultiset.hpp
    Lib/DHSet.hpp
    Lib/Deque.hpp
    Lib/DynamicHeap.hpp
    Lib/Environment.cpp
    Lib/Environment.hpp
    Lib/Event.cpp
    Lib/Event.hpp
    Lib/Exception.cpp
    Lib/Exception.hpp
    Lib/Hash.hpp
    Lib/Int.cpp
    Lib/Int.hpp
    Lib/IntNameTable.cpp
    Lib/IntNameTable.hpp
    Lib/IntUnionFind.cpp
    Lib/IntUnionFind.hpp
    Lib/InverseLookup.hpp
    Lib/List.hpp
    Lib/Map.hpp
    Lib/MaybeBool.hpp
    Lib/Metaiterators.hpp
    Lib/MultiCounter.hpp
    Lib/NameArray.cpp
    Lib/NameArray.hpp
    Lib/Numbering.hpp
    Lib/Output.hpp
    Lib/PairUtils.hpp
    Lib/Portability.hpp
    Lib/ProofExtra.hpp
    Lib/Random.cpp
    Lib/Random.hpp
    Lib/RatioKeeper.hpp
    Lib/Recycled.hpp
    Lib/Reflection.hpp
    Lib/SafeRecursion.hpp
    Lib/ScopedLet.hpp
    Lib/ScopedPtr.hpp
    Lib/Set.hpp
    Lib/SharedSet.hpp
    Lib/SkipList.hpp
    Lib/SmartPtr.hpp
    Lib/Sort.hpp
    Lib/Stack.hpp
    Lib/StringUtils.cpp
    Lib/StringUtils.hpp
    Lib/Sys/Multiprocessing.cpp
    Lib/Sys/Multiprocessing.hpp
    Lib/System.cpp
    Lib/System.hpp
    Lib/Timer.cpp
    Lib/Timer.hpp
    Lib/TriangularArray.hpp
    Lib/Vector.hpp
    Lib/VirtualIterator.hpp
    Minisat/core/Dimacs.h
    Minisat/core/Solver.cc
    Minisat/core/Solver.h
    Minisat/core/SolverTypes.h
    Minisat/mtl/Alg.h
    Minisat/mtl/Alloc.h
    Minisat/mtl/Heap.h
    Minisat/mtl/IntMap.h
    Minisat/mtl/IntTypes.h
    Minisat/mtl/Map.h
    Minisat/mtl/Queue.h
    Minisat/mtl/Rnd.h
    Minisat/mtl/Sort.h
    Minisat/mtl/Vec.h
    Minisat/mtl/XAlloc.h
    Minisat/simp/SimpSolver.cc
    Minisat/simp/SimpSolver.h
    Minisat/utils/Options.cc
    Minisat/utils/Options.h
    Minisat/utils/ParseUtils.h
    Minisat/utils/System.cc
    Minisat/utils/System.h
    Parse/SMTLIB2.cpp
    Parse/SMTLIB2.hpp
    Parse/TPTP.cpp
    Parse/TPTP.hpp
    SAT/BufferedSolver.cpp
    SAT/BufferedSolver.hpp
    SAT/CadicalInterfacing.cpp
    SAT/CadicalInterfacing.hpp
    SAT/FallbackSolverWrapper.cpp
    SAT/FallbackSolverWrapper.hpp
    SAT/MinimizingSolver.cpp
    SAT/MinimizingSolver.hpp
    SAT/MinisatInterfacing.cpp
    SAT/MinisatInterfacingNewSimp.cpp
    SAT/SAT2FO.cpp
    SAT/SAT2FO.hpp
    SAT/SATClause.cpp
    SAT/SATClause.hpp
    SAT/SATInference.cpp
    SAT/SATInference.hpp
    SAT/SATLiteral.cpp
    SAT/SATLiteral.hpp
    SAT/SATSolver.hpp
    SAT/Z3Interfacing.cpp
    SAT/Z3Interfacing.hpp
    SAT/Z3MainLoop.cpp
    SAT/Z3MainLoop.hpp
    SATSubsumption/SATSubsumptionAndResolution.cpp
    SATSubsumption/SATSubsumptionAndResolution.hpp
    SATSubsumption/subsat/SubstitutionTheory.hpp
    SATSubsumption/subsat/constraint.cpp
    SATSubsumption/subsat/constraint.hpp
    SATSubsumption/subsat/decision_queue.hpp
    SATSubsumption/subsat/default_init_allocator.hpp
    SATSubsumption/subsat/log.cpp
    SATSubsumption/subsat/log.hpp
    SATSubsumption/subsat/subsat.cpp
    SATSubsumption/subsat/subsat.hpp
    SATSubsumption/subsat/subsat_config.hpp
    SATSubsumption/subsat/types.cpp
    SATSubsumption/subsat/types.hpp
    SATSubsumption/subsat/variable_domain_size.hpp
    SATSubsumption/subsat/vector_map.hpp
    Saturation/AWPassiveClauseContainers.cpp
    Saturation/AWPassiveClauseContainers.hpp
    Saturation/AbstractPassiveClauseContainers.hpp
    Saturation/ClauseContainer.cpp
    Saturation/ClauseContainer.hpp
    Saturation/ConsequenceFinder.cpp
    Saturation/ConsequenceFinder.hpp
    Saturation/Discount.cpp
    Saturation/Discount.hpp
    Saturation/ExtensionalityClauseContainer.cpp
    Saturation/ExtensionalityClauseContainer.hpp
    Saturation/LRS.cpp
    Saturation/LRS.hpp
    Saturation/LabelFinder.cpp
    Saturation/LabelFinder.hpp
    Saturation/ManCSPassiveClauseContainer.cpp
    Saturation/Otter.cpp
    Saturation/Otter.hpp
    Saturation/PredicateSplitPassiveClauseContainers.cpp
    Saturation/PredicateSplitPassiveClauseContainers.hpp
    Saturation/ProvingHelper.cpp
    Saturation/ProvingHelper.hpp
    Saturation/SaturationAlgorithm.cpp
    Saturation/SaturationAlgorithm.hpp
    Saturation/Splitter.cpp
    Saturation/Splitter.hpp
    Saturation/SymElOutput.cpp
    Saturation/SymElOutput.hpp
    Shell/AnswerLiteralManager.cpp
    Shell/AnswerLiteralManager.hpp
    Shell/BlockedClauseElimination.cpp
    Shell/BlockedClauseElimination.hpp
    Shell/CNF.cpp
    Shell/CNF.hpp
    Shell/CommandLine.cpp
    Shell/CommandLine.hpp
    Shell/DistinctGroupExpansion.cpp
    Shell/DistinctGroupExpansion.hpp
    Shell/DistinctProcessor.cpp
    Shell/DistinctProcessor.hpp
    Shell/EqResWithDeletion.cpp
    Shell/EqResWithDeletion.hpp
    Shell/EqualityProxy.cpp
    Shell/EqualityProxy.hpp
    Shell/EqualityProxyMono.cpp
    Shell/EqualityProxyMono.hpp
    Shell/FOOLElimination.cpp
    Shell/FOOLElimination.hpp
    Shell/Flattening.cpp
    Shell/Flattening.hpp
    Shell/FunctionDefinition.cpp
    Shell/FunctionDefinition.hpp
    Shell/FunctionDefinitionHandler.cpp
    Shell/FunctionDefinitionHandler.hpp
    Shell/GeneralSplitting.cpp
    Shell/GeneralSplitting.hpp
    Shell/GoalGuessing.cpp
    Shell/InequalitySplitting.cpp
    Shell/InequalitySplitting.hpp
    Shell/InterpolantMinimizer.cpp
    Shell/InterpolantMinimizer.hpp
    Shell/Interpolants.cpp
    Shell/Interpolants.hpp
    Shell/InterpretedNormalizer.cpp
    Shell/InterpretedNormalizer.hpp
    Shell/LaTeX.cpp
    Shell/LaTeX.hpp
    Shell/LambdaElimination.cpp
    Shell/LambdaElimination.hpp
    Shell/Lexer.cpp
    Shell/Lexer.cpp
    Shell/Lexer.hpp
    Shell/Lexer.hpp
    Shell/LispLexer.cpp
    Shell/LispLexer.hpp
    Shell/LispParser.cpp
    Shell/LispParser.hpp
    Shell/NNF.cpp
    Shell/NNF.hpp
    Shell/Naming.cpp
    Shell/Naming.hpp
    Shell/NewCNF.cpp
    Shell/NewCNF.hpp
    Shell/Normalisation.cpp
    Shell/Normalisation.hpp
    Shell/Options.cpp
    Shell/Options.hpp
    Shell/PartialRedundancyHandler.cpp
    Shell/PartialRedundancyHandler.hpp
    Shell/PredicateDefinition.cpp
    Shell/PredicateDefinition.hpp
    Shell/Preprocess.cpp
    Shell/Preprocess.cpp
    Shell/Preprocess.hpp
    Shell/Preprocess.hpp
    Shell/Property.cpp
    Shell/Property.hpp
    Shell/Rectify.cpp
    Shell/Rectify.hpp
    Shell/SMTLIBLogic.hpp
    Shell/Shuffling.cpp
    Shell/Shuffling.hpp
    Shell/SimplifyFalseTrue.cpp
    Shell/SimplifyFalseTrue.hpp
    Shell/SineUtils.cpp
    Shell/SineUtils.hpp
    Shell/Skolem.cpp
    Shell/Skolem.hpp
    Shell/Statistics.cpp
    Shell/Statistics.hpp
    Shell/SubexpressionIterator.cpp
    Shell/SubexpressionIterator.hpp
    Shell/SymCounter.cpp
    Shell/SymCounter.hpp
    Shell/SymbolDefinitionInlining.cpp
    Shell/SymbolDefinitionInlining.hpp
    Shell/SymbolOccurrenceReplacement.cpp
    Shell/SymbolOccurrenceReplacement.hpp
    Shell/TPTPPrinter.cpp
    Shell/TPTPPrinter.hpp
    Shell/TermAlgebra.cpp
    Shell/TermAlgebra.hpp
    Shell/TheoryAxioms.cpp
    Shell/TheoryAxioms.hpp
    Shell/TheoryFinder.cpp
    Shell/TheoryFinder.hpp
    Shell/TheoryFlattening.cpp
    Shell/TheoryFlattening.hpp
    Shell/Token.cpp
    Shell/Token.hpp
    Shell/TweeGoalTransformation.cpp
    Shell/TweeGoalTransformation.hpp
    Shell/UIHelper.cpp
    Shell/UIHelper.hpp
    cadical/contrib/craigtracer.cpp
    cadical/contrib/craigtracer.hpp
    cadical/src/analyze.cpp
    cadical/src/arena.cpp
    cadical/src/arena.hpp
    cadical/src/assume.cpp
    cadical/src/averages.cpp
    cadical/src/averages.hpp
    cadical/src/backtrack.cpp
    cadical/src/backward.cpp
    cadical/src/bins.cpp
    cadical/src/bins.hpp
    cadical/src/block.cpp
    cadical/src/block.hpp
    cadical/src/cadical.hpp
    cadical/src/checker.cpp
    cadical/src/checker.hpp
    cadical/src/clause.cpp
    cadical/src/clause.hpp
    cadical/src/collect.cpp
    cadical/src/compact.cpp
    cadical/src/condition.cpp
    cadical/src/config.cpp
    cadical/src/config.hpp
    cadical/src/constrain.cpp
    cadical/src/contract.cpp
    cadical/src/contract.hpp
    cadical/src/cover.cpp
    cadical/src/cover.hpp
    cadical/src/decide.cpp
    cadical/src/decompose.cpp
    cadical/src/decompose.hpp
    cadical/src/deduplicate.cpp
    cadical/src/drattracer.cpp
    cadical/src/drattracer.hpp
    cadical/src/elim.cpp
    cadical/src/elim.hpp
    cadical/src/ema.cpp
    cadical/src/ema.hpp
    cadical/src/extend.cpp
    cadical/src/external.cpp
    cadical/src/external.hpp
    cadical/src/external_propagate.cpp
    cadical/src/file.cpp
    cadical/src/file.hpp
    cadical/src/flags.cpp
    cadical/src/flags.hpp
    cadical/src/flip.cpp
    cadical/src/format.cpp
    cadical/src/format.hpp
    cadical/src/frattracer.cpp
    cadical/src/frattracer.hpp
    cadical/src/gates.cpp
    cadical/src/heap.hpp
    cadical/src/idruptracer.cpp
    cadical/src/idruptracer.hpp
    cadical/src/instantiate.cpp
    cadical/src/instantiate.hpp
    cadical/src/internal.cpp
    cadical/src/internal.hpp
    cadical/src/inttypes.hpp
    cadical/src/level.hpp
    cadical/src/lidruptracer.cpp
    cadical/src/lidruptracer.hpp
    cadical/src/limit.cpp
    cadical/src/limit.hpp
    cadical/src/logging.cpp
    cadical/src/logging.hpp
    cadical/src/lookahead.cpp
    cadical/src/lratbuilder.cpp
    cadical/src/lratbuilder.hpp
    cadical/src/lratchecker.cpp
    cadical/src/lratchecker.hpp
    cadical/src/lrattracer.cpp
    cadical/src/lrattracer.hpp
    cadical/src/lucky.cpp
    cadical/src/message.cpp
    cadical/src/message.hpp
    cadical/src/minimize.cpp
    cadical/src/occs.cpp
    cadical/src/occs.hpp
    cadical/src/options.cpp
    cadical/src/options.hpp
    cadical/src/parse.cpp
    cadical/src/parse.hpp
    cadical/src/phases.cpp
    cadical/src/phases.hpp
    cadical/src/probe.cpp
    cadical/src/profile.cpp
    cadical/src/profile.hpp
    cadical/src/proof.cpp
    cadical/src/proof.hpp
    cadical/src/propagate.cpp
    cadical/src/queue.cpp
    cadical/src/queue.hpp
    cadical/src/radix.hpp
    cadical/src/random.cpp
    cadical/src/random.hpp
    cadical/src/range.hpp
    cadical/src/reap.cpp
    cadical/src/reap.hpp
    cadical/src/reduce.cpp
    cadical/src/reluctant.hpp
    cadical/src/rephase.cpp
    cadical/src/report.cpp
    cadical/src/resources.cpp
    cadical/src/resources.hpp
    cadical/src/restart.cpp
    cadical/src/restore.cpp
    cadical/src/score.cpp
    cadical/src/score.hpp
    cadical/src/shrink.cpp
    cadical/src/signal.cpp
    cadical/src/signal.hpp
    cadical/src/solution.cpp
    cadical/src/solver.cpp
    cadical/src/stats.cpp
    cadical/src/stats.hpp
    cadical/src/subsume.cpp
    cadical/src/terminal.cpp
    cadical/src/terminal.hpp
    cadical/src/ternary.cpp
    cadical/src/testing.hpp
    cadical/src/tracer.hpp
    cadical/src/transred.cpp
    cadical/src/util.cpp
    cadical/src/util.hpp
    cadical/src/var.cpp
    cadical/src/var.hpp
    cadical/src/veripbtracer.cpp
    cadical/src/veripbtracer.hpp
    cadical/src/version.cpp
    cadical/src/version.hpp
    cadical/src/vivify.cpp
    cadical/src/vivify.hpp
    cadical/src/walk.cpp
    cadical/src/watch.cpp
    cadical/src/watch.hpp

    Forwards.hpp
    "${CMAKE_CURRENT_BINARY_DIR}/version.cpp"
)
