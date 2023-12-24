#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wincompatible-pointer-types"
#pragma clang diagnostic ignored "-Wint-to-pointer-cast"

#include <lib/hashmap.h>
#include "logger.h"

#define GROW_AT 0.60  // percent full to begin growing at
#define BUCKET_ITEM_ALIGNMENT 16

enum BUCKETSTATE {
    UNALLOC = 0xAA,
    ALLOC = 0xCC,
    TOMBSTONE = 0xFF,
} __attribute__ ((packed));  // hopefully only use one byte

struct _Bucket {
    uint64_t hash;
    enum BUCKETSTATE state;  // state of the bucket
    uint8_t __packing[BUCKET_ITEM_ALIGNMENT - ((sizeof(uint64_t) + sizeof(enum BUCKETSTATE)) % BUCKET_ITEM_ALIGNMENT)];  // we assume the alignment is 8 for the item
    uint8_t item;  // for variable-sized items
};

#define BUCKET_BYTE_COUNT (sizeof(struct _Bucket) - sizeof(uint8_t))

static struct _Bucket* get_bucket(const struct _Bucket* buckets, int index, size_t itemsize, size_t alignment_offset) {
    const size_t bucket_byte_size = BUCKET_BYTE_COUNT + itemsize;
    const size_t adjusted_bucket_byte_size = bucket_byte_size + alignment_offset;
    const size_t byte_offset = index * adjusted_bucket_byte_size;
    const uint8_t* bucket_location = ((uint8_t*)buckets) + byte_offset;
    struct _Bucket* bucket = (struct _Bucket*)bucket_location;
    return bucket;
}

void verify_hashmap(HashMap* m) {
#if _VERIFY_HASHMAP_INTEGRITY
    for (int i = 0; i < m->_size; i++) {
        uint8_t state = get_bucket(m->_buckets, i, m->_itemsize, m->_alignment_offset)->state;
        if (state != UNALLOC && state != ALLOC && state != TOMBSTONE) {
            log_error("hashmap integrity check failed: index=%d, state=%02X", i, state);
            exit(1);
        }
    }
#endif
}

HashMap new_hashmap(size_t itemsize, size_t itemalignment, size_t size, uint64_t (*hash)(const void* key)) {
    size_t bucket_byte_size = BUCKET_BYTE_COUNT + itemsize;
    const size_t alignment_offset = _Alignof(struct _Bucket) - (bucket_byte_size % _Alignof(struct _Bucket));
    // Align size to the desired amount
    bucket_byte_size += alignment_offset;

    const size_t byte_count = size * bucket_byte_size;
    struct _Bucket* _buckets = malloc(byte_count);
    memset(_buckets, 0, byte_count);
    if (_buckets == NULL) {
        perror("new_hashmap: ");
        exit(1);
    }
    for (int i = 0; i < size; i++) {
        get_bucket(_buckets, i, itemsize, alignment_offset)->state = UNALLOC;
    }
    HashMap m = {
        .length = 0,
        ._size = size,
        ._hash = hash,
        ._itemsize = itemsize,
        ._alignment_offset = alignment_offset,
        ._buckets = _buckets,
    };
    verify_hashmap(&m);
    return m;
}

void del_hashmap(HashMap m) {
    verify_hashmap(&m);
    free(m._buckets);
}

static struct _Bucket* find_entry(HashMap* m, uint64_t hash) {
    size_t index = hash % m->_size;
    while (1) {
        struct _Bucket* bucket = get_bucket(m->_buckets, index, m->_itemsize, m->_alignment_offset);

        // Only quit if:
        // UNALLOC => not present
        // ALLOC with matching hash => present
        //
        // Continune while TOMBSTONE or non-matching hash
        if (bucket->state == UNALLOC || (bucket->state == ALLOC && bucket->hash == hash)) {
            return bucket;
        }

        index = (index + 1) % m->_size;
    }
    return NULL;
}

