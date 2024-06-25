#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wincompatible-pointer-types"
#pragma clang diagnostic ignored "-Wint-to-pointer-cast"

#include <lib/hashmap.h>
#include "logger.h"
#include "lib/math.h"
#include "engine_lib.h"

#define GROW_AT 0.60  // percent full to begin growing at
#define BUCKET_ITEM_ALIGNMENT 16

enum BUCKETSTATE {
    UNALLOC = 0xAA,
    ALLOC = 0xCC,
    TOMBSTONE = 0xFF,
} __attribute__ ((packed));  // hopefully only use one byte

struct _Bucket {
#ifdef VESTIGE_CORE_HASHMAP_INTEGRITY_CHECK
    uint8_t initialised;
#endif
    uint64_t hash;
    enum BUCKETSTATE state;  // state of the bucket
    uint8_t __packing[BUCKET_ITEM_ALIGNMENT - ((sizeof(uint64_t) + sizeof(enum BUCKETSTATE)) % BUCKET_ITEM_ALIGNMENT)];  // we assume the alignment is 8 for the item
    uint8_t item;  // for variable-sized items
};

#define BUCKET_BYTE_COUNT (sizeof(struct _Bucket) - sizeof(uint8_t))

static struct _Bucket* get_bucket(const struct _Bucket* buckets, int index, size_t itemsize, size_t alignment_offset);
#ifdef VESTIGE_CORE_HASHMAP_INTEGRITY_CHECK
void _hashmap_verify_integrity(HashMap m, int line) {
    if (m.initialised != VESTIGE_CORE_HASHMAP_INITIALIED_KEY) {
        log_error("hashmap integrity check failed: initiliased=%02X", m.initialised);
        engine_crash_at(SHUTDOWN_CORE_INTEGRITY_FAILURE, line);
    }

    for (int i = 0; i < m._size; i++) {
        uint8_t state = get_bucket(m._buckets, i, m._itemsize, m._alignment_offset)->state;
        if (state != UNALLOC && state != ALLOC && state != TOMBSTONE) {
            log_error("hashmap integrity check failed: index=%d, state=%02X", i, state);
            engine_crash_at(SHUTDOWN_CORE_INTEGRITY_FAILURE, line);
        }
    }
}

void _hashmap_verify_bucket_integrity(struct _Bucket b, int line) {
    if (b.initialised != VESTIGE_CORE_HASHMAP_INITIALIED_KEY) {
        engine_crash_at(SHUTDOWN_CORE_INTEGRITY_FAILURE, line);
    }
}

#define hashmap_verify_integrity(m) _hashmap_verify_integrity(m, __LINE__)
#define hashmap_verify_bucket_integrity(b) _hashmap_verify_bucket_integrity(b, __LINE__)
#else
void hashmap_verify_integrity(HashMap m) {}
void hashmap_verify_bucket_integrity(struct _Bucket b) {}

#define hashmap_verify_integrity(m) ;
#define hashmap_verify_bucket_integrity(b) ;
#endif

static struct _Bucket* get_bucket(const struct _Bucket* buckets, int index, size_t itemsize, size_t alignment_offset) {
    const size_t bucket_byte_size = BUCKET_BYTE_COUNT + itemsize;
    const size_t adjusted_bucket_byte_size = bucket_byte_size + alignment_offset;
    const size_t byte_offset = index * adjusted_bucket_byte_size;
    const uint8_t* bucket_location = ((uint8_t*)buckets) + byte_offset;
    struct _Bucket* bucket = (struct _Bucket*)bucket_location;
    return bucket;
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
        engine_crash(SHUTDOWN_LIBRARY_ERROR);
    }
    for (int i = 0; i < size; i++) {
        struct _Bucket* bucket = get_bucket(_buckets, i, itemsize, alignment_offset);
#ifdef VESTIGE_CORE_HASHMAP_INTEGRITY_CHECK
        bucket->initialised = VESTIGE_CORE_HASHMAP_BUCKET_INITIALIED_KEY;
#endif
        bucket->state = UNALLOC;
    }
    HashMap m = {
#ifdef VESTIGE_CORE_HASHMAP_INTEGRITY_CHECK
        .initialised = VESTIGE_CORE_HASHMAP_INITIALIED_KEY,
#endif
        .length = 0,
        ._size = size,
        ._hash = hash,
        ._itemsize = itemsize,
        ._alignment_offset = alignment_offset,
        ._buckets = _buckets,
    };
    hashmap_verify_integrity(m);
    return m;
}

