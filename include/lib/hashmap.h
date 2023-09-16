#ifndef _VESTIGE_LIB_HASHMAP_H
#define _VESTIGE_LIB_HASHMAP_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <lib/string.h>

#define _DEFAULT_HASHMAP_SIZE 16

#define HASHMAP(t) \
    GHASHMAP(t, stringhash)

// Generic hashmap
#define GHASHMAP(t, h) \
    new_hashmap(sizeof(t), _DEFAULT_HASHMAP_SIZE, (h))

typedef struct HashMap {
    uint64_t (*_hash)(const void* item);

    uint32_t length;
    uint32_t _size;
    uint32_t _itemsize;

    struct {
        uint64_t hash;
        uint8_t state;  // state of the bucket
        uint8_t item[];  // for variable-sized items
    }* _buckets;
} HashMap;

HashMap new_hashmap(size_t itemsize, size_t size, uint64_t (*hash)(const void* key));
void del_hashmap(HashMap m);
void hashmap_grow(HashMap* m);

void hashmap_set(HashMap* m, const void* key, const void* item);

const void* hashmap_get(HashMap* m, const void* key);
const void* hashmap_delete(HashMap* m, const void* key);

uint64_t gethash(const void* item, size_t size);
uint64_t stringhash(const void* s);
uint64_t inthash(const void* i);
uint64_t floathash(const void* f);

#endif
