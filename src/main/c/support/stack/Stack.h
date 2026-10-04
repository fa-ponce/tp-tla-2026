#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#ifndef STACK_H
#define STACK_H

typedef struct stk_item{
    void* element;
    struct stk_item* next;
}stk_item;

typedef struct stack{
    size_t size;    //Cantidad de elementos
    stk_item* stk;
}stack;

stack* create_stack(void);

void free_stack(stack* stk);

bool push(stack* stk, void* element);

void* pop(stack* stk);

void* peek(stack* stk);

bool is_empty(stack* stk);

#endif