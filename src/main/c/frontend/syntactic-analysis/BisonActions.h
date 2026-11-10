#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/CompilerState.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonParser.h"
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState);

/**
 * Bison semantic actions.
 */

/** Program structure. */

Program * ScriptProgramSemanticAction(DeclarationList * declarations, ScriptDeclaration * script);
Program * ForProgramSemanticAction(DeclarationList * declarations, ForStatement * forStatement);
Declaration * LetDeclarationSemanticAction(LetDeclaration * letDeclaration);
Declaration * FunctionDeclarationSemanticAction(FunctionDeclaration * functionDeclaration);
Declaration * PipelineDeclarationSemanticAction(PipelineDeclaration * pipelineDeclaration);
LetDeclaration * LetSemanticAction(char * name, Expression * value);
FunctionDeclaration * FunctionSemanticAction(char * name, IdentifierList * parameters, Expression * returnExpression);
PipelineDeclaration * PipelineSemanticAction(char * name, IdentifierList * parameters, Condition * requirement, StageList * stages);

/** Lists. */

DeclarationList * DeclarationListSemanticAction(Declaration * declaration, DeclarationList * next);
IdentifierList * IdentifierListSemanticAction(char * identifier, IdentifierList * next);
IdentifierList * SingleIdentifierListSemanticAction(char * identifier);
ExpressionList * ExpressionListSemanticAction(Expression * expression, ExpressionList * next);
ExpressionList * SingleExpressionListSemanticAction(Expression * expression);
StageList * StageListSemanticAction(Stage * stage, StageList * next);
StageList * SingleStageListSemanticAction(Stage * stage);

/** Stages. */

Stage * FilterStageSemanticAction(FilterStage * filterStage);
Stage * MutateStageSemanticAction(MutateStage * mutateStage);
Stage * SelectStageSemanticAction(SelectStage * selectStage);
Stage * SortStageSemanticAction(SortStage * sortStage);
Stage * LimitStageSemanticAction(LimitStage * limitStage);
Stage * ApplyStageSemanticAction(ApplyStage * applyStage);
FilterStage * FilterSemanticAction(Condition * condition);
MutateStage * MutateSemanticAction(char * column, MutateExpression * expression);
MutateExpression * ConditionalMutateExpressionSemanticAction(Condition * condition, Expression * thenExpression, Expression * elseExpression);
MutateExpression * SimpleMutateExpressionSemanticAction(Expression * expression);
SelectStage * SelectSemanticAction(IdentifierList * columns);
SortStage * SortSemanticAction(char * column, TokenLabel direction);
LimitStage * LimitSemanticAction(Expression * count);
ApplyStage * ApplySemanticAction(char * pipelineName, ExpressionList * arguments);

/** Script body and branching blocks. */

ScriptDeclaration * ScriptSemanticAction(Expression * loadPath, ScriptBody * body);
ScriptBody * StageScriptBodySemanticAction(Stage * stage, ScriptBody * next);
ScriptBody * BlockScriptBodySemanticAction(Block * block, ScriptBody * next);
ScriptBody * SaveScriptBodySemanticAction(Expression * savePath);
Block * StageBlockSemanticAction(Stage * stage, Block * block);
Block * SaveBlockSemanticAction(Expression * savePath);

/** Iteration over variants. */

ForStatement * ForSemanticAction(IdentifierList * identifiers, TupleList * values, ScriptDeclaration * body);
TupleList * SingletonTupleListSemanticAction(ExpressionList * values);
TupleList * TupleListSemanticAction(ExpressionList * tuple, TupleList * next);
TupleList * SingleTupleListSemanticAction(ExpressionList * tuple);

/** Boolean conditions. */

Condition * LogicalConditionSemanticAction(Condition * leftCondition, Condition * rightCondition, ConditionType type);
Condition * NegationConditionSemanticAction(Condition * condition);
Condition * ComparisonConditionSemanticAction(Expression * leftExpression, TokenLabel comparisonOperator, Expression * rightExpression);

/** Arithmetic expressions. */

Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type);
Expression * NumberExpressionSemanticAction(const double value);
Expression * StringExpressionSemanticAction(char * value);
Expression * BooleanExpressionSemanticAction(const bool value);
Expression * IdentifierExpressionSemanticAction(char * identifier);
Expression * ExternalParameterExpressionSemanticAction(char * name);
Expression * FunctionCallExpressionSemanticAction(char * functionName, ExpressionList * arguments);
Expression * ColumnExpressionSemanticAction(char* column);

#endif
