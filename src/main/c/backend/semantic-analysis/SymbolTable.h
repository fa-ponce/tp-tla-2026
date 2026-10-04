#include <Token.h>
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

// Inicializa el stack de scopes
bool initScopes(void);

// Crea un nuevo scope anidado
bool enterScope(void);

// Busca el simbolo en todos los scopes recursivamente
// Empezando por el actual y subiendo
int findSymbol(Token symbol);

// Aniade un simbolo al scope actual
bool addSymbol(Token symbol);

// Chequea si el simbolo existe en el scope actual
bool checkSymbol(Token symbol);

// Elimina el scope actual
void exitScope(void);

void destroyScopes(void);

#endif