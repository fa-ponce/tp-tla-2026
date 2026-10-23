#include "TypeChecker.h"
#include "SymbolTable.h"

static ValueType _typeOf(Expression* expression);
static ValueType _inferFunction(Symbol* function);
static void _checkCondition(Condition* condition);
static void _checkStageList(StageList* stageList);
static void _checkStage(Stage* stage);
static void _checkApply(ApplyStage* applyStage);
static void _checkScript(ScriptDeclaration* script);
static void _checkFor(ForStatement* forStatement);
static void _expect(ValueType actual, ValueType expected, const char* context);
static const char* _typeName(ValueType type);

static Logger* _logger = NULL;
static bool _failed = false;

static void _shutdownTypeCheckerModule() {
    if (_logger != NULL) {
        destroyLogger(_logger);
        _logger = NULL;
    }
}

ModuleDestructor initializeTypeCheckerModule() {
    _logger = createLogger("TypeChecker");
    return _shutdownTypeCheckerModule;
}

static const char* _typeName(ValueType type){
    switch(type){
        case NUMBER_TYPE: return "number";
        case STRING_TYPE: return "string";
        case BOOLEAN_TYPE: return "boolean";
        default: return "unknown";
    }
}

// UNKNOWN se acepta siempre: el chequeo queda para la ejecucion
static void _expect(ValueType actual, ValueType expected, const char* context){
    if(actual != UNKNOWN_TYPE && actual != expected){
        logError(_logger, "%s: se esperaba %s y se encontro %s", context, _typeName(expected), _typeName(actual));
        _failed = true;
    }
}

static ValueType _typeOf(Expression* expression){
    switch(expression->type){
    case NUMBER_LITERAL:
        return NUMBER_TYPE;
    case STRING_LITERAL:
        return STRING_TYPE;
    case BOOLEAN_LITERAL:
        return BOOLEAN_TYPE;
    case COLUMN_REFERENCE:
    case EXTERNAL_PARAMETER:
        return UNKNOWN_TYPE;
    case IDENTIFIER:
        return expression->symbol->type;
    case FUNCTION_CALL:
        // Los parametros son UNKNOWN, pero los argumentos pueden tener errores propios
        for(ExpressionList* list = expression->arguments; list != NULL; list = list->next){
            _typeOf(list->expression);
        }
        return _inferFunction(expression->symbol);
    case ADDITION: {
        ValueType left = _typeOf(expression->leftExpression);
        ValueType right = _typeOf(expression->rightExpression);
        // Con un string de cualquier lado, + concatena
        if(left == STRING_TYPE || right == STRING_TYPE){
            return STRING_TYPE;
        }
        _expect(left, NUMBER_TYPE, "Operando izquierdo de +");
        _expect(right, NUMBER_TYPE, "Operando derecho de +");
        return (left == UNKNOWN_TYPE || right == UNKNOWN_TYPE) ? UNKNOWN_TYPE : NUMBER_TYPE;
    }
    case SUBTRACTION:
    case MULTIPLICATION:
    case DIVISION:
        _expect(_typeOf(expression->leftExpression), NUMBER_TYPE, "Operando izquierdo aritmetico");
        _expect(_typeOf(expression->rightExpression), NUMBER_TYPE, "Operando derecho aritmetico");
        return NUMBER_TYPE;
    default:
        return UNKNOWN_TYPE;
    }
}

static ValueType _inferFunction(Symbol* function){
    switch(function->state){
    case INFERENCE_DONE:
        return function->type;
    case INFERENCE_IN_PROGRESS:
        // El cuerpo es una sola expresion sin IF, asi que la recursion nunca termina
        logError(_logger, "La funcion '%s' es recursiva", function->function->name);
        _failed = true;
        return UNKNOWN_TYPE;
    default:
        function->state = INFERENCE_IN_PROGRESS;
        function->type = _typeOf(function->function->returnExpression);
        function->state = INFERENCE_DONE;
        return function->type;
    }
}

static void _checkCondition(Condition* condition){
    if(condition == NULL){
        return;
    }
    switch(condition->type){
    case CONJUNCTION:
    case DISJUNCTION:
        _checkCondition(condition->leftCondition);
        _checkCondition(condition->rightCondition);
        break;
    case NEGATION:
        _checkCondition(condition->negatedCondition);
        break;
    case COMPARISON: {
        ValueType left = _typeOf(condition->leftExpression);
        ValueType right = _typeOf(condition->rightExpression);
        if(left != UNKNOWN_TYPE && right != UNKNOWN_TYPE && left != right){
            logError(_logger, "No se puede comparar %s con %s", _typeName(left), _typeName(right));
            _failed = true;
        }
        break;
    }
    default:
        break;
    }
}

