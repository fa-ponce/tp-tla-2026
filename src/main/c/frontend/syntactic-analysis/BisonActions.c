#include "BisonActions.h"

/* MODULE INTERNAL STATE */

static CompilerState * _compilerState = NULL;
static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownBisonActionsModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: BisonActions...");
		destroyLogger(_logger);
		_logger = NULL;
	}
	_compilerState = NULL;
}

ModuleDestructor initializeBisonActionsModule(CompilerState * compilerState) {
	_compilerState = compilerState;
	_logger = createLogger("BisonActions");
	return _shutdownBisonActionsModule;
}

/* IMPORTED FUNCTIONS */

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char * functionName);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}

/* PUBLIC FUNCTIONS */

/** Program structure. */

Program * ScriptProgramSemanticAction(DeclarationList * declarations, ScriptDeclaration * script) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->declarations = declarations;
	program->script = script;
	program->type = SCRIPT_PROGRAM;
	_compilerState->abstractSyntaxtTree = program;
	return program;
}

Program * ForProgramSemanticAction(DeclarationList * declarations, ForStatement * forStatement) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->declarations = declarations;
	program->forStatement = forStatement;
	program->type = FOR_PROGRAM;
	_compilerState->abstractSyntaxtTree = program;
	return program;
}

Declaration * LetDeclarationSemanticAction(LetDeclaration * letDeclaration) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Declaration * declaration = calloc(1, sizeof(Declaration));
	declaration->letDeclaration = letDeclaration;
	declaration->type = LET_DECLARATION;
	return declaration;
}

Declaration * FunctionDeclarationSemanticAction(FunctionDeclaration * functionDeclaration) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Declaration * declaration = calloc(1, sizeof(Declaration));
	declaration->functionDeclaration = functionDeclaration;
	declaration->type = FUNCTION_DECLARATION;
	return declaration;
}

Declaration * PipelineDeclarationSemanticAction(PipelineDeclaration * pipelineDeclaration) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Declaration * declaration = calloc(1, sizeof(Declaration));
	declaration->pipelineDeclaration = pipelineDeclaration;
	declaration->type = PIPELINE_DECLARATION;
	return declaration;
}

LetDeclaration * LetSemanticAction(char * name, Expression * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	LetDeclaration * letDeclaration = calloc(1, sizeof(LetDeclaration));
	letDeclaration->name = name;
	letDeclaration->value = value;
	return letDeclaration;
}

FunctionDeclaration * FunctionSemanticAction(char * name, IdentifierList * parameters, Expression * returnExpression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	FunctionDeclaration * functionDeclaration = calloc(1, sizeof(FunctionDeclaration));
	functionDeclaration->name = name;
	functionDeclaration->parameters = parameters;
	functionDeclaration->returnExpression = returnExpression;
	return functionDeclaration;
}

PipelineDeclaration * PipelineSemanticAction(char * name, IdentifierList * parameters, Condition * requirement, StageList * stages) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	PipelineDeclaration * pipelineDeclaration = calloc(1, sizeof(PipelineDeclaration));
	pipelineDeclaration->name = name;
	pipelineDeclaration->parameters = parameters;
	pipelineDeclaration->requirement = requirement;
	pipelineDeclaration->stages = stages;
	return pipelineDeclaration;
}

/** Lists. */

DeclarationList * DeclarationListSemanticAction(Declaration * declaration, DeclarationList * next) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	DeclarationList * declarationList = calloc(1, sizeof(DeclarationList));
	declarationList->declaration = declaration;
	declarationList->next = next;
	return declarationList;
}

IdentifierList * IdentifierListSemanticAction(char * identifier, IdentifierList * next) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	IdentifierList * identifierList = calloc(1, sizeof(IdentifierList));
	identifierList->identifier = identifier;
	identifierList->next = next;
	return identifierList;
}

IdentifierList * SingleIdentifierListSemanticAction(char * identifier) {
	return IdentifierListSemanticAction(identifier, NULL);
}

ExpressionList * ExpressionListSemanticAction(Expression * expression, ExpressionList * next) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ExpressionList * expressionList = calloc(1, sizeof(ExpressionList));
	expressionList->expression = expression;
	expressionList->next = next;
	return expressionList;
}

ExpressionList * SingleExpressionListSemanticAction(Expression * expression) {
	return ExpressionListSemanticAction(expression, NULL);
}

StageList * StageListSemanticAction(Stage * stage, StageList * next) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StageList * stageList = calloc(1, sizeof(StageList));
	stageList->stage = stage;
	stageList->next = next;
	return stageList;
}

