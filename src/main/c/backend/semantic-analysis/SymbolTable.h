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

typedef enum {
    UNKNOWN_TYPE,
    NUMBER_TYPE,
    STRING_TYPE,
    BOOLEAN_TYPE
} ValueType;

typedef struct Symbol {
    SymbolKind kind;
    ValueType type;
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

// Aniade un simbolo al scope actual y devuelve el puntero (NULL si falla)
// La tabla es duenia del simbolo: sobrevive a exitScope y se libera en destroySymbols
Symbol* addSymbol(char* name, Symbol symbol);

// Chequea si el simbolo existe en el scope actual
bool checkSymbol(char* name);

// Elimina el scope actual
void exitScope(void);

void destroyScopes(void);

// Libera todos los simbolos creados, llamar cuando nadie mas los use
void destroySymbols(void);

#endif