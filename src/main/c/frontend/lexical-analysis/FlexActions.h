#ifndef FLEX_ACTIONS_HEADER
#define FLEX_ACTIONS_HEADER

#include "../../support/configuration/Environment.h"
#include "../../support/language/String.h"
#include "../../support/logging/Logger.h"
#include "../../support/type/CompilationStatus.h"
#include "../../support/type/FlexContext.h"
#include "../../support/type/LexicalAnalyzer.h"
#include "../../support/type/ModuleDestructor.h"
#include "../../support/type/Token.h"
#include "../../support/type/TokenLabel.h"
#include "../Frontend.h"

/** Initialize module's internal state. */
ModuleDestructor initializeFlexActionsModule();

CompilationStatus LoadLexemeAction();
CompilationStatus SaveLexemeAction();
CompilationStatus FilterLexemeAction();
CompilationStatus MutateLexemeAction();
CompilationStatus SelectLexemeAction();
CompilationStatus SortLexemeAction();
CompilationStatus ByLexemeAction();
CompilationStatus LimitLexemeAction();
CompilationStatus AscLexemeAction();
CompilationStatus DescLexemeAction();
CompilationStatus IfLexemeAction();
CompilationStatus ThenLexemeAction();
CompilationStatus ElseLexemeAction();
CompilationStatus ApplyLexemeAction();
CompilationStatus AndLexemeAction();
CompilationStatus OrLexemeAction();
CompilationStatus NotLexemeAction();
CompilationStatus LetLexemeAction();
CompilationStatus FunctionLexemeAction();
CompilationStatus ReturnLexemeAction();
CompilationStatus PipelineLexemeAction();
CompilationStatus RequireLexemeAction();
CompilationStatus ForLexemeAction();
CompilationStatus InLexemeAction();
CompilationStatus CurlyBracketLexemeAction(TokenLabel label);
CompilationStatus ParenthesisLexemeAction(TokenLabel label);
CompilationStatus BracketLexemeAction(TokenLabel label);
CompilationStatus CommaLexemeAction();
CompilationStatus SemiColonLexemeAction();
CompilationStatus EqualLexemeAction();
CompilationStatus NotEqualLexemeAction();
CompilationStatus ArithmeticOperatorLexemeAction(TokenLabel label);
CompilationStatus ComparisonOperator(TokenLabel label);
CompilationStatus StringLexemeAction();
CompilationStatus NumberLexemeAction();
CompilationStatus IdentLexemeAction();
CompilationStatus IgnoredLexemeAction();
CompilationStatus UnknownLexemeAction();
CompilationStatus EOFLexemeAction();



#endif
