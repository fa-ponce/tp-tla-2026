#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

/** Shutdown module's internal state. */
void _shutdownAbstractSyntaxTreeModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: AbstractSyntaxTree...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeAbstractSyntaxTreeModule() {
	_logger = createLogger("AbstractSyntaxTree");
	return _shutdownAbstractSyntaxTreeModule;
}

/* PUBLIC FUNCTIONS */

void destroyProgram(Program * program) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program != NULL) {
		destroyDeclarationList(program->declarations);
		switch (program->type) {
			case SCRIPT_PROGRAM:
				destroyScriptDeclaration(program->script);
				break;
			case FOR_PROGRAM:
				destroyForStatement(program->forStatement);
				break;
		}
		free(program);
	}
}

void destroyDeclarationList(DeclarationList * declarationList) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	while (declarationList != NULL) {
		DeclarationList * next = declarationList->next;
		destroyDeclaration(declarationList->declaration);
		free(declarationList);
		declarationList = next;
	}
}

void destroyDeclaration(Declaration * declaration) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (declaration != NULL) {
		switch (declaration->type) {
			case LET_DECLARATION:
				destroyLetDeclaration(declaration->letDeclaration);
				break;
			case FUNCTION_DECLARATION:
				destroyFunctionDeclaration(declaration->functionDeclaration);
				break;
			case PIPELINE_DECLARATION:
				destroyPipelineDeclaration(declaration->pipelineDeclaration);
				break;
		}
		free(declaration);
	}
}

void destroyLetDeclaration(LetDeclaration * letDeclaration) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (letDeclaration != NULL) {
		free(letDeclaration->name);
		destroyExpression(letDeclaration->value);
		free(letDeclaration);
	}
}

void destroyFunctionDeclaration(FunctionDeclaration * functionDeclaration) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (functionDeclaration != NULL) {
		free(functionDeclaration->name);
		destroyIdentifierList(functionDeclaration->parameters);
		destroyExpression(functionDeclaration->returnExpression);
		free(functionDeclaration);
	}
}

void destroyPipelineDeclaration(PipelineDeclaration * pipelineDeclaration) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (pipelineDeclaration != NULL) {
		free(pipelineDeclaration->name);
		destroyIdentifierList(pipelineDeclaration->parameters);
		destroyCondition(pipelineDeclaration->requirement);
		destroyStageList(pipelineDeclaration->stages);
		free(pipelineDeclaration);
	}
}

void destroyIdentifierList(IdentifierList * identifierList) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	while (identifierList != NULL) {
		IdentifierList * next = identifierList->next;
		free(identifierList->identifier);
		free(identifierList);
		identifierList = next;
	}
}

void destroyExpressionList(ExpressionList * expressionList) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	while (expressionList != NULL) {
		ExpressionList * next = expressionList->next;
		destroyExpression(expressionList->expression);
		free(expressionList);
		expressionList = next;
	}
}

void destroyStageList(StageList * stageList) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	while (stageList != NULL) {
		StageList * next = stageList->next;
		destroyStage(stageList->stage);
		free(stageList);
		stageList = next;
	}
}

void destroyStage(Stage * stage) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (stage != NULL) {
		switch (stage->type) {
			case FILTER_STAGE:
				destroyFilterStage(stage->filterStage);
				break;
			case MUTATE_STAGE:
				destroyMutateStage(stage->mutateStage);
				break;
			case SELECT_STAGE:
				destroySelectStage(stage->selectStage);
				break;
			case SORT_STAGE:
				destroySortStage(stage->sortStage);
				break;
			case LIMIT_STAGE:
				destroyLimitStage(stage->limitStage);
				break;
			case APPLY_STAGE:
				destroyApplyStage(stage->applyStage);
				break;
		}
		free(stage);
	}
}

void destroyFilterStage(FilterStage * filterStage) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (filterStage != NULL) {
		destroyCondition(filterStage->condition);
		free(filterStage);
	}
}

void destroyMutateStage(MutateStage * mutateStage) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (mutateStage != NULL) {
		free(mutateStage->column);
		destroyMutateExpression(mutateStage->expression);
		free(mutateStage);
	}
}

void destroyMutateExpression(MutateExpression * mutateExpression) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (mutateExpression != NULL) {
		switch (mutateExpression->type) {
			case CONDITIONAL_MUTATE_EXPRESSION:
				destroyCondition(mutateExpression->condition);
				destroyExpression(mutateExpression->thenExpression);
				destroyExpression(mutateExpression->elseExpression);
				break;
			case SIMPLE_MUTATE_EXPRESSION:
				destroyExpression(mutateExpression->expression);
				break;
		}
		free(mutateExpression);
	}
}

void destroySelectStage(SelectStage * selectStage) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (selectStage != NULL) {
		destroyIdentifierList(selectStage->columns);
		free(selectStage);
	}
}

void destroySortStage(SortStage * sortStage) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (sortStage != NULL) {
		free(sortStage->column);
		free(sortStage);
	}
}

void destroyLimitStage(LimitStage * limitStage) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (limitStage != NULL) {
		destroyExpression(limitStage->count);
		free(limitStage);
	}
}

void destroyApplyStage(ApplyStage * applyStage) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (applyStage != NULL) {
		free(applyStage->pipelineName);
		destroyExpressionList(applyStage->arguments);
		free(applyStage);
	}
}

void destroyScriptDeclaration(ScriptDeclaration * scriptDeclaration) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (scriptDeclaration != NULL) {
		destroyExpression(scriptDeclaration->loadPath);
		destroyScriptBody(scriptDeclaration->body);
		free(scriptDeclaration);
	}
}

void destroyScriptBody(ScriptBody * scriptBody) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	while (scriptBody != NULL) {
		ScriptBody * next = scriptBody->next;
		switch (scriptBody->type) {
			case STAGE_SCRIPT_BODY:
				destroyStage(scriptBody->stage);
				break;
			case BLOCK_SCRIPT_BODY:
				destroyBlock(scriptBody->block);
				break;
			case SAVE_SCRIPT_BODY:
				destroyExpression(scriptBody->savePath);
				break;
		}
		free(scriptBody);
		scriptBody = next;
	}
}

void destroyBlock(Block * block) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (block != NULL) {
		destroyStageList(block->stages);
		destroyExpression(block->savePath);
		free(block);
	}
}

void destroyForStatement(ForStatement * forStatement) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (forStatement != NULL) {
		destroyIdentifierList(forStatement->identifiers);
		destroyTupleList(forStatement->values);
		destroyScriptDeclaration(forStatement->body);
		free(forStatement);
	}
}

void destroyTupleList(TupleList * tupleList) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	while (tupleList != NULL) {
		TupleList * next = tupleList->next;
		destroyExpressionList(tupleList->tuple);
		free(tupleList);
		tupleList = next;
	}
}

void destroyCondition(Condition * condition) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (condition != NULL) {
		switch (condition->type) {
			case DISJUNCTION:
			case CONJUNCTION:
				destroyCondition(condition->leftCondition);
				destroyCondition(condition->rightCondition);
				break;
			case NEGATION:
				destroyCondition(condition->negatedCondition);
				break;
			case COMPARISON:
				destroyExpression(condition->leftExpression);
				destroyExpression(condition->rightExpression);
				break;
		}
		free(condition);
	}
}

void destroyExpression(Expression * expression) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (expression != NULL) {
		switch (expression->type) {
			case ADDITION:
			case DIVISION:
			case MULTIPLICATION:
			case SUBTRACTION:
				destroyExpression(expression->leftExpression);
				destroyExpression(expression->rightExpression);
				break;
			case NUMBER_LITERAL:
			case BOOLEAN_LITERAL:
				break;
			case STRING_LITERAL:
				free(expression->string);
				break;
			case IDENTIFIER:
				free(expression->identifier);
				break;
			case EXTERNAL_PARAMETER:
				free(expression->externalParameter);
				break;
			case FUNCTION_CALL:
				free(expression->functionName);
				destroyExpressionList(expression->arguments);
				break;
		}
		free(expression);
	}
}
