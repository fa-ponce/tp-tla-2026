#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/type/CompilationStatus.h"
#include "../../support/type/ModuleDestructor.h"

ModuleDestructor initializeSemanticAnalyzerModule();

CompilationStatus executeSemanticAnalysis(Program* program);


#endif
