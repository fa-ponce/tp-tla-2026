#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#ifndef HASH_MAP_H
#define HASH_MAP_H

#define INITIAL_CAPACITY 32

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

typedef struct item{
    char* key;
    void* value;
    struct item* next; //Esta es la lista de colisiones
}item;

typedef struct ht{
    item* items;
    size_t count;
    size_t size;
}ht;

ht* create_ht(void);

void destroy_ht(ht* table);

void* get_value(ht* table, void* key);

bool set_value(ht* table, char* key, void* value);

size_t get_count(ht* table);

// Iterador de hashMap
void ht_foreach(ht* table, void (*fn)(void*));

#endif