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
	bool boolean;

	/** Non-terminals. */

	Program * program;
	DeclarationList * declarationList;
	Declaration * declaration;
	LetDeclaration * letDeclaration;
	FunctionDeclaration * functionDeclaration;
	PipelineDeclaration * pipelineDeclaration;
	IdentifierList * identifierList;
	ExpressionList * expressionList;
	StageList * stageList;
	Stage * stage;
	FilterStage * filterStage;
	MutateStage * mutateStage;
	MutateExpression * mutateExpression;
	SelectStage * selectStage;
	SortStage * sortStage;
	LimitStage * limitStage;
	ApplyStage * applyStage;
	ScriptDeclaration * scriptDeclaration;
	ScriptBody * scriptBody;
	Block * block;
	ForStatement * forStatement;
	TupleList * tupleList;
	Condition * condition;
	Expression * expression;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parsing succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
%destructor { free($$); } <string>
%destructor { destroyDeclarationList($$); } <declarationList>
%destructor { destroyDeclaration($$); } <declaration>
%destructor { destroyLetDeclaration($$); } <letDeclaration>
%destructor { destroyFunctionDeclaration($$); } <functionDeclaration>
%destructor { destroyPipelineDeclaration($$); } <pipelineDeclaration>
%destructor { destroyIdentifierList($$); } <identifierList>
%destructor { destroyExpressionList($$); } <expressionList>
%destructor { destroyStageList($$); } <stageList>
%destructor { destroyStage($$); } <stage>
%destructor { destroyFilterStage($$); } <filterStage>
%destructor { destroyMutateStage($$); } <mutateStage>
%destructor { destroyMutateExpression($$); } <mutateExpression>
%destructor { destroySelectStage($$); } <selectStage>
%destructor { destroySortStage($$); } <sortStage>
%destructor { destroyLimitStage($$); } <limitStage>
%destructor { destroyApplyStage($$); } <applyStage>
%destructor { destroyScriptDeclaration($$); } <scriptDeclaration>
%destructor { destroyScriptBody($$); } <scriptBody>
%destructor { destroyBlock($$); } <block>
%destructor { destroyForStatement($$); } <forStatement>
%destructor { destroyTupleList($$); } <tupleList>
%destructor { destroyCondition($$); } <condition>
%destructor { destroyExpression($$); } <expression>

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
%token <boolean> BOOLEAN
%token <string> STRING
%token <string> IDENT
%token <string> EXTERNAL_PARAM

%token <token> IGNORED
%token <token> UNKNOWN

/** Non-terminals. */
%type <program> program
%type <declarationList> declList
%type <declaration> decl
%type <letDeclaration> letDecl
%type <functionDeclaration> functionDecl
%type <pipelineDeclaration> pipelineDecl
%type <identifierList> paramList colList forBinding
%type <expressionList> argList tuple
%type <stageList> stageList
%type <stage> stage
%type <filterStage> filterStage
%type <mutateStage> mutateStage
%type <mutateExpression> mutateExpr
%type <selectStage> selectStage
%type <sortStage> sortStage
%type <token> dir relop
%type <limitStage> limitStage
%type <applyStage> applyStage
%type <scriptDeclaration> scriptDecl
%type <scriptBody> scriptBody scriptBodyAfterBlock
%type <block> block blockBody
%type <forStatement> forStmt
%type <tupleList> forValues tupleList
%type <condition> requireOpt boolExpr boolTerm boolFactor comparison
%type <expression> expr term factor primary

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.
// Empty lists and optional nodes are represented with NULL.

/** Program structure. */

program: declList scriptDecl											{ $$ = ScriptProgramSemanticAction($1, $2); }
	| declList forStmt													{ $$ = ForProgramSemanticAction($1, $2); }
	;

declList: decl declList													{ $$ = DeclarationListSemanticAction($1, $2); }
	| %empty															{ $$ = NULL; }
	;

decl: letDecl															{ $$ = LetDeclarationSemanticAction($1); }
	| functionDecl														{ $$ = FunctionDeclarationSemanticAction($1); }
	| pipelineDecl														{ $$ = PipelineDeclarationSemanticAction($1); }
	;

