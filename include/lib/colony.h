#ifndef VESTIGE_COLONY_H
#define VESTIGE_COLONY_H

#include <stdint.h>
#include <stddef.h>

#define COLONY_ITEMS_PER_BUCKET 256
#define COLONY_BITSET_TYPE uint64_t
#define COLONY_BITSET_SIZE (8 * sizeof(COLONY_BITSET_TYPE))
#define COLONY_BITSET_LENGTH COLONY_ITEMS_PER_BUCKET / COLONY_BITSET_SIZE 

#define COLONY_ITER_BEGIN(T,colony)\
for (int _i = 0; _i < colony.length;) {\
    size_t _bucket_index = _i / COLONY_ITEMS_PER_BUCKET;\
    struct ColonyBucket* bucket = colony.buckets[_bucket_index];\
    size_t _bitset = 0;\
    size_t _bitset_i = 0;\
    for (int _j = 0; _j < bucket->length; _j++) {\
        while ((bucket->taken_indices[_bitset] & (1 << _bitset_i)) == 0) {\
            _bitset_i += 1;\
            if (_bitset_i >= COLONY_BITSET_SIZE) {\
                _bitset_i = 0;\
                _bitset += 1;\
            }\
        }\
        size_t _item_index = _bitset * COLONY_BITSET_SIZE + _bitset_i;\
        _bitset_i += 1;\
        _i += 1;\
        T* i = (T*)((uint8_t*)&bucket->items + colony._item_size * _item_index);
#define COLONY_ITER_END\
    }\
}

struct ColonyBucket {
    uint64_t taken_indices[COLONY_BITSET_LENGTH];
    uint64_t length;
    uint8_t has_free_spots;
    /*
     * We want to allocate the items inline, so we need to add padding to align on a
     * 8 byte boundary
     * Size of above items: 9 bytes
     */
    uint8_t __padding[7];
    uint8_t items;
};

typedef struct Colony {
    struct ColonyBucket** buckets;
    size_t length;
    size_t _length;
    size_t _size;
    size_t _item_size;
} Colony;

Colony new_colony(uint32_t itemsize);
void del_colony(Colony colony);
size_t colony_insert(Colony* colony, void* item);
void* colony_remove(Colony* colony, size_t index);

#endif