StageList * SingleStageListSemanticAction(Stage * stage) {
	return StageListSemanticAction(stage, NULL);
}

/** Stages. */

Stage * FilterStageSemanticAction(FilterStage * filterStage) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Stage * stage = calloc(1, sizeof(Stage));
	stage->filterStage = filterStage;
	stage->type = FILTER_STAGE;
	return stage;
}

Stage * MutateStageSemanticAction(MutateStage * mutateStage) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Stage * stage = calloc(1, sizeof(Stage));
	stage->mutateStage = mutateStage;
	stage->type = MUTATE_STAGE;
	return stage;
}

Stage * SelectStageSemanticAction(SelectStage * selectStage) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Stage * stage = calloc(1, sizeof(Stage));
	stage->selectStage = selectStage;
	stage->type = SELECT_STAGE;
	return stage;
}

Stage * SortStageSemanticAction(SortStage * sortStage) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Stage * stage = calloc(1, sizeof(Stage));
	stage->sortStage = sortStage;
	stage->type = SORT_STAGE;
	return stage;
}

Stage * LimitStageSemanticAction(LimitStage * limitStage) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Stage * stage = calloc(1, sizeof(Stage));
	stage->limitStage = limitStage;
	stage->type = LIMIT_STAGE;
	return stage;
}

Stage * ApplyStageSemanticAction(ApplyStage * applyStage) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Stage * stage = calloc(1, sizeof(Stage));
	stage->applyStage = applyStage;
	stage->type = APPLY_STAGE;
	return stage;
}

FilterStage * FilterSemanticAction(Condition * condition) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	FilterStage * filterStage = calloc(1, sizeof(FilterStage));
	filterStage->condition = condition;
	return filterStage;
}

MutateStage * MutateSemanticAction(char * column, MutateExpression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	MutateStage * mutateStage = calloc(1, sizeof(MutateStage));
	mutateStage->column = column;
	mutateStage->expression = expression;
	return mutateStage;
}

MutateExpression * ConditionalMutateExpressionSemanticAction(Condition * condition, Expression * thenExpression, Expression * elseExpression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	MutateExpression * mutateExpression = calloc(1, sizeof(MutateExpression));
	mutateExpression->condition = condition;
	mutateExpression->thenExpression = thenExpression;
	mutateExpression->elseExpression = elseExpression;
	mutateExpression->type = CONDITIONAL_MUTATE_EXPRESSION;
	return mutateExpression;
}

MutateExpression * SimpleMutateExpressionSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	MutateExpression * mutateExpression = calloc(1, sizeof(MutateExpression));
	mutateExpression->expression = expression;
	mutateExpression->type = SIMPLE_MUTATE_EXPRESSION;
	return mutateExpression;
}

SelectStage * SelectSemanticAction(IdentifierList * columns) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SelectStage * selectStage = calloc(1, sizeof(SelectStage));
	selectStage->columns = columns;
	return selectStage;
}

SortStage * SortSemanticAction(char * column, TokenLabel direction) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	SortStage * sortStage = calloc(1, sizeof(SortStage));
	sortStage->column = column;
	sortStage->direction = direction;
	return sortStage;
}

LimitStage * LimitSemanticAction(Expression * count) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	LimitStage * limitStage = calloc(1, sizeof(LimitStage));
	limitStage->count = count;
	return limitStage;
}

ApplyStage * ApplySemanticAction(char * pipelineName, ExpressionList * arguments) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ApplyStage * applyStage = calloc(1, sizeof(ApplyStage));
	applyStage->pipelineName = pipelineName;
	applyStage->arguments = arguments;
	return applyStage;
}

/** Script body and branching blocks. */

ScriptDeclaration * ScriptSemanticAction(Expression * loadPath, ScriptBody * body) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ScriptDeclaration * scriptDeclaration = calloc(1, sizeof(ScriptDeclaration));
	scriptDeclaration->loadPath = loadPath;
	scriptDeclaration->body = body;
	return scriptDeclaration;
}

ScriptBody * StageScriptBodySemanticAction(Stage * stage, ScriptBody * next) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ScriptBody * scriptBody = calloc(1, sizeof(ScriptBody));
	scriptBody->stage = stage;
	scriptBody->type = STAGE_SCRIPT_BODY;
	scriptBody->next = next;
	return scriptBody;
}

ScriptBody * BlockScriptBodySemanticAction(Block * block, ScriptBody * next) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ScriptBody * scriptBody = calloc(1, sizeof(ScriptBody));
	scriptBody->block = block;
	scriptBody->type = BLOCK_SCRIPT_BODY;
	scriptBody->next = next;
	return scriptBody;
}

