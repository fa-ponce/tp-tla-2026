#include "Generator.h"
#include "../semantic-analysis/SymbolTable.h"
#include "../../frontend/syntactic-analysis/BisonParser.h"

/* MODULE INTERNAL STATE */

static const unsigned int _indentationSize = 4;
static Logger * _logger = NULL;
static FILE * _output = NULL;

// Funciones de soporte de Python, se emiten igual en todos los programas generados
static const char * const _prologue[] = {
	"import csv",
	"import sys",
	"",
	"def _cell(text):",
	"    if text == \"true\" or text == \"false\":",
	"        return text == \"true\"",
	"    try:",
	"        return float(text)",
	"    except ValueError:",
	"        return text",
	"",
	"def _str(value):",
	"    if isinstance(value, bool):",
	"        return \"true\" if value else \"false\"",
	"    if isinstance(value, (int, float)):",
	"        value = round(float(value), 10)",
	"        return str(int(value)) if value.is_integer() else repr(value)",
	"    return value",
	"",
	"def _add(left, right):",
	"    if isinstance(left, str) or isinstance(right, str):",
	"        return _str(left) + _str(right)",
	"    return left + right",
	"",
	"def _param(name):",
	"    for argument in sys.argv[1:]:",
	"        key, separator, value = argument.partition(\"=\")",
	"        if separator and key == name:",
	"            return _cell(value)",
	"    sys.exit(\"Falta el parametro externo '\" + name + \"' (usar \" + name + \"=<valor>)\")",
	"",
	"def _require(condition, pipeline):",
	"    if not condition:",
	"        sys.exit(\"No se cumple el require del pipeline '\" + pipeline + \"'\")",
	"",
	"def _load(path):",
	"    with open(path, newline=\"\") as file:",
	"        return [{key: _cell(value) for key, value in row.items()} for row in csv.DictReader(file)]",
	"",
	"def _save(rows, path):",
	"    with open(path, \"w\", newline=\"\") as file:",
	"        if rows:",
	"            writer = csv.DictWriter(file, fieldnames=list(rows[0].keys()))",
	"            writer.writeheader()",
	"            for row in rows:",
	"                writer.writerow({key: _str(value) for key, value in row.items()})",
	NULL
};

/** Shutdown module's internal state. */
void _shutdownGeneratorModule() {
	if (_logger != NULL) {
		logDebugging(_logger, "Destroying module: Generator...");
		destroyLogger(_logger);
		_logger = NULL;
	}
}

ModuleDestructor initializeGeneratorModule() {
	_logger = createLogger("Generator");
	return _shutdownGeneratorModule;
}

/** PRIVATE FUNCTIONS */

static void _emit(const char * const format, ...);
static void _indent(const unsigned int level);
static void _generateName(const char * name, Symbol * symbol);
static ValueType _staticType(Expression * expression);
static void _generateExpression(Expression * expression);
static void _generateAddition(Expression * expression);
static void _generateAsString(Expression * expression, ValueType type);
static void _generateArguments(ExpressionList * arguments);
static void _generateParameters(IdentifierList * parameters);
static const char * _comparisonOperator(TokenLabel operator);
static const char _arithmeticOperator(ExpressionType type);
static void _generateCondition(Condition * condition);
static void _generateMutateExpression(MutateExpression * mutate);
static void _generateStage(const unsigned int level, Stage * stage, const char * rows);
static void _generateSave(const unsigned int level, Expression * path, const char * rows);
static void _generateBlock(const unsigned int level, Block * block);
static void _generateScript(const unsigned int level, ScriptDeclaration * script);
static void _generateFor(ForStatement * forStatement);
static void _generateFunction(FunctionDeclaration * function);
static void _generatePipeline(PipelineDeclaration * pipeline);
static void _generateProgram(Program * program);

static void _emit(const char * const format, ...) {
	va_list arguments;
	va_start(arguments, format);
	vfprintf(_output, format, arguments);
	va_end(arguments);
}

static void _indent(const unsigned int level) {
	_emit("%*s", (int) (level * _indentationSize), "");
}

// Todo nombre lleva el id de su simbolo: evita choques por shadowing, con palabras
// reservadas de Python y con los nombres auxiliares (row, rows, branch, _load, ...)
static void _generateName(const char * name, Symbol * symbol) {
	_emit("%s_%d", name, symbol->id);
}