letDecl: LET IDENT EQUAL expr SEMICOLON									{ $$ = LetSemanticAction($2, $4); }
	;

functionDecl: FUNCTION IDENT OPEN_PARENTHESIS paramList CLOSE_PARENTHESIS
		OPEN_CURLY_BRACKET RETURN expr SEMICOLON CLOSE_CURLY_BRACKET	{ $$ = FunctionSemanticAction($2, $4, $8); }
	;

pipelineDecl: PIPELINE IDENT OPEN_PARENTHESIS paramList CLOSE_PARENTHESIS
		requireOpt OPEN_CURLY_BRACKET stageList CLOSE_CURLY_BRACKET		{ $$ = PipelineSemanticAction($2, $4, $6, $8); }
	;

requireOpt: REQUIRE boolExpr											{ $$ = $2; }
	| %empty															{ $$ = NULL; }
	;

/** Lists. */

paramList: colList														{ $$ = $1; }
	| %empty															{ $$ = NULL; }
	;

colList: IDENT COMMA colList											{ $$ = IdentifierListSemanticAction($1, $3); }
	| IDENT																{ $$ = SingleIdentifierListSemanticAction($1); }
	;

argList: expr COMMA argList												{ $$ = ExpressionListSemanticAction($1, $3); }
	| expr																{ $$ = SingleExpressionListSemanticAction($1); }
	;

stageList: stage stageList												{ $$ = StageListSemanticAction($1, $2); }
	| stage																{ $$ = SingleStageListSemanticAction($1); }
	;

/** Stages. */

stage: filterStage														{ $$ = FilterStageSemanticAction($1); }
	| mutateStage														{ $$ = MutateStageSemanticAction($1); }
	| selectStage														{ $$ = SelectStageSemanticAction($1); }
	| sortStage															{ $$ = SortStageSemanticAction($1); }
	| limitStage														{ $$ = LimitStageSemanticAction($1); }
	| applyStage														{ $$ = ApplyStageSemanticAction($1); }
	;

filterStage: FILTER boolExpr											{ $$ = FilterSemanticAction($2); }
	;

mutateStage: MUTATE IDENT EQUAL mutateExpr								{ $$ = MutateSemanticAction($2, $4); }
	;

mutateExpr: IF boolExpr THEN expr[then] ELSE expr[else]					{ $$ = ConditionalMutateExpressionSemanticAction($2, $then, $else); }
	| expr																{ $$ = SimpleMutateExpressionSemanticAction($1); }
	;

selectStage: SELECT colList												{ $$ = SelectSemanticAction($2); }
	;

sortStage: SORT BY IDENT dir											{ $$ = SortSemanticAction($3, $4); }
	;

dir: ASC																{ $$ = ASC; }
	| DESC																{ $$ = DESC; }
	;

limitStage: LIMIT expr													{ $$ = LimitSemanticAction($2); }
	;

applyStage: APPLY IDENT OPEN_PARENTHESIS argList CLOSE_PARENTHESIS		{ $$ = ApplySemanticAction($2, $4); }
	| APPLY IDENT OPEN_PARENTHESIS CLOSE_PARENTHESIS					{ $$ = ApplySemanticAction($2, NULL); }
	;

/** Script body and branching blocks. */

scriptDecl: LOAD expr scriptBody										{ $$ = ScriptSemanticAction($2, $3); }
	;

scriptBody: stage scriptBody											{ $$ = StageScriptBodySemanticAction($1, $2); }
	| block scriptBodyAfterBlock										{ $$ = BlockScriptBodySemanticAction($1, $2); }
	| SAVE expr															{ $$ = SaveScriptBodySemanticAction($2); }
	;

scriptBodyAfterBlock: scriptBody										{ $$ = $1; }
	| %empty															{ $$ = NULL; }
	;

block: OPEN_CURLY_BRACKET blockBody CLOSE_CURLY_BRACKET					{ $$ = $2; }
	;