ScriptBody * SaveScriptBodySemanticAction(Expression * savePath) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ScriptBody * scriptBody = calloc(1, sizeof(ScriptBody));
	scriptBody->savePath = savePath;
	scriptBody->type = SAVE_SCRIPT_BODY;
	scriptBody->next = NULL;
	return scriptBody;
}

/**
 * The grammar is right-recursive, so the stage that comes first in the source
 * is the last one to be reduced: it's prepended to the block's stages.
 */
Block * StageBlockSemanticAction(Stage * stage, Block * block) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	block->stages = StageListSemanticAction(stage, block->stages);
	return block;
}

Block * SaveBlockSemanticAction(Expression * savePath) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Block * block = calloc(1, sizeof(Block));
	block->stages = NULL;
	block->savePath = savePath;
	return block;
}

/** Iteration over variants. */

ForStatement * ForSemanticAction(IdentifierList * identifiers, TupleList * values, ScriptDeclaration * body) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ForStatement * forStatement = calloc(1, sizeof(ForStatement));
	forStatement->identifiers = identifiers;
	forStatement->values = values;
	forStatement->body = body;
	return forStatement;
}

/**
 * Turns [v1, v2, ...] into [[v1], [v2], ...], reusing each ExpressionList
 * node as a one-element tuple (only its "next" link is cut), so no expression
 * changes owner twice.
 */
TupleList * SingletonTupleListSemanticAction(ExpressionList * values) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	TupleList * head = NULL;
	TupleList * tail = NULL;
	while (values != NULL) {
		ExpressionList * next = values->next;
		values->next = NULL;
		TupleList * node = SingleTupleListSemanticAction(values);
		if (tail == NULL) {
			head = node;
		}
		else {
			tail->next = node;
		}
		tail = node;
		values = next;
	}
	return head;
}

TupleList * TupleListSemanticAction(ExpressionList * tuple, TupleList * next) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	TupleList * tupleList = calloc(1, sizeof(TupleList));
	tupleList->tuple = tuple;
	tupleList->next = next;
	return tupleList;
}

TupleList * SingleTupleListSemanticAction(ExpressionList * tuple) {
	return TupleListSemanticAction(tuple, NULL);
}

/** Boolean conditions. */

Condition * LogicalConditionSemanticAction(Condition * leftCondition, Condition * rightCondition, ConditionType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Condition * condition = calloc(1, sizeof(Condition));
	condition->leftCondition = leftCondition;
	condition->rightCondition = rightCondition;
	condition->type = type;
	return condition;
}

Condition * NegationConditionSemanticAction(Condition * negatedCondition) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Condition * condition = calloc(1, sizeof(Condition));
	condition->negatedCondition = negatedCondition;
	condition->type = NEGATION;
	return condition;
}

Condition * ComparisonConditionSemanticAction(Expression * leftExpression, TokenLabel comparisonOperator, Expression * rightExpression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Condition * condition = calloc(1, sizeof(Condition));
	condition->leftExpression = leftExpression;
	condition->comparisonOperator = comparisonOperator;
	condition->rightExpression = rightExpression;
	condition->type = COMPARISON;
	return condition;
}

/** Arithmetic expressions. */

Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = type;
	return expression;
}

Expression * NumberExpressionSemanticAction(const double value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->number = value;
	expression->type = NUMBER_LITERAL;
	return expression;
}

Expression * StringExpressionSemanticAction(char * value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->string = value;
	expression->type = STRING_LITERAL;
	return expression;
}

Expression * BooleanExpressionSemanticAction(const bool value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->boolean = value;
	expression->type = BOOLEAN_LITERAL;
	return expression;
}

Expression * IdentifierExpressionSemanticAction(char * identifier) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->identifier = identifier;
	expression->type = IDENTIFIER;
	return expression;
}

Expression * ExternalParameterExpressionSemanticAction(char * name) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->externalParameter = name;
	expression->type = EXTERNAL_PARAMETER;
	return expression;
}

Expression * FunctionCallExpressionSemanticAction(char * functionName, ExpressionList * arguments) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->functionName = functionName;
	expression->arguments = arguments;
	expression->type = FUNCTION_CALL;
	return expression;
}

Expression * ColumnExpressionSemanticAction(char* column){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression* expression = calloc(1, sizeof(Expression));
	expression->column = column;
	expression->type = COLUMN_REFERENCE;
	return expression;
}