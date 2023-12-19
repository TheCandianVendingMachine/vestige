#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wincompatible-pointer-types"

#include <lib/hashmap.h>

#define GROW_AT 0.60  // percent full to begin growing at

enum BUCKETSTATE {
    UNALLOC,
    ALLOC,
    TOMBSTONE,
} __attribute__ ((packed));  // hopefully only use one byte

struct _Bucket {
    uint64_t hash;
    enum BUCKETSTATE state;  // state of the bucket
    uint8_t item[];  // for variable-sized items
};

HashMap new_hashmap(size_t itemsize, size_t size, uint64_t (*hash)(const void* key)) {
    struct _Bucket* _buckets = malloc(size * (sizeof(uint64_t) + sizeof(uint8_t) + itemsize));
    if (_buckets == NULL) {
        perror("new_hashmap: ");
        exit(1);
    }
    HashMap m = {
        .length = 0,
        ._size = size,
        ._hash = hash,
        ._itemsize = itemsize,
        ._buckets = _buckets,
    };
    return m;
}

void del_hashmap(HashMap m) {
    free(m._buckets);
}

static struct _Bucket* find_entry(HashMap* m, uint64_t hash) {
    size_t index = hash % m->_size;
    while (1) {
        struct _Bucket* bucket = &m->_buckets[index];

        // Only quit if:
        // UNALLOC => not present
        // ALLOC with matching hash => present
        //
        // Continune while TOMBSTONE or non-matching hash
        if (bucket->state == UNALLOC || (bucket->state == ALLOC && bucket->hash == hash))
            return bucket;

        index = (index + 1) % m->_size;
    }
    return NULL;
}

static void set_entry(struct _Bucket* bucket, uint64_t hash, enum BUCKETSTATE state, const void* item, size_t itemsize) {
    bucket->hash = hash;
    bucket->state = state;
    memcpy(bucket->item, item, itemsize);
}

void hashmap_grow(HashMap* m) {
    struct _Bucket* oldbuckets = m->_buckets;

    m->_size *= 2;
    m->_buckets = malloc(m->_size * (sizeof(uint64_t) + sizeof(uint8_t) + m->_itemsize));
    memset(m->_buckets, 0, m->_size * (sizeof(uint64_t) + sizeof(uint8_t) + m->_itemsize));

    // Move all allocated items over
    for (size_t i = 0; i < m->_size / 2; i++) {
        struct _Bucket* oldbucket = &oldbuckets[i];
        if (oldbucket->state == ALLOC) {
            struct _Bucket* newbucket = find_entry(m, oldbucket->hash);
            set_entry(newbucket, oldbucket->hash, ALLOC, oldbucket->item, m->_itemsize);
        }
    }

    free(oldbuckets);
}

void hashmap_set(HashMap* m, const void* key, const void* item) {
    if (m->length >= (GROW_AT * m->_size))
        hashmap_grow(m);

    uint64_t hash = m->_hash(key);
    struct _Bucket* bucket = find_entry(m, hash);
    set_entry(bucket, hash, ALLOC, item, m->_itemsize);
    m->length++;
}

const void* hashmap_get(HashMap* m, const void* key) {
    uint64_t hash = m->_hash(key);
    struct _Bucket* bucket = find_entry(m, hash);
    if (bucket->state != ALLOC)
        return NULL;
    return &bucket->item;
}

const void* hashmap_delete(HashMap* m, const void* key) {
    uint64_t hash = m->_hash(key);
    struct _Bucket* bucket = find_entry(m, hash);

    if (bucket->state == ALLOC) {
        bucket->state = TOMBSTONE;  // TODO: optimization, this only needs to be done if next bucket is ALLOC
        m->length--;  // TODO: we could fill up the hash map with tombstones and be unable to do anything
        return &bucket->item;
    }

    return NULL;
}

static uint64_t fnv1hash(const char* data, size_t size) {
    uint64_t hash = 14695981039346656037lu;
    for (size_t i = 0; i < size; i++) {
        hash *= 1099511628211;
        hash ^= data[i];
    }
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

uint64_t floathash(const void* f) {
    return gethash(f, sizeof(float));
}

#pragma clang diagnostic push
