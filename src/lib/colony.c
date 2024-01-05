#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lib/colony.h"

struct ColonyBucket* create_bucket(uint32_t itemsize) {
    const size_t bytes = (sizeof(struct ColonyBucket) - sizeof(uint8_t)) + itemsize * COLONY_ITEMS_PER_BUCKET;
    struct ColonyBucket* bucket = malloc(bytes);
    memset(bucket, 0, bytes);
    bucket->has_free_spots = true;

    return bucket;
}

void insert_into_bucket(struct ColonyBucket* bucket, uint32_t itemsize, void* item) {
    assert(bucket->has_free_spots);
    size_t index = 0;
    int bitset;
    int i;
    // find free index
    for (bitset = 0; bitset < COLONY_BITSET_LENGTH; bitset++) {
        for (i = 0; i < COLONY_BITSET_SIZE; i++) {
            if ((bucket->taken_indices[bitset] & (1 << i)) == 0) {
                goto found_index;
            }
            index++;
        }
    }
    // insert item into index
    found_index: {
        uint8_t* item_ptr = &bucket->items;
        item_ptr += itemsize * index;
        memcpy(item_ptr, item, itemsize);
    }
    bucket->taken_indices[bitset] |= (1 << i);
    bucket->length += 1;

    // check if all indices taken
    bucket->has_free_spots = bucket->length != COLONY_ITEMS_PER_BUCKET;
}

void* remove_from_bucket(struct ColonyBucket* bucket, uint32_t itemsize, size_t index) {
    size_t bitset = index / COLONY_BITSET_SIZE - 1;
    size_t i = index - (bitset + 1) * COLONY_BITSET_SIZE;
    bucket->taken_indices[bitset] ^= (1 << i);

    bucket->has_free_spots = true;
    bucket->length -= 1;

    uint8_t* item_ptr = &bucket->items;
    return item_ptr + index * itemsize;
}

Colony new_colony(uint32_t itemsize) {
    Colony colony;
    colony.buckets = NULL;
    colony.length = 0;
    colony._length = 0;
    colony._size = 0;
    colony._item_size = itemsize;
    return colony;
}

void del_colony(Colony colony) {
    for (int i = 0; i < colony._size; i++) {
        free(colony.buckets[i]);
    }
    free(colony.buckets);
}

size_t colony_insert(Colony* colony, void* item) {
    if (colony->buckets == NULL) {
        colony->_size = 1;
        colony->_length = 1;
        colony->buckets = malloc(sizeof(struct ColonyBucket**));
        colony->buckets[0] = create_bucket(colony->_item_size);
        insert_into_bucket(colony->buckets[0], colony->_item_size, item);

        colony->length += 1;
        return colony->length;
    }

    for (int i = 0; i < colony->_length; i++) {
        if (colony->buckets[i]->has_free_spots) {
            insert_into_bucket(colony->buckets[i], colony->_item_size, item);
            colony->length += 1;
            return colony->length;
        }
    }

    colony->_size *= 2;
    colony->buckets = realloc(colony->buckets, sizeof(struct ColonyBucket**) * colony->_size);

    insert_into_bucket(colony->buckets[colony->_length], colony->_item_size, item);
    colony->_length += 1;

    colony->length += 1;
    return colony->length;
}

void* colony_remove(Colony* colony, size_t index) {
    size_t bucket_index = index / COLONY_ITEMS_PER_BUCKET;
    index -= bucket_index;
    return remove_from_bucket(colony->buckets[bucket_index], colony->_item_size, index);
}