void del_hashmap(HashMap m) {
    hashmap_verify_integrity(m);
    free(m._buckets);
    m.initialised = (uint8_t)~VESTIGE_CORE_HASHMAP_INITIALIED_KEY;
}

static struct _Bucket* find_entry(HashMap* m, uint64_t hash) {
    hashmap_verify_integrity(*m);
    size_t index = hash % m->_size;
    while (1) {
        struct _Bucket* bucket = get_bucket(m->_buckets, index, m->_itemsize, m->_alignment_offset);

        // Only quit if:
        // UNALLOC => not present
        // ALLOC with matching hash => present
        //
        // Continune while TOMBSTONE or non-matching hash
        if (bucket->state == UNALLOC || (bucket->state == ALLOC && bucket->hash == hash)) {
            hashmap_verify_bucket_integrity(*bucket);
            return bucket;
        }

        index = (index + 1) % m->_size;
    }
    return NULL;
}

static void set_entry(struct _Bucket* bucket, uint64_t hash, enum BUCKETSTATE state, const void* item, size_t itemsize) {
    hashmap_verify_bucket_integrity(*bucket);
    bucket->hash = hash;
    bucket->state = state;
    memcpy(&bucket->item, item, itemsize);
}

void hashmap_grow(HashMap* m) {
    hashmap_verify_integrity(*m);
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
    hashmap_verify_integrity(*m);
}

void hashmap_set(HashMap* m, const void* key, const void* item) {
    hashmap_verify_integrity(*m);
    if (m->length >= (GROW_AT * m->_size)) {
        hashmap_grow(m);
        hashmap_verify_integrity(*m);
    }

    uint64_t hash = m->_hash(key);
    struct _Bucket* bucket = find_entry(m, hash);
    set_entry(bucket, hash, ALLOC, item, m->_itemsize);
    m->length++;
    hashmap_verify_integrity(*m);
}

const void* hashmap_get(HashMap* m, const void* key) {
    hashmap_verify_integrity(*m);
    uint64_t hash = m->_hash(key);
    struct _Bucket* bucket = find_entry(m, hash);
    if (bucket->state != ALLOC) {
        hashmap_verify_integrity(*m);
        return NULL;
    }
    hashmap_verify_integrity(*m);
    return &bucket->item;
}

const void* hashmap_delete(HashMap* m, const void* key) {
    hashmap_verify_integrity(*m);
    uint64_t hash = m->_hash(key);
    struct _Bucket* bucket = find_entry(m, hash);

    hashmap_verify_integrity(*m);
    if (bucket->state == ALLOC) {
        hashmap_verify_integrity(*m);
        bucket->state = TOMBSTONE;  // TODO: optimization, this only needs to be done if next bucket is ALLOC
        m->length--;  // TODO: we could fill up the hash map with tombstones and be unable to do anything
        hashmap_verify_integrity(*m);
        return &bucket->item;
    }

    hashmap_verify_integrity(*m);
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

uint64_t vector2ihash(const void* f) {
    Vector2i v = *(Vector2i*)f;

    // Map negative integers -> positive odds, positive integers -> positive evens
    v.x *= 2;
    v.y *= 2;
    if (v.x < 0) { v.x = (-v.x) + 1; }
    if (v.y < 0) { v.y = (-v.y) + 1; }

    if (v.x < v.y) {
        return v.y * v.y + v.x;
    } else {
        return v.x * v.x + v.x + v.y;
    }
}

uint64_t vector3ihash(const void* f) {
    Vector3i v = *(Vector3i*)f;
    Vector2i v1 = (Vector2i) { .x = v.x, .y = v.y };
    Vector2i v2 = (Vector2i) { .x = v.y, .y = v.z };
    uint64_t p1 = vector2ihash(&v1);
    uint64_t p2 = vector2ihash(&v2);

    // multiply individual pairs by some prime for ordering to matter
    Vector2i final_pair = { .x = p1 * 6803, .y = p2 * 7907 };

    return vector2ihash(&final_pair);
}

#pragma clang diagnostic push