// Tipo conocido al generar. Parametros y variables del for son UNKNOWN porque el codigo
// de un pipeline o del cuerpo del for se genera una sola vez para todos sus usos
static ValueType _staticType(Expression * expression) {
	switch (expression->type) {
		case NUMBER_LITERAL:
			return NUMBER_TYPE;
		case STRING_LITERAL:
			return STRING_TYPE;
		case BOOLEAN_LITERAL:
			return BOOLEAN_TYPE;
		case IDENTIFIER:
			return expression->symbol->kind == LET_SYMBOL ? expression->symbol->type : UNKNOWN_TYPE;
		case FUNCTION_CALL:
			return expression->symbol->type;
		case ADDITION: {
			ValueType left = _staticType(expression->leftExpression);
			ValueType right = _staticType(expression->rightExpression);
			if (left == STRING_TYPE || right == STRING_TYPE) {
				return STRING_TYPE;
			}
			return (left == NUMBER_TYPE && right == NUMBER_TYPE) ? NUMBER_TYPE : UNKNOWN_TYPE;
		}
		case SUBTRACTION:
		case MULTIPLICATION:
		case DIVISION:
			return NUMBER_TYPE;
		default:
			return UNKNOWN_TYPE;
	}
}

static const char _arithmeticOperator(ExpressionType type) {
	switch (type) {
		case SUBTRACTION: return '-';
		case MULTIPLICATION: return '*';
		case DIVISION: return '/';
		default: return '+';
	}
}

static void _generateExpression(Expression * expression) {
	switch (expression->type) {
		case NUMBER_LITERAL:
			_emit("%.15g", expression->number);
			break;
		case STRING_LITERAL:
			_emit("\"%s\"", expression->string);
			break;
		case BOOLEAN_LITERAL:
			_emit("%s", expression->boolean ? "True" : "False");
			break;
		case IDENTIFIER:
			_generateName(expression->identifier, expression->symbol);
			break;
		case EXTERNAL_PARAMETER:
			_emit("_param(\"%s\")", expression->externalParameter);
			break;
		case COLUMN_REFERENCE:
			_emit("row[\"%s\"]", expression->column);
			break;
		case FUNCTION_CALL:
			_generateName(expression->functionName, expression->symbol);
			_emit("(");
			_generateArguments(expression->arguments);
			_emit(")");
			break;
		case ADDITION:
			_generateAddition(expression);
			break;
		case SUBTRACTION:
		case MULTIPLICATION:
		case DIVISION:
			_emit("(");
			_generateExpression(expression->leftExpression);
			_emit(" %c ", _arithmeticOperator(expression->type));
			_generateExpression(expression->rightExpression);
			_emit(")");
			break;
		default:
			break;
	}
}

// + suma o concatena segun el tipo; si no se conoce al generar, decide _add al ejecutar
static void _generateAddition(Expression * expression) {
	ValueType left = _staticType(expression->leftExpression);
	ValueType right = _staticType(expression->rightExpression);
	if (left == STRING_TYPE || right == STRING_TYPE) {
		_emit("(");
		_generateAsString(expression->leftExpression, left);
		_emit(" + ");
		_generateAsString(expression->rightExpression, right);
		_emit(")");
	}
	else if (left == NUMBER_TYPE && right == NUMBER_TYPE) {
		_emit("(");
		_generateExpression(expression->leftExpression);
		_emit(" + ");
		_generateExpression(expression->rightExpression);
		_emit(")");
	}
	else {
		_emit("_add(");
		_generateExpression(expression->leftExpression);
		_emit(", ");
		_generateExpression(expression->rightExpression);
		_emit(")");
	}
}

static void _generateAsString(Expression * expression, ValueType type) {
	if (type == STRING_TYPE) {
		_generateExpression(expression);
		return;
	}
	_emit("_str(");
	_generateExpression(expression);
	_emit(")");
}

static void _generateArguments(ExpressionList * arguments) {
	for (ExpressionList * list = arguments; list != NULL; list = list->next) {
		_generateExpression(list->expression);
		if (list->next != NULL) {
			_emit(", ");
		}
	}
}

