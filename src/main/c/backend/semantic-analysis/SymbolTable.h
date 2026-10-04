#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"

typedef enum {
    LET_SYMBOL,
    FUNCTION_SYMBOL,
    PIPELINE_SYMBOL,
    PARAMETER_SYMBOL,
    FOR_VARIABLE_SYMBOL
} SymbolKind;

  typedef struct {
    SymbolKind kind;
    union {
        LetDeclaration* let;
        FunctionDeclaration* function;
        PipelineDeclaration* pipeline;
    };
} Symbol;


// Inicializa el stack de scopes
bool initScopes(void);

// Crea un nuevo scope anidado
bool enterScope(void);

// Busca el simbolo en todos los scopes recursivamente
// Empezando por el actual y subiendo
// Devuelve el depth
int findSymbol(char* name);

// Devuelve el simbolo
Symbol* lookupSymbol(char* name);

// Aniade un simbolo al scope actual
bool addSymbol(char* name, Symbol* symbol);

// Chequea si el simbolo existe en el scope actual
bool checkSymbol(char* name);

// Elimina el scope actual
void exitScope(void);

void destroyScopes(void);

#endif