static void _checkStageList(StageList* stageList){
    for(StageList* list = stageList; list != NULL; list = list->next){
        _checkStage(list->stage);
    }
}

static void _checkStage(Stage* stage){
    switch(stage->type){
    case FILTER_STAGE:
        _checkCondition(stage->filterStage->condition);
        break;
    case MUTATE_STAGE: {
        MutateExpression* mutate = stage->mutateStage->expression;
        if(mutate->type == CONDITIONAL_MUTATE_EXPRESSION){
            _checkCondition(mutate->condition);
            _typeOf(mutate->thenExpression);
            _typeOf(mutate->elseExpression);
        } else {
            _typeOf(mutate->expression);
        }
        break;
    }
    case LIMIT_STAGE:
        _expect(_typeOf(stage->limitStage->count), NUMBER_TYPE, "LIMIT");
        break;
    case APPLY_STAGE:
        _checkApply(stage->applyStage);
        break;
    default:
        // SELECT y SORT no tienen expresiones
        break;
    }
}

// APPLY inserta las stages del pipeline en ese punto, asi que se chequean
// con los parametros tipados segun los argumentos de este APPLY
static void _checkApply(ApplyStage* applyStage){
    Symbol* pipeline = applyStage->symbol;
    if(pipeline->state == INFERENCE_IN_PROGRESS){
        logError(_logger, "El pipeline '%s' se aplica a si mismo", applyStage->pipelineName);
        _failed = true;
        return;
    }
    IdentifierList* parameter = pipeline->pipeline->parameters;
    ExpressionList* argument = applyStage->arguments;
    for(; parameter != NULL && argument != NULL; parameter = parameter->next, argument = argument->next){
        parameter->symbol->type = _typeOf(argument->expression);
    }
    pipeline->state = INFERENCE_IN_PROGRESS;
    _checkCondition(pipeline->pipeline->requirement);
    _checkStageList(pipeline->pipeline->stages);
    pipeline->state = INFERENCE_PENDING;
}

static void _checkScript(ScriptDeclaration* script){
    _expect(_typeOf(script->loadPath), STRING_TYPE, "LOAD");
    for(ScriptBody* body = script->body; body != NULL; body = body->next){
        switch(body->type){
        case STAGE_SCRIPT_BODY:
            _checkStage(body->stage);
            break;
        case BLOCK_SCRIPT_BODY:
            _checkStageList(body->block->stages);
            _expect(_typeOf(body->block->savePath), STRING_TYPE, "SAVE");
            break;
        case SAVE_SCRIPT_BODY:
            _expect(_typeOf(body->savePath), STRING_TYPE, "SAVE");
            break;
        default:
            break;
        }
    }
}

// El for corre el programa una vez por variante, asi que se chequea una vez por variante
static void _checkFor(ForStatement* forStatement){
    for(TupleList* variant = forStatement->values; variant != NULL; variant = variant->next){
        IdentifierList* variable = forStatement->identifiers;
        ExpressionList* value = variant->tuple;
        for(; variable != NULL && value != NULL; variable = variable->next, value = value->next){
            variable->symbol->type = _typeOf(value->expression);
        }
        _checkScript(forStatement->body);
    }
}

bool executeTypeCheck(Program* program){
    _failed = false;
    for(DeclarationList* node = program->declarations; node != NULL; node = node->next){
        Declaration* declaration = node->declaration;
        switch(declaration->type){
        case LET_DECLARATION:
            declaration->letDeclaration->symbol->type = _typeOf(declaration->letDeclaration->value);
            break;
        case FUNCTION_DECLARATION:
            // Asi tambien se chequean las funciones que nadie llama
            _inferFunction(declaration->functionDeclaration->symbol);
            break;
        default:
            // Los pipelines se chequean en cada APPLY
            break;
        }
    }
    if(program->type == SCRIPT_PROGRAM){
        _checkScript(program->script);
    } else {
        _checkFor(program->forStatement);
    }
    return !_failed;
}
