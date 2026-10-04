#include "SymbolTable.h"
#include "../../support/hash-map/HashMap.h"
#include "../../support/stack/Stack.h"

static stack* scopeStack = NULL;

// Habria q inicializarlo en el entryPoint, antes de arrancar
// A llenar la tabla de simbolos
bool initScopes(void){
    scopeStack = create_stack();
    return scopeStack != NULL;
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


bool addSymbol(char* name, Symbol* symbol){
    ht* scopeTable = peek(scopeStack);
    if(scopeTable == NULL){ // No hay ningun scope abierto
        return false;
    }
    // Nos fijamos si el simbolo ya esta declarado en el scope
    if(checkSymbol(name)){
        return false;
    }

    // Como no esta en el scope, lo podemos agregar tranquilos
    return set_value(scopeTable, name, symbol);
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
        ht_foreach(scopeTable, free);
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
