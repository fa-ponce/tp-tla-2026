#include <Token.h>
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

// Crea un nuevo scope anidado
void enterScope();

// Busca el simbolo en todos los scopes recursivamente
// Empezando por el actual y subiendo
int findSymbol(Token symbol);

// Aniade un simbolo al scope actual
void addSymbol(Token symbol);

// Chequea si el simbolo existe en el scope actual
int checkSymbol(Token symbol);

// Elimina el scope actual
void exitScope();

#endif