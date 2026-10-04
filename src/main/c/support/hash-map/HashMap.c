#include "HashMap.h"

// Return 64-bit FNV-1a hash for key (NUL-terminated). See description:
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
static uint64_t hash_key(const char* key) {
    uint64_t hash = FNV_OFFSET;
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

// Duplica la capacidad y reubica cada nodo según su nueva posición
static bool resize(ht* table){
    size_t new_size = table->size * 2;
    item* new_items = calloc(new_size, sizeof(item));
    if (new_items == NULL){
        return false;
    }

    for (size_t i = 0; i < table->size; i++){
        item* entry = table->items[i].next;
        while (entry != NULL){
            item* next = entry->next;
            size_t position = hash_key(entry->key) % new_size;
            entry->next = new_items[position].next;
            new_items[position].next = entry;
            entry = next;
        }
    }

    free(table->items);
    table->items = new_items;
    table->size = new_size;
    return true;
}

static item* create_item(char* key, void* value){
    item* it = calloc(1, sizeof(item));
    if(it== NULL){
        return NULL;
    }

    it->key = malloc(strlen(key) + 1);
    if(it->key == NULL){
        free(it);
        return NULL;
    }

    it->value = malloc(strlen(value) + 1);
    if(it->value == NULL){
        free(it->key);
        free(it);
        return NULL;
    }

    strcpy(it->key, key);
    strcpy(it->value, value);
    return it;
}

ht* create_ht(void){
    ht* table = malloc(sizeof(ht));
    if(table == NULL){
        return NULL;
    }

    table->count = 0;
    table->size = INITIAL_CAPACITY;

    table->items = calloc(INITIAL_CAPACITY, sizeof(item));
    if(table->items == NULL){
        free(table);
        return NULL;
    }

    return table;
}

void destroy_ht(ht* table){
for(size_t i = 0; i < table->size; i++){
        item* entry = table->items[i].next;
        while(entry != NULL){
            item* next = entry->next;
            free(entry->key);
            free(entry->value);   // depende de la decision de la seccion 3
            free(entry);
            entry = next;
        }
    }
    free(table->items);
    free(table);
}

void* get_value(ht* table, void* key){
    // Busco el indice a que quiero
    size_t index = hash_key(key) % table->size;
    
    item* it = &table->items[index];
    while(it->next != NULL){
        if(strcmp(it->next->key, key) == 0){
            return it->next->value;
        }
        it = it->next;
    }
    return NULL; //No lo encontramos
}

bool set_value(ht* table, char* key, void* value){
    if(table->count + 1 > table->size){
        resize(table);
    }

    size_t index = hash_key(key) % table->size;
   
    item* it = &table->items[index];
    while(it->next != NULL){
        // Nos fijamos si ya existe la key
        if(strcmp(it->next->key, key) == 0){
            char* new_value = malloc(strlen(value) + 1);
            if(new_value == NULL){
                return false;
            }
            strcpy(new_value, value);
            free(it->next->value);
            it->next->value = new_value;
            return true;
        }
        it = it->next;
    }

    // La key no existia, por lo que insertamos el par al final de la lista
    item* new_item = create_item(key, value);
    if(new_item == NULL){
        return false;
    }
    it->next = new_item;
    table->count++;
    return true;
}

size_t get_count(ht* table){
    return table->count;
}