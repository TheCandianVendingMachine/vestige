#define BUCKET_GROWTH_FACTOR 2

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lib/colony.h"
#include "engine_lib.h"

#define ADJUSTED_BUCKET_SIZE(count)\
    sizeof(struct ColonyBucket) +\
    sizeof(COLONY_BITSET_TYPE) * (get_bitset_count(count) - 1)

size_t get_bitset_count(uint64_t count) {
    size_t bitset_count = count / COLONY_BITSET_SIZE;
    if (count % COLONY_BITSET_SIZE == 0) {
        return bitset_count;
    }
    return 1 + bitset_count;
}

void create_bucket(struct ColonyBucket* bucket, uint32_t itemsize, uint64_t count) {
    bucket->bitset_count = get_bitset_count(count);
    memset(&bucket->taken_indices, 0, bucket->bitset_count * sizeof(COLONY_BITSET_TYPE));
    bucket->size = count;
    bucket->length = 0;

#ifndef COLONY_TEST_NO_CPY
    bucket->items = malloc(itemsize * count);
    memset(bucket->items, 0, itemsize * count);
#endif
}

void insert_into_bucket(struct ColonyBucket* bucket, uint32_t itemsize, void* item) {
    assert(bucket->length != bucket->size);
    // find free index
    for (size_t bitset = 0; bitset < bucket->bitset_count; bitset++) {
        COLONY_BITSET_TYPE bits = (&bucket->taken_indices)[bitset];
        if (bits != ~((COLONY_BITSET_TYPE)0)) {
            int index = __builtin_ffsll(~bits) - 1;
#ifndef COLONY_TEST_NO_CPY
            uint8_t* item_ptr = bucket->items;
            item_ptr += itemsize * (bitset * COLONY_BITSET_SIZE + index);
            memcpy(item_ptr, item, itemsize);
#endif
            (&bucket->taken_indices)[bitset] |= ((COLONY_BITSET_TYPE)1 << index);
            bucket->length += 1;
            return;
        }
    }
    // should never reach here
    engine_crash(SHUTDOWN_LIBRARY_ERROR);
}

void* remove_from_bucket(struct ColonyBucket* bucket, uint32_t itemsize, size_t index) {
    size_t bitset = index / COLONY_BITSET_SIZE;
    size_t i = index - bitset * COLONY_BITSET_SIZE;
    (&bucket->taken_indices)[bitset] ^= ((COLONY_BITSET_TYPE)1 << i);

    bucket->length -= 1;

#ifndef COLONY_TEST_NO_CPY
    uint8_t* item_ptr = bucket->items;
    return item_ptr + index * itemsize;
#else
    return NULL;
#endif
}

void* get_from_bucket(struct ColonyBucket bucket, uint32_t itemsize, size_t index) {
#ifndef COLONY_TEST_NO_CPY
    uint8_t* item_ptr = bucket.items;
    return item_ptr + index * itemsize;
#else
    return NULL;
#endif
}

Colony new_colony(uint32_t itemsize) {
    Colony colony;
    colony.buckets = NULL;
    colony._bucket_offsets = NULL;
    colony._full_buckets = NULL;
    colony._full_bucket_length = 0;
    colony.length = 0;
    colony._bucket_length = 0;
    colony._item_size = itemsize;
    return colony;
}

void del_colony(Colony colony) {
    for (int i = 0; i < colony._bucket_length; i++) {
#ifndef COLONY_TEST_NO_CPY
        free(_COLONY_GET_BUCKET(colony,i)->items);
#endif
    }
    free(colony.buckets);
    free(colony._bucket_offsets);
    free(colony._full_buckets);
}

