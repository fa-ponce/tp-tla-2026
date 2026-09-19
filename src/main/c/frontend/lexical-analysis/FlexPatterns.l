%top{

#include "../../support/type/CompilationStatus.h"
#include "../../support/type/FlexContext.h"
#include "../../support/type/TokenLabel.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"

}

%{

/**
 * @see https://westes.github.io/flex/manual/Definitions-Section.html#Definitions-Section
 */

extern CompilationStatus LoadLexemeAction();
extern CompilationStatus SaveLexemeAction();
extern CompilationStatus FilterLexemeAction();
extern CompilationStatus MutateLexemeAction();
extern CompilationStatus SelectLexemeAction();
extern CompilationStatus SortLexemeAction();
extern CompilationStatus ByLexemeAction();
extern CompilationStatus LimitLexemeAction();
extern CompilationStatus AscLexemeAction();
extern CompilationStatus DescLexemeAction();
extern CompilationStatus IfLexemeAction();
extern CompilationStatus ThenLexemeAction();
extern CompilationStatus ElseLexemeAction();
extern CompilationStatus ApplyLexemeAction();
extern CompilationStatus AndLexemeAction();
extern CompilationStatus OrLexemeAction();
extern CompilationStatus NotLexemeAction();
extern CompilationStatus LetLexemeAction();
extern CompilationStatus FunctionLexemeAction();
extern CompilationStatus ReturnLexemeAction();
extern CompilationStatus PipelineLexemeAction();
extern CompilationStatus RequireLexemeAction();
extern CompilationStatus ForLexemeAction();
extern CompilationStatus InLexemeAction();
extern CompilationStatus CurlyBracketLexemeAction(TokenLabel label);
extern CompilationStatus ParenthesisLexemeAction(TokenLabel label);
extern CompilationStatus BracketLexemeAction(TokenLabel label);
extern CompilationStatus CommaLexemeAction();
extern CompilationStatus SemiColonLexemeAction();
extern CompilationStatus EqualLexemeAction();
extern CompilationStatus NotEqualLexemeAction();
extern CompilationStatus ArithmeticOperatorLexemeAction(TokenLabel label);
extern CompilationStatus ComparisonOperator(TokenLabel label);
extern CompilationStatus StringLexemeAction();
extern CompilationStatus NumberLexemeAction();
extern CompilationStatus IdentLexemeAction();
extern CompilationStatus IgnoredLexemeAction();
extern CompilationStatus UnknownLexemeAction();
extern CompilationStatus EOFLexemeAction();

%}

/**
 * Flex contexts (a.k.a. start conditions).
 *
 * @see https://westes.github.io/flex/manual/Start-Conditions.html
 */


/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */
WHITESPACE			[[:space:]]+
IDENT               [[:alpha:]_][[:alnum:]_]*
digit               [[:digit:]]          
STRING              \"([^\"\\]|\\.)*\"
NUMBER              {digit}+(\.{digit}+)?

%%

%{

/**
 * @see https://westes.github.io/flex/manual/Rules-Section.html#Rules-Section
 */

%}

"//"[^\n]*      { return IgnoredLexemeAction(); }

"LOAD"          { return LoadLexemeAction(); }
"SAVE"          { return SaveLexemeAction(); }
"FILTER"        { return FilterLexemeAction(); }
"MUTATE"        { return MutateLexemeAction(); }
"SELECT"        { return SelectLexemeAction(); }
"SORT"          { return SortLexemeAction(); }
"BY"            { return ByLexemeAction(); }
"LIMIT"         { return LimitLexemeAction(); }
"ASC"           { return AscLexemeAction(); }
"DESC"          { return DescLexemeAction(); }
"IF"            { return IfLexemeAction(); }
"THEN"          { return ThenLexemeAction(); }
"ELSE"          { return ElseLexemeAction(); }
"APPLY"         { return ApplyLexemeAction(); }
"AND"           { return AndLexemeAction(); }
"OR"            { return OrLexemeAction(); }
"NOT"           { return NotLexemeAction(); }

"let"           { return LetLexemeAction(); }
"function"      { return FunctionLexemeAction(); }
"return"        { return ReturnLexemeAction(); }
"pipeline"      { return PipelineLexemeAction(); }
"require"       { return RequireLexemeAction(); }
"for"           { return ForLexemeAction(); }
"in"            { return InLexemeAction(); }

"{"             { return CurlyBracketLexemeAction(OPEN_CURLY_BRACKET); }
"}"             { return CurlyBracketLexemeAction(CLOSE_CURLY_BRACKET); }
"("             { return ParenthesisLexemeAction(OPEN_PARENTHESIS); }
")"             { return ParenthesisLexemeAction(CLOSE_PARENTHESIS); }
"["             { return BracketLexemeAction(OPEN_BRACKET); }
"]"             { return BracketLexemeAction(CLOSE_BRACKET); }

","             { return CommaLexemeAction(); }
";"             { return SemiColonLexemeAction(); }

"="             { return EqualLexemeAction(); }
"!="            { return NotEqualLexemeAction(); }
"-"	            { return ArithmeticOperatorLexemeAction(SUB); }
"*"	            { return ArithmeticOperatorLexemeAction(MUL); }
"/"	            { return ArithmeticOperatorLexemeAction(DIV); }
"+"             { return ArithmeticOperatorLexemeAction(ADD); }

"<"             { return ComparisonOperator(LOWER_THAN_SIGN); }
">"             { return ComparisonOperator(HIGHER_THAN_SIGN); }
"<="            { return ComparisonOperator(LOWER_OR_EQUAL_THAN_SIGN); }
">="            { return ComparisonOperator(HIGHER_OR_EQUAL_THAN_SIGN); }

{STRING}        { return StringLexemeAction(); }
{NUMBER}        { return NumberLexemeAction(); }
{IDENT}         { return IdentLexemeAction();  }
{WHITESPACE}    { return IgnoredLexemeAction(); }

.               { return UnknownLexemeAction(); }

<<EOF>>		    { return EOFLexemeAction(); }


%%

/**
 * @see https://westes.github.io/flex/manual/User-Code-Section.html#User-Code-Section
 */

#include "FlexActions.h"
#include "FlexExport.h"
