#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../support/logging/Logger.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/TokenLabel.h"
#include <stdbool.h>
#include <stdlib.h>

/** Initialize module's internal state. */
ModuleDestructor initializeAbstractSyntaxTreeModule();

/**
 * This type definitions allows self-referencing types (e.g., an expression
 * that is made of another expressions, such as talking about you in 3rd
 * person, but without the madness).
 */

typedef enum ProgramType ProgramType;
typedef enum DeclarationType DeclarationType;
typedef enum StageType StageType;
typedef enum MutateExpressionType MutateExpressionType;
typedef enum ScriptBodyType ScriptBodyType;
typedef enum ConditionType ConditionType;
typedef enum ExpressionType ExpressionType;

typedef struct Program Program;
typedef struct DeclarationList DeclarationList;
typedef struct Declaration Declaration;
typedef struct LetDeclaration LetDeclaration;
typedef struct FunctionDeclaration FunctionDeclaration;
typedef struct PipelineDeclaration PipelineDeclaration;
typedef struct IdentifierList IdentifierList;
typedef struct ExpressionList ExpressionList;
typedef struct StageList StageList;
typedef struct Stage Stage;
typedef struct FilterStage FilterStage;
typedef struct MutateStage MutateStage;
typedef struct MutateExpression MutateExpression;
typedef struct SelectStage SelectStage;
typedef struct SortStage SortStage;
typedef struct LimitStage LimitStage;
typedef struct ApplyStage ApplyStage;
typedef struct ScriptDeclaration ScriptDeclaration;
typedef struct ScriptBody ScriptBody;
typedef struct Block Block;
typedef struct ForStatement ForStatement;
typedef struct TupleList TupleList;
typedef struct Condition Condition;
typedef struct Expression Expression;
struct Symbol;

/**
 * Node types for the Abstract Syntax Tree (AST).
 *
 * Enum values can't reuse Bison token names (NUMBER, STRING, AND, etc.),
 * because they share the same C namespace.
 */

enum ProgramType {
	SCRIPT_PROGRAM,
	FOR_PROGRAM
};

enum DeclarationType {
	LET_DECLARATION,
	FUNCTION_DECLARATION,
	PIPELINE_DECLARATION
};

enum StageType {
	FILTER_STAGE,
	MUTATE_STAGE,
	SELECT_STAGE,
	SORT_STAGE,
	LIMIT_STAGE,
	APPLY_STAGE
};

enum MutateExpressionType {
	CONDITIONAL_MUTATE_EXPRESSION,
	SIMPLE_MUTATE_EXPRESSION
};

enum ScriptBodyType {
	STAGE_SCRIPT_BODY,
	BLOCK_SCRIPT_BODY,
	SAVE_SCRIPT_BODY
};

enum ConditionType {
	DISJUNCTION,
	CONJUNCTION,
	NEGATION,
	COMPARISON
};

enum ExpressionType {
	ADDITION,
	DIVISION,
	MULTIPLICATION,
	SUBTRACTION,
	NUMBER_LITERAL,
	STRING_LITERAL,
	BOOLEAN_LITERAL,
	IDENTIFIER,
	EXTERNAL_PARAMETER,
	COLUMN_REFERENCE,
	FUNCTION_CALL
};

/** Program structure. */

struct Program {
	DeclarationList * declarations;
	union {
		ScriptDeclaration * script;
		ForStatement * forStatement;
	};
	ProgramType type;
};

struct Declaration {
	union {
		LetDeclaration * letDeclaration;
		FunctionDeclaration * functionDeclaration;
		PipelineDeclaration * pipelineDeclaration;
	};
	DeclarationType type;
};

struct LetDeclaration {
	char * name;
	struct Symbol * symbol;
	Expression * value;
};

struct FunctionDeclaration {
	char * name;
	struct Symbol * symbol;
	IdentifierList * parameters;
	Expression * returnExpression;
};

struct PipelineDeclaration {
	char * name;
	struct Symbol * symbol;
	IdentifierList * parameters;
	Condition * requirement;
	StageList * stages;
};