size_t colony_insert(Colony* colony, void* item) {
    if (colony->buckets == NULL) {
        colony->_bucket_length = 1;
        colony->buckets = malloc(ADJUSTED_BUCKET_SIZE(COLONY_ITEMS_PER_BUCKET));
        colony->_full_buckets = malloc(sizeof(COLONY_BITSET_TYPE));
        colony->_full_buckets[0] = 0;
        colony->_full_bucket_length = 1;
        colony->_bucket_offsets = malloc(sizeof(size_t) * colony->_bucket_length);
        colony->_bucket_offsets[0] = 0;
        create_bucket(&colony->buckets[0], colony->_item_size, COLONY_ITEMS_PER_BUCKET);
        insert_into_bucket(&colony->buckets[0], colony->_item_size, item);

        colony->length += 1;
        return colony->length - 1;
    }

    for (int i = 0; i < colony->_full_bucket_length; i++) {
        COLONY_BITSET_TYPE bits = colony->_full_buckets[i];
        if (bits != ~((COLONY_BITSET_TYPE)0)) {
            int index = __builtin_ffsll(~bits) - 1;
            int adjusted_index = i * COLONY_BITSET_SIZE + index;
            // If we ran out of created buckets, then allocate a new one
            if (adjusted_index >= colony->_bucket_length) {
                break;
            }

            // Otherwise insert into bucket
            struct ColonyBucket* bucket = _PCOLONY_GET_BUCKET(colony,adjusted_index);
            assert(bucket->length < bucket->size);

            insert_into_bucket(bucket, colony->_item_size, item);
            colony->length += 1;

            if (bucket->length == bucket->size) {
                colony->_full_buckets[i] |= ((COLONY_BITSET_TYPE)1 << index);
            }

            return colony->length - 1;
        }
    }

    size_t colony_byte_count = 0;
    size_t last_bucket_item_count = 0;
    for (int i = 0; i < colony->_bucket_length; i++) {
        struct ColonyBucket* bucket = _PCOLONY_GET_BUCKET(colony,i);
        last_bucket_item_count = bucket->size;
        colony_byte_count += ADJUSTED_BUCKET_SIZE(last_bucket_item_count);
    }

    colony->_bucket_length += 1;
    if (colony->_bucket_length >= COLONY_BITSET_SIZE * colony->_full_bucket_length) {
        colony->_full_bucket_length += 1;
        colony->_full_buckets = realloc(colony->_full_buckets, colony->_full_bucket_length * sizeof(COLONY_BITSET_TYPE));
        colony->_full_buckets[colony->_full_bucket_length - 1] = 0;
    }

    colony->_bucket_offsets = realloc(colony->_bucket_offsets, sizeof(size_t) * colony->_bucket_length);
    colony->_bucket_offsets[colony->_bucket_length - 1] = colony_byte_count;

    uint64_t new_bucket_item_size = last_bucket_item_count * BUCKET_GROWTH_FACTOR;
    if (new_bucket_item_size > COLONY_MAX_ITEMS_PER_BUCKET) {
        new_bucket_item_size = COLONY_MAX_ITEMS_PER_BUCKET;
    }
    colony_byte_count += ADJUSTED_BUCKET_SIZE(new_bucket_item_size);

    colony->buckets = realloc(colony->buckets, colony_byte_count);
    struct ColonyBucket* new_bucket = _PCOLONY_GET_BUCKET(colony,colony->_bucket_length - 1);

    create_bucket(new_bucket, colony->_item_size, new_bucket_item_size);
    insert_into_bucket(new_bucket, colony->_item_size, item);

    colony->length += 1;
    return colony->length - 1;
}

void* colony_remove(Colony* colony, size_t index) {
    for (size_t i = 0; i < colony->_bucket_length; i++) {
        struct ColonyBucket* bucket = _PCOLONY_GET_BUCKET(colony,i);
        if (index < bucket->size) {
            size_t bitset = i / COLONY_BITSET_SIZE;
            COLONY_BITSET_TYPE bits = ~((COLONY_BITSET_TYPE)1 << (i - bitset * COLONY_BITSET_SIZE));

            colony->_full_buckets[bitset] &= bits;

            colony->length -= 1;

            return remove_from_bucket(bucket, colony->_item_size, index);
        }
        index -= bucket->size;
    }
    return NULL;
}

void* colony_get(Colony colony, size_t index) {
    for (size_t i = 0; i < colony._bucket_length; i++) {
        struct ColonyBucket* bucket = _COLONY_GET_BUCKET(colony,i);
        if (index < bucket->size) {
            return get_from_bucket(*bucket, colony._item_size, index);
        }
        index -= bucket->size;
    }
    return NULL;
}