blockBody: stage blockBody												{ $$ = StageBlockSemanticAction($1, $2); }
	| SAVE expr															{ $$ = SaveBlockSemanticAction($2); }
	;

/** Iteration over variants. */

forStmt: FOR forBinding IN OPEN_BRACKET forValues CLOSE_BRACKET
		OPEN_CURLY_BRACKET scriptDecl CLOSE_CURLY_BRACKET				{ $$ = ForSemanticAction($2, $5, $8); }
	;

forBinding: IDENT														{ $$ = SingleIdentifierListSemanticAction($1); }
	| OPEN_PARENTHESIS colList CLOSE_PARENTHESIS						{ $$ = $2; }
	;

forValues: argList														{ $$ = SingletonTupleListSemanticAction($1); }
	| tupleList															{ $$ = $1; }
	;

tupleList: tuple COMMA tupleList										{ $$ = TupleListSemanticAction($1, $3); }
	| tuple																{ $$ = SingleTupleListSemanticAction($1); }
	;

tuple: OPEN_BRACKET argList CLOSE_BRACKET								{ $$ = $2; }
	;

/** Boolean conditions. */

boolExpr: boolExpr OR boolTerm											{ $$ = LogicalConditionSemanticAction($1, $3, DISJUNCTION); }
	| boolTerm															{ $$ = $1; }
	;

boolTerm: boolTerm AND boolFactor										{ $$ = LogicalConditionSemanticAction($1, $3, CONJUNCTION); }
	| boolFactor														{ $$ = $1; }
	;

boolFactor: NOT boolFactor												{ $$ = NegationConditionSemanticAction($2); }
	| OPEN_PARENTHESIS boolExpr CLOSE_PARENTHESIS						{ $$ = $2; }
	| comparison														{ $$ = $1; }
	;

comparison: expr[left] relop expr[right]								{ $$ = ComparisonConditionSemanticAction($left, $2, $right); }
	;

relop: EQUAL															{ $$ = EQUAL; }
	| NOT_EQUAL															{ $$ = NOT_EQUAL; }
	| LOWER_THAN_SIGN													{ $$ = LOWER_THAN_SIGN; }
	| LOWER_OR_EQUAL_THAN_SIGN											{ $$ = LOWER_OR_EQUAL_THAN_SIGN; }
	| HIGHER_THAN_SIGN													{ $$ = HIGHER_THAN_SIGN; }
	| HIGHER_OR_EQUAL_THAN_SIGN											{ $$ = HIGHER_OR_EQUAL_THAN_SIGN; }
	;

/** Arithmetic expressions. */

expr: expr ADD term														{ $$ = ArithmeticExpressionSemanticAction($1, $3, ADDITION); }
	| expr SUB term														{ $$ = ArithmeticExpressionSemanticAction($1, $3, SUBTRACTION); }
	| term																{ $$ = $1; }
	;

term: term MUL factor													{ $$ = ArithmeticExpressionSemanticAction($1, $3, MULTIPLICATION); }
	| term DIV factor													{ $$ = ArithmeticExpressionSemanticAction($1, $3, DIVISION); }
	| factor															{ $$ = $1; }
	;

factor: primary															{ $$ = $1; }
	;

primary: NUMBER															{ $$ = NumberExpressionSemanticAction($1); }
	| STRING															{ $$ = StringExpressionSemanticAction($1); }
	| BOOLEAN															{ $$ = BooleanExpressionSemanticAction($1); }
	| IDENT																{ $$ = IdentifierExpressionSemanticAction($1); }
	| EXTERNAL_PARAM													{ $$ = ExternalParameterExpressionSemanticAction($1); }
	| IDENT OPEN_PARENTHESIS argList CLOSE_PARENTHESIS					{ $$ = FunctionCallExpressionSemanticAction($1, $3); }
	| IDENT OPEN_PARENTHESIS CLOSE_PARENTHESIS							{ $$ = FunctionCallExpressionSemanticAction($1, NULL); }
	| OPEN_PARENTHESIS expr CLOSE_PARENTHESIS							{ $$ = $2; }
	;

%%