static void _generateParameters(IdentifierList * parameters) {
	for (IdentifierList * list = parameters; list != NULL; list = list->next) {
		_generateName(list->identifier, list->symbol);
		if (list->next != NULL) {
			_emit(", ");
		}
	}
}

static const char * _comparisonOperator(TokenLabel operator) {
	switch (operator) {
		case NOT_EQUAL: return "!=";
		case LOWER_THAN_SIGN: return "<";
		case LOWER_OR_EQUAL_THAN_SIGN: return "<=";
		case HIGHER_THAN_SIGN: return ">";
		case HIGHER_OR_EQUAL_THAN_SIGN: return ">=";
		default: return "==";
	}
}

static void _generateCondition(Condition * condition) {
	switch (condition->type) {
		case CONJUNCTION:
		case DISJUNCTION:
			_emit("(");
			_generateCondition(condition->leftCondition);
			_emit(condition->type == CONJUNCTION ? " and " : " or ");
			_generateCondition(condition->rightCondition);
			_emit(")");
			break;
		case NEGATION:
			_emit("(not ");
			_generateCondition(condition->negatedCondition);
			_emit(")");
			break;
		case COMPARISON:
			_emit("(");
			_generateExpression(condition->leftExpression);
			_emit(" %s ", _comparisonOperator(condition->comparisonOperator));
			_generateExpression(condition->rightExpression);
			_emit(")");
			break;
		default:
			break;
	}
}

static void _generateMutateExpression(MutateExpression * mutate) {
	if (mutate->type == CONDITIONAL_MUTATE_EXPRESSION) {
		_emit("(");
		_generateExpression(mutate->thenExpression);
		_emit(" if ");
		_generateCondition(mutate->condition);
		_emit(" else ");
		_generateExpression(mutate->elseExpression);
		_emit(")");
	}
	else {
		_generateExpression(mutate->expression);
	}
}

// Cada stage crea una lista nueva en vez de modificar las filas: asi un bloque
// puede trabajar sobre su rama sin afectar a las filas del programa principal
static void _generateStage(const unsigned int level, Stage * stage, const char * rows) {
	_indent(level);
	switch (stage->type) {
		case FILTER_STAGE:
			_emit("%s = [row for row in %s if ", rows, rows);
			_generateCondition(stage->filterStage->condition);
			_emit("]\n");
			break;
		case MUTATE_STAGE:
			_emit("%s = [{**row, \"%s\": ", rows, stage->mutateStage->column);
			_generateMutateExpression(stage->mutateStage->expression);
			_emit("} for row in %s]\n", rows);
			break;
		case SELECT_STAGE:
			_emit("%s = [{", rows);
			for (IdentifierList * column = stage->selectStage->columns; column != NULL; column = column->next) {
				_emit("\"%s\": row[\"%s\"]%s", column->identifier, column->identifier, column->next == NULL ? "" : ", ");
			}
			_emit("} for row in %s]\n", rows);
			break;
		case SORT_STAGE:
			_emit("%s = sorted(%s, key=lambda row: row[\"%s\"]%s)\n", rows, rows, stage->sortStage->column,
				stage->sortStage->direction == DESC ? ", reverse=True" : "");
			break;
		case LIMIT_STAGE:
			_emit("%s = %s[:int(", rows, rows);
			_generateExpression(stage->limitStage->count);
			_emit(")]\n");
			break;
		case APPLY_STAGE:
			_emit("%s = ", rows);
			_generateName(stage->applyStage->pipelineName, stage->applyStage->symbol);
			_emit("(%s", rows);
			for (ExpressionList * argument = stage->applyStage->arguments; argument != NULL; argument = argument->next) {
				_emit(", ");
				_generateExpression(argument->expression);
			}
			_emit(")\n");
			break;
		default:
			break;
	}
}

static void _generateSave(const unsigned int level, Expression * path, const char * rows) {
	_indent(level);
	_emit("_save(%s, ", rows);
	_generateExpression(path);
	_emit(")\n");
}

static void _generateBlock(const unsigned int level, Block * block) {
	_indent(level);
	_emit("branch = rows\n");
	for (StageList * list = block->stages; list != NULL; list = list->next) {
		_generateStage(level, list->stage, "branch");
	}
	_generateSave(level, block->savePath, "branch");
}

