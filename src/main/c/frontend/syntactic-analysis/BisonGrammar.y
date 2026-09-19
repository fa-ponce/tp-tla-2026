%{

#include "../../support/type/TokenLabel.h"
#include "AbstractSyntaxTree.h"
#include "BisonActions.h"

/**
 * The error reporting function for Bison parser.
 *
 * @todo Add location to the grammar and "pushToken" API function.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Error-Reporting-Function.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Tracking-Locations.html
 */
void yyerror(const YYLTYPE * location, const char * message) {}

%}

// You touch this, and you die.
%define api.pure full
%define api.push-pull push
%define api.value.union.name SemanticValue
%define parse.error detailed
%locations

%union {
	/** Terminals. */

	TokenLabel token;
	char* string;
	double number;

	/** Non-terminals. */

	Constant * constant;
	Expression * expression;
	Factor * factor;
	Program * program;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parsing succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
%destructor { destroyConstant($$); } <constant>
%destructor { destroyExpression($$); } <expression>
%destructor { destroyFactor($$); } <factor>

/** Terminals. */
%token <token>  LOAD
%token <token>  SAVE
%token <token>  FILTER
%token <token>  MUTATE
%token <token>  SELECT
%token <token>  SORT
%token <token>  BY
%token <token>  LIMIT
%token <token>  ASC
%token <token>  DESC
%token <token>  IF
%token <token>  THEN
%token <token>  ELSE
%token <token>  APPLY
%token <token>  AND
%token <token>  OR
%token <token>  NOT
%token <token>  LET
%token <token>  FUNCTION 
%token <token>  RETURN
%token <token>  PIPELINE
%token <token>  REQUIRE
%token <token>  FOR
%token <token>  IN

%token <token>  OPEN_CURLY_BRACKET
%token <token>  CLOSE_CURLY_BRACKET
%token <token>  OPEN_PARENTHESIS
%token <token>  CLOSE_PARENTHESIS
%token <token>  OPEN_BRACKET
%token <token>  CLOSE_BRACKET

%token <token>  COMMA
%token <token>  SEMICOLON

%token <token>  EQUAL
%token <token>  NOT_EQUAL

%token <token>  SUB
%token <token>  MUL
%token <token>  DIV
%token <token>  ADD 

%token <token>  LOWER_THAN_SIGN
%token <token>  HIGHER_THAN_SIGN
%token <token>  LOWER_OR_EQUAL_THAN_SIGN
%token <token>  HIGHER_OR_EQUAL_THAN_SIGN

%token <number> NUMBER
%token <string> STRING
%token <string> IDENT

%token <token> IGNORED
%token <token> UNKNOWN

/** Non-terminals. */
%type <constant> constant
%type <expression> expression
%type <factor> factor
%type <program> program

/**
 * Precedence and associativity.
 *
 * @see https://en.cppreference.com/w/cpp/language/operator_precedence.html
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
%left ADD SUB
%left MUL DIV

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: expression											{ $$ = ExpressionProgramSemanticAction($1); }
	;

expression: expression[left] ADD expression[right]			{ $$ = ArithmeticExpressionSemanticAction($left, $right, ADDITION); }
	| expression[left] DIV expression[right]				{ $$ = ArithmeticExpressionSemanticAction($left, $right, DIVISION); }
	| expression[left] MUL expression[right]				{ $$ = ArithmeticExpressionSemanticAction($left, $right, MULTIPLICATION); }
	| expression[left] SUB expression[right]				{ $$ = ArithmeticExpressionSemanticAction($left, $right, SUBTRACTION); }
	| factor												{ $$ = FactorExpressionSemanticAction($1); }
	;

factor: OPEN_PARENTHESIS expression CLOSE_PARENTHESIS		{ $$ = ExpressionFactorSemanticAction($2); }
	| constant												{ $$ = ConstantFactorSemanticAction($1); }
	;

constant: INTEGER											{ $$ = IntegerConstantSemanticAction($1); }
	;

%%