static void set_entry(struct _Bucket* bucket, uint64_t hash, enum BUCKETSTATE state, const void* item, size_t itemsize) {
    bucket->hash = hash;
    bucket->state = state;
    memcpy(&bucket->item, item, itemsize);
}

void hashmap_grow(HashMap* m) {
    verify_hashmap(m);
    struct _Bucket* oldbuckets = m->_buckets;

    m->_size *= 2;
    size_t byte_count = m->_size * (BUCKET_BYTE_COUNT + m->_itemsize + m->_alignment_offset);
    m->_buckets = malloc(byte_count);

    // Set everything new to unallocated
    for (size_t i = 0; i < m->_size; i++) {
        get_bucket(m->_buckets, i, m->_itemsize, m->_alignment_offset)->state = UNALLOC;
    }

    // Move all allocated items over
    for (size_t i = 0; i < m->_size / 2; i++) {
        struct _Bucket* oldbucket = get_bucket(oldbuckets, i, m->_itemsize, m->_alignment_offset);
        if (oldbucket->state == ALLOC) {
            struct _Bucket* newbucket = find_entry(m, oldbucket->hash);
            set_entry(newbucket, oldbucket->hash, ALLOC, &oldbucket->item, m->_itemsize);
        } else {
            get_bucket(m->_buckets, i, m->_itemsize, m->_alignment_offset)->state = UNALLOC;
        }
    }


    free(oldbuckets);
    verify_hashmap(m);
}

void hashmap_set(HashMap* m, const void* key, const void* item) {
    verify_hashmap(m);
    if (m->length >= (GROW_AT * m->_size)) {
        hashmap_grow(m);
        verify_hashmap(m);
    }

    uint64_t hash = m->_hash(key);
    struct _Bucket* bucket = find_entry(m, hash);
    set_entry(bucket, hash, ALLOC, item, m->_itemsize);
    m->length++;
    verify_hashmap(m);
}

const void* hashmap_get(HashMap* m, const void* key) {
    verify_hashmap(m);
    uint64_t hash = m->_hash(key);
    struct _Bucket* bucket = find_entry(m, hash);
    if (bucket->state != ALLOC) {
        verify_hashmap(m);
        return NULL;
    }
    verify_hashmap(m);
    return &bucket->item;
}

const void* hashmap_delete(HashMap* m, const void* key) {
    verify_hashmap(m);
    uint64_t hash = m->_hash(key);
    struct _Bucket* bucket = find_entry(m, hash);

    verify_hashmap(m);
    if (bucket->state == ALLOC) {
        verify_hashmap(m);
        bucket->state = TOMBSTONE;  // TODO: optimization, this only needs to be done if next bucket is ALLOC
        m->length--;  // TODO: we could fill up the hash map with tombstones and be unable to do anything
        verify_hashmap(m);
        return &bucket->item;
    }

    verify_hashmap(m);
    return NULL;
}

static uint64_t fnv1hash(const char* data, size_t size) {
    uint64_t hash = 0xcbf29ce484222325ull;
    size_t i = 0;
    do {
        hash *= 0x100000001b3ull;
        hash ^= data[i];
    } while (++i < size);
    return hash;
}

uint64_t gethash(const void* item, size_t size) {
    const char* data = (const char*)item;
    return fnv1hash(data, size);
}

uint64_t stringhash(const void* p) {
    const String* s = (const String*)p;
    return gethash(s->buffer, s->length);
}

uint64_t cstrhash(const void* p) {
    const char* s = (const char*)p;
    return gethash(s, strlen(s));
}

uint64_t inthash(const void* i) {
    return gethash(i, sizeof(int));
}

uint64_t int8hash(const void* i) {
    return gethash(i, sizeof(int8_t));
}

uint64_t floathash(const void* f) {
    return gethash(f, sizeof(float));
}

#pragma clang diagnostic push