static void _generateScript(const unsigned int level, ScriptDeclaration * script) {
	_indent(level);
	_emit("rows = _load(");
	_generateExpression(script->loadPath);
	_emit(")\n");
	for (ScriptBody * body = script->body; body != NULL; body = body->next) {
		switch (body->type) {
			case STAGE_SCRIPT_BODY:
				_generateStage(level, body->stage, "rows");
				break;
			case BLOCK_SCRIPT_BODY:
				_generateBlock(level, body->block);
				break;
			case SAVE_SCRIPT_BODY:
				_generateSave(level, body->savePath, "rows");
				break;
			default:
				break;
		}
	}
}

static void _generateFor(ForStatement * forStatement) {
	bool single = forStatement->identifiers->next == NULL;
	_emit(single ? "for " : "for (");
	_generateParameters(forStatement->identifiers);
	_emit(single ? " in [" : ") in [");
	for (TupleList * variant = forStatement->values; variant != NULL; variant = variant->next) {
		_emit(single ? "" : "(");
		_generateArguments(variant->tuple);
		_emit(single ? "" : ")");
		_emit(variant->next == NULL ? "" : ", ");
	}
	_emit("]:\n");
	_generateScript(1, forStatement->body);
}

static void _generateFunction(FunctionDeclaration * function) {
	_emit("def ");
	_generateName(function->name, function->symbol);
	_emit("(");
	_generateParameters(function->parameters);
	_emit("):\n");
	_indent(1);
	_emit("return ");
	_generateExpression(function->returnExpression);
	_emit("\n\n");
}

static void _generatePipeline(PipelineDeclaration * pipeline) {
	_emit("def ");
	_generateName(pipeline->name, pipeline->symbol);
	_emit("(rows");
	for (IdentifierList * parameter = pipeline->parameters; parameter != NULL; parameter = parameter->next) {
		_emit(", ");
		_generateName(parameter->identifier, parameter->symbol);
	}
	_emit("):\n");
	// El require no se valida al compilar, asi que se chequea al ejecutar
	if (pipeline->requirement != NULL) {
		_indent(1);
		_emit("_require(");
		_generateCondition(pipeline->requirement);
		_emit(", \"%s\")\n", pipeline->name);
	}
	for (StageList * list = pipeline->stages; list != NULL; list = list->next) {
		_generateStage(1, list->stage, "rows");
	}
	_indent(1);
	_emit("return rows\n\n");
}

static void _generateProgram(Program * program) {
	for (int k = 0; _prologue[k] != NULL; ++k) {
		_emit("%s\n", _prologue[k]);
	}
	_emit("\n");
	// Primero functions y pipelines: en el lenguaje se pueden usar antes de declararse
	for (DeclarationList * node = program->declarations; node != NULL; node = node->next) {
		if (node->declaration->type == FUNCTION_DECLARATION) {
			_generateFunction(node->declaration->functionDeclaration);
		}
		else if (node->declaration->type == PIPELINE_DECLARATION) {
			_generatePipeline(node->declaration->pipelineDeclaration);
		}
	}
	for (DeclarationList * node = program->declarations; node != NULL; node = node->next) {
		if (node->declaration->type == LET_DECLARATION) {
			LetDeclaration * let = node->declaration->letDeclaration;
			_generateName(let->name, let->symbol);
			_emit(" = ");
			_generateExpression(let->value);
			_emit("\n");
		}
	}
	_emit("\n");
	if (program->type == SCRIPT_PROGRAM) {
		_generateScript(0, program->script);
	}
	else {
		_generateFor(program->forStatement);
	}
}

/** PUBLIC FUNCTIONS */

CompilationStatus executeGenerator(CompilerState * compilerState) {
	logDebugging(_logger, "Generating Python output in \"%s\"...", compilerState->outputPath);
	_output = fopen(compilerState->outputPath, "w");
	if (_output == NULL) {
		logError(_logger, "No se pudo abrir el archivo de salida \"%s\"", compilerState->outputPath);
		return FAILED;
	}
	_generateProgram(compilerState->abstractSyntaxtTree);
	bool failed = ferror(_output);
	fclose(_output);
	_output = NULL;
	if (failed) {
		logError(_logger, "No se pudo escribir el archivo de salida \"%s\"", compilerState->outputPath);
		return FAILED;
	}
	logDebugging(_logger, "Generation is done.");
	return SUCCEEDED;
}