/** Lists (NULL represents an empty list). */

struct DeclarationList {
	Declaration * declaration;
	DeclarationList * next;
};

struct IdentifierList {
	char * identifier;
	struct Symbol * symbol;
	IdentifierList * next;
};

struct ExpressionList {
	Expression * expression;
	ExpressionList * next;
};

struct StageList {
	Stage * stage;
	StageList * next;
};

struct TupleList {
	ExpressionList * tuple;
	TupleList * next;
};

/** Stages. */

struct Stage {
	union {
		FilterStage * filterStage;
		MutateStage * mutateStage;
		SelectStage * selectStage;
		SortStage * sortStage;
		LimitStage * limitStage;
		ApplyStage * applyStage;
	};
	StageType type;
};

struct FilterStage {
	Condition * condition;
};

struct MutateStage {
	char * column;
	MutateExpression * expression;
};

struct MutateExpression {
	union {
		Expression * expression;
		struct {
			Condition * condition;
			Expression * thenExpression;
			Expression * elseExpression;
		};
	};
	MutateExpressionType type;
};

struct SelectStage {
	IdentifierList * columns;
};

struct SortStage {
	char * column;
	TokenLabel direction;
};

struct LimitStage {
	Expression * count;
};

struct ApplyStage {
	char * pipelineName;
	struct Symbol * symbol;
	ExpressionList * arguments;
};

/** Script body and branching blocks. */

struct ScriptDeclaration {
	Expression * loadPath;
	ScriptBody * body;
};

struct ScriptBody {
	union {
		Stage * stage;
		Block * block;
		Expression * savePath;
	};
	ScriptBodyType type;
	ScriptBody * next;
};

struct Block {
	StageList * stages;
	Expression * savePath;
};

/** Iteration over variants. */

struct ForStatement {
	IdentifierList * identifiers;
	TupleList * values;
	ScriptDeclaration * body;
};

/** Boolean conditions. */

struct Condition {
	union {
		struct {
			Condition * leftCondition;
			Condition * rightCondition;
		};
		Condition * negatedCondition;
		struct {
			Expression * leftExpression;
			TokenLabel comparisonOperator;
			Expression * rightExpression;
		};
	};
	ConditionType type;
};

/** Arithmetic expressions. */

struct Expression {
	union {
		struct {
			Expression * leftExpression;
			Expression * rightExpression;
		};
		double number;
		char * string;
		bool boolean;
		char * identifier;
		char * externalParameter;
		char * column;
		struct {
			char * functionName;
			ExpressionList * arguments;
		};
	};
	struct Symbol * symbol;
	ExpressionType type;
};

/**
 * Node recursive super-duper-trambolik-destructors.
 */

void destroyProgram(Program * program);
void destroyDeclarationList(DeclarationList * declarationList);
void destroyDeclaration(Declaration * declaration);
void destroyLetDeclaration(LetDeclaration * letDeclaration);
void destroyFunctionDeclaration(FunctionDeclaration * functionDeclaration);
void destroyPipelineDeclaration(PipelineDeclaration * pipelineDeclaration);
void destroyIdentifierList(IdentifierList * identifierList);
void destroyExpressionList(ExpressionList * expressionList);
void destroyStageList(StageList * stageList);
void destroyStage(Stage * stage);
void destroyFilterStage(FilterStage * filterStage);
void destroyMutateStage(MutateStage * mutateStage);
void destroyMutateExpression(MutateExpression * mutateExpression);
void destroySelectStage(SelectStage * selectStage);
void destroySortStage(SortStage * sortStage);
void destroyLimitStage(LimitStage * limitStage);
void destroyApplyStage(ApplyStage * applyStage);
void destroyScriptDeclaration(ScriptDeclaration * scriptDeclaration);
void destroyScriptBody(ScriptBody * scriptBody);
void destroyBlock(Block * block);
void destroyForStatement(ForStatement * forStatement);
void destroyTupleList(TupleList * tupleList);
void destroyCondition(Condition * condition);
void destroyExpression(Expression * expression);

#endif
