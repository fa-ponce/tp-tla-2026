#ifndef TYPE_CHECKER_H
#define TYPE_CHECKER_H

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../support/type/ModuleDestructor.h"

ModuleDestructor initializeTypeCheckerModule();

// Requiere el AST ya resuelto (todos los symbol enlazados). Devuelve false si hubo errores.
bool executeTypeCheck(Program* program);

#endif
