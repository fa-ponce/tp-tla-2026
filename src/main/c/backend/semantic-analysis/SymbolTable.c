#include "SymbolTable.h"
#include "../../support/hash-map/HashMap.h"
#include "../../support/stack/Stack.h"

static stack* scopeStack = NULL;
static stack* allSymbols = NULL;

// Habria q inicializarlo en el entryPoint, antes de arrancar
// A llenar la tabla de simbolos
bool initScopes(void){
    scopeStack = create_stack();
    allSymbols = create_stack();
    return scopeStack != NULL && allSymbols != NULL;
}

bool enterScope(void){
    ht* scopeTable = create_ht();
    if(scopeTable == NULL){
        return false;
    }

    // Pusheo una nueva tabla de simbolos al scope
    bool status = push(scopeStack, scopeTable);

    if(status){ // true, se pudeo pushear
        return true;
    } else { // false, fallo algo
        destroy_ht(scopeTable);
        return false;
    }
}


int findSymbol(char* name){
    if(scopeStack == NULL) return -1;

    int depth = 0;
    for(stk_item* item = scopeStack->stk; item != NULL; item = item->next){
        ht* scopeTable = item->element;
        if(get_value(scopeTable, name) != NULL){
            return depth;
        }
        depth++;
    }
    return -1; // No lo encontro
}

Symbol* lookupSymbol(char* name){
    if(scopeStack == NULL){
        return NULL;
    }
    for(stk_item* item = scopeStack->stk; item != NULL; item = item->next){
        Symbol* symbol = get_value(item->element, name);
        if(symbol != NULL){
            return symbol;
        }
    }
    return NULL;
}


Symbol* addSymbol(char* name, Symbol symbol){
    ht* scopeTable = peek(scopeStack);
    if(scopeTable == NULL || checkSymbol(name)){
        return NULL;
    }
    Symbol* newSymbol = malloc(sizeof(Symbol));
    if(newSymbol == NULL){
        return NULL;
    }
    *newSymbol = symbol;
    if(!push(allSymbols, newSymbol)){
        free(newSymbol);
        return NULL;
    }
    // Si falla el set, el simbolo ya esta en allSymbols y se libera al final
    return set_value(scopeTable, name, newSymbol) ? newSymbol : NULL;
}

bool checkSymbol(char* name){
    ht* scopeTable = peek(scopeStack);
    if(scopeTable == NULL){ // No hay ningun scope abierto
        return false;
    }

    return get_value(scopeTable, name) != NULL;
}

void exitScope(void){
    ht* scopeTable = pop(scopeStack);
    if(scopeTable != NULL){
        destroy_ht(scopeTable);
    }
}

void destroyScopes(void){
      if(scopeStack == NULL){
          return;
      }
      while(!is_empty(scopeStack)){
          exitScope();
      }
      free_stack(scopeStack);
      scopeStack = NULL;
  }


void destroySymbols(void){
    if(allSymbols == NULL){
        return;
    }
    while(!is_empty(allSymbols)){
        free(pop(allSymbols));
    }
    free_stack(allSymbols);
    allSymbols = NULL;
}
