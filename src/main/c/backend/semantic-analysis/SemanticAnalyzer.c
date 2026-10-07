#include "SemanticAnalyzer.h"
#include "SymbolTable.h"

static void _visitDeclarationList(DeclarationList* declarationList);
static void _visitDeclaration(Declaration* declaration);
static void _visitLetDeclaration(LetDeclaration* letDeclaration);
static void _visitFunctionDeclaration(FunctionDeclaration* functionDeclaration);
static void _visitPipelineDeclaration(PipelineDeclaration* pipelineDeclaration);
static void _visitCondition(Condition* condition);
static void _visitStageList(StageList* stageList);
static void _visitStage(Stage* stage);
static void _declareIdentifierList(IdentifierList* identifierList, SymbolKind kind);
static void _declare(char* name, Symbol symbol);
static void _visitExpression(Expression* expression);
static void _visitFilterStage(FilterStage* filterStage);
static void _visitMutateStage(MutateStage* mutateStage);
static void _visitSelectStage(SelectStage* selectStage);
static void _visitLimitStage(LimitStage* limitStage);
static void _visitApplyStage(ApplyStage* applyStage);
static void _visitSortStage(SortStage* sortStage);
static void _visitProgram(Program* program);
static void _visitIdentifierList(IdentifierList* identifierList);
static void _visitExpressionList(ExpressionList* expressionList);
static void _visitTupleList(TupleList* tupleList);
static void _visitMutateExpression(MutateExpression* mutateExpression);
static void _visitScriptDeclaration(ScriptDeclaration* scriptDeclaration);
static void _visitScriptBody(ScriptBody* scriptBody);
static void _visitBlock(Block* block);
static void _visitForStatement(ForStatement* forStatement);



static Logger* _logger = NULL;
static bool _failed = false;   // para seguir recorriendo y reportar todos los errores, no solo el primero



static void _shutdownSemanticAnalyzerModule() {
    if (_logger != NULL) {
        destroyLogger(_logger);
        _logger = NULL;
    }
}

ModuleDestructor initializeSemanticAnalyzerModule() {
    _logger = createLogger("SemanticAnalyzer");
    return _shutdownSemanticAnalyzerModule;
}

static void _visitDeclarationList(DeclarationList* declarationList){
    for(DeclarationList* node = declarationList; node != NULL; node = node->next){
            _visitDeclaration(node->declaration);
        }
}

static void _visitDeclaration(Declaration* declaration){
    switch (declaration->type){
        case LET_DECLARATION:
            _visitLetDeclaration(declaration->letDeclaration);
            break;
        case FUNCTION_DECLARATION:
            _visitFunctionDeclaration(declaration->functionDeclaration);
            break;
        case PIPELINE_DECLARATION:
            _visitPipelineDeclaration(declaration->pipelineDeclaration);
            break;
        default:
            break;
    }
}

static void _visitLetDeclaration(LetDeclaration* letDeclaration){
    _visitExpression(letDeclaration->value);
    _declare(letDeclaration->name, (Symbol){ .kind = LET_SYMBOL, .let = letDeclaration });
}


static void _visitFunctionDeclaration(FunctionDeclaration* functionDeclaration){
    _declare(functionDeclaration->name, (Symbol){ .kind = FUNCTION_SYMBOL, .function = functionDeclaration});

    // Entramos a un nuevo scope para leer los parametros
    if(!enterScope()){
        logError(_logger, "No hay memoria para el scope de %s", functionDeclaration->name);
        _failed = true;
        return;
    }
    // Visito los parametros
    _declareIdentifierList(functionDeclaration->parameters, PARAMETER_SYMBOL);
    // Visito el cuerpo de la funcion
    _visitExpression(functionDeclaration->returnExpression);
    
    //Salgo del scope
    exitScope();
}

static void _visitPipelineDeclaration(PipelineDeclaration* pipelineDeclaration){
    _declare(pipelineDeclaration->name, (Symbol){ .kind = PIPELINE_SYMBOL, .pipeline = pipelineDeclaration});

    // Entramos a un nuevo scope para leer los parametros
    if(!enterScope()){
        logError(_logger, "No hay memoria para el scope de %s", pipelineDeclaration->name);
        _failed = true;
        return;
    }
    // Visito los parametros
    _declareIdentifierList(pipelineDeclaration->parameters, PARAMETER_SYMBOL);
    // visito los requerimientos
    _visitCondition(pipelineDeclaration->requirement);
    // visito las stages
    _visitStageList(pipelineDeclaration->stages);

    //Salog del scope
    exitScope();
}

static void _visitCondition(Condition* condition){
    if(condition == NULL){
          return;
    }

    switch (condition->type){
    case CONJUNCTION:
    case DISJUNCTION:
        _visitCondition(condition->leftCondition);
        _visitCondition(condition->rightCondition);
        break;
    case NEGATION:
        _visitCondition(condition->negatedCondition);
    break;
    case COMPARISON:
        _visitExpression(condition->leftExpression);
        _visitExpression(condition->rightExpression);
        break;
    default:
        break;
    }
}

static void _visitStageList(StageList* stageList){
    for(StageList* list = stageList; list != NULL; list = list->next){
        _visitStage(list->stage);
    }
}

static void _visitStage(Stage* stage){
    switch (stage->type){
    case FILTER_STAGE:
        _visitFilterStage(stage->filterStage);
        break;
    case MUTATE_STAGE:
        _visitMutateStage(stage->mutateStage);
        break;
    case SELECT_STAGE:
        _visitSelectStage(stage->selectStage);
        break;
    case SORT_STAGE:
        _visitSortStage(stage->sortStage);
        break;
    case LIMIT_STAGE:
        _visitLimitStage(stage->limitStage);
        break;
    case APPLY_STAGE:
        _visitApplyStage(stage->applyStage);
        break;
    default:
        break;
    }
}

