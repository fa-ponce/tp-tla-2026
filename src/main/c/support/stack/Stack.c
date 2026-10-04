#include "Stack.h"

stack* create_stack(void){
    stack* stk = calloc(1, sizeof(stack));
    if(stk == NULL){
        return NULL;
    }
    return stk;
}

void free_stack(stack* stk){
    if(stk == NULL) {
        return;
    }
    
    stk_item* item = stk->stk;
    
    while(item != NULL){
        stk_item* next = item->next;
        free(item);
        item = next;
    }

    free(stk);
}

bool push(stack* stk, void* element){
    stk_item* item = malloc(sizeof(stk_item));
    if(item == NULL){
        return false;
    }
    item->element = element;
    item->next = stk->stk;
    stk->stk = item;
    stk->size++;
    return true;
}

void* pop(stack* stk){
    if(stk->stk == NULL){
        return NULL;
    }
    stk_item* top = stk->stk;
    void* element = top->element;
    stk->stk = top->next;
    free(top);
    stk->size--;
    return element;
}

bool is_empty(stack* stk){
   return stk->size == 0;
}

