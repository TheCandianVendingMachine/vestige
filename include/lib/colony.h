#ifndef VESTIGE_COLONY_H
#define VESTIGE_COLONY_H

//#define COLONY_TEST_NO_CPY

#include <stdint.h>
#include <stddef.h>

#define COLONY_BITSET_TYPE uint64_t
#define COLONY_BITSET_SIZE (8 * sizeof(COLONY_BITSET_TYPE))
#define COLONY_ITEMS_PER_BUCKET 8
#define COLONY_MAX_ITEMS_PER_BUCKET 1024

#define _PCOLONY_GET_BUCKET(colony,i)\
    ((struct ColonyBucket*)((uint8_t*)(colony)->buckets + (colony)->_bucket_offsets[(i)]))
#define _COLONY_GET_BUCKET(colony,i) _PCOLONY_GET_BUCKET(&colony,i)

#define COLONY_ITER_BEGIN(T,colony){\
size_t _bucket_index = 0;\
for (size_t _i = 0; _i < colony.length;) {\
    struct ColonyBucket* bucket = _COLONY_GET_BUCKET(colony,_bucket_index);\
    size_t _bitset = 0;\
    COLONY_BITSET_TYPE _bitset_i = 0;\
    for (int _j = 0; _i < colony.length && _j < bucket->length; _j++) {\
        while ((((&bucket->taken_indices)[_bitset] >> _bitset_i) & 1) == 0) {\
            _bitset_i += 1;\
            if (_bitset_i >= COLONY_BITSET_SIZE) {\
                _bitset_i = 0;\
                _bitset += 1;\
            }\
        }\
        size_t _item_index = _bitset * COLONY_BITSET_SIZE + _bitset_i;\
        _bitset_i += 1;\
        T* i = (T*)((uint8_t*)bucket->items + colony._item_size * _item_index)
#define COLONY_ITER_END\
        _i += 1;\
    }\
    _bucket_index += 1;\
}}

struct ColonyBucket {
    void* items;
    size_t length;
    size_t size;
    size_t bitset_count;
    COLONY_BITSET_TYPE taken_indices;
};

typedef struct Colony {
    struct ColonyBucket* buckets;
    size_t* _bucket_offsets;

    COLONY_BITSET_TYPE* _full_buckets;
    size_t _full_bucket_length;

    size_t _bucket_length;

    size_t length;
    size_t _item_size;
} Colony;

Colony new_colony(uint32_t itemsize);
void del_colony(Colony colony);
size_t colony_insert(Colony* colony, void* item);
void* colony_remove(Colony* colony, size_t index);
void* colony_get(Colony colony, size_t index);

#endif