static void _declareIdentifierList(IdentifierList* identifierList, SymbolKind kind){
    for(IdentifierList* list = identifierList; list != NULL; list = list->next){
        _declare(list->identifier, (Symbol){ .kind = kind });
    }
}

static void _declare(char* name, Symbol symbol){
    if(checkSymbol(name)){
        logError(_logger, "'%s' ya fue declarada en este scope", name);
        _failed = true;
        return;
    }

    Symbol* newSymbol = malloc(sizeof(Symbol));
    if(newSymbol == NULL){
        logError(_logger, "No hay memoria para el simbolo '%s'", name);
        _failed = true;
        return;
    }

    *newSymbol = symbol;
    if(!addSymbol(name, newSymbol)){
        logError(_logger, "No se pudo agregar '%s' a la tabla de simbolos", name);
        free(newSymbol);
        _failed = true;
    }
}


static void _visitExpression(Expression* expression){
    switch (expression->type){
    // Estos cuatro casos son iguales, los tratamos igual a todos
    case ADDITION:        
    case DIVISION:
    case MULTIPLICATION:
    case SUBTRACTION:
        _visitExpression(expression->leftExpression);
        _visitExpression(expression->rightExpression);
        break;
    case NUMBER_LITERAL:
        break;
    case STRING_LITERAL:
        break;
    case BOOLEAN_LITERAL:
        break;
    case IDENTIFIER:
        if(lookupSymbol(expression->identifier) == NULL){
            logError(_logger, "El identificador '%s' no fue declarado previamente", expression->identifier);
            _failed = true;
        }
        break;
    case EXTERNAL_PARAMETER:
        break;
    case FUNCTION_CALL:{
        Symbol* symbol = lookupSymbol(expression->functionName);
        if(symbol == NULL){
            logError(_logger, "La funcion '%s' no fue declarada previamente", expression->functionName);
            _failed = true;
        } else if (symbol->kind != FUNCTION_SYMBOL){
            logError(_logger, "'%s' no es una funcion", expression->functionName);
            _failed = true;
        }
        _visitExpressionList(expression->arguments);
        break;
    }
    case COLUMN_REFERENCE:
        break;

    default:
        break;
    }
}

static void _visitFilterStage(FilterStage *filterStage){
    _visitCondition(filterStage->condition);
}

static void _visitMutateStage(MutateStage *mutateStage){

}

static void _visitSelectStage(SelectStage *selectStage){
    _visitIdentifierList(selectStage->columns);
}

static void _visitLimitStage(LimitStage *limitStage){
    _visitExpression(limitStage->count);
}

static void _visitApplyStage(ApplyStage *applyStage){
    Symbol* symbol = lookupSymbol(applyStage->pipelineName);
    if(symbol == NULL){
        logError(_logger, "El pipeline '%s' no fue declarado previamente", applyStage->pipelineName);
        _failed = true;
    } else if (symbol->kind != PIPELINE_SYMBOL){
        logError(_logger, "'%s' no es un pipeline", applyStage->pipelineName);
        _failed = true;
    }
    _visitExpressionList(applyStage->arguments);
}

static void _visitSortStage(SortStage *sortStage){

}

static void _visitProgram(Program *program){
    _visitDeclarationList(program->declarations);
    switch (program->type){
    case SCRIPT_PROGRAM:
        _visitScriptDeclaration(program->script);
        break;
    case FOR_PROGRAM:
        _visitForStatement(program->forStatement);
        break;
    default:
        break;
    }
}

static void _visitIdentifierList(IdentifierList *identifierList){
}

static void _visitExpressionList(ExpressionList *expressionList){
    for(ExpressionList* list = expressionList; list != NULL; list = list->next){
        _visitExpression(list->expression);
    }
}

static void _visitTupleList(TupleList *tupleList){
    for(TupleList* list = tupleList; list != NULL; list = list->next){
        _visitExpressionList(list->tuple);
    }
}

static void _visitMutateExpression(MutateExpression *mutateExpression){
    switch (mutateExpression->type){
    case CONDITIONAL_MUTATE_EXPRESSION:
        _visitCondition(mutateExpression->condition);
        _visitExpression(mutateExpression->thenExpression);
        _visitExpression(mutateExpression->elseExpression);
        break;
    case SIMPLE_MUTATE_EXPRESSION:
        _visitExpression(mutateExpression->expression);
        break;
    default:
        break;
    }
}

static void _visitScriptDeclaration(ScriptDeclaration *scriptDeclaration){
    
}

static void _visitScriptBody(ScriptBody *scriptBody)
{
}

static void _visitBlock(Block *block)
{
}

static void _visitForStatement(ForStatement *forStatement){
    if(!enterScope()){
        logError(_logger, "No hay memoria para el scope del for");
        _failed = true;
        return;
    }
    _declareIdentifierList(forStatement->identifiers, FOR_VARIABLE_SYMBOL);
    _visitTupleList(forStatement->values);
    _visitScriptDeclaration(forStatement->body);
    exitScope();
}

CompilationStatus executeSemanticAnalysis(Program* program) {
    if (!initScopes() || !enterScope()) {
        return OUT_OF_MEMORY;
    }
    _visitProgram(program);
    destroyScopes();
    return _failed ? FAILED : SUCCEEDED;
}

