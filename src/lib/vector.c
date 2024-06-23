#include <assert.h>
#include <lib/vector.h>

#include "engine_lib.h"

#ifdef VESTIGE_CORE_VECTOR_INTEGRITY_CHECK
void vector_integrity_check(Vector v) {
    if (v.initialised != VESTIGE_CORE_VECTOR_INITIALIED_KEY) {
        engine_crash(SHUTDOWN_CORE_INTEGRITY_FAILURE);
    }
}
#else
void vector_integrity_check(Vector v) {
}
#endif

Vector new_vector(uint32_t itemsize, size_t size) {
    uint8_t* buffer = malloc(size * itemsize);
    if (buffer == NULL) {
        perror("new_vector: ");
        engine_crash(SHUTDOWN_LIBRARY_ERROR);
    }
    Vector v = {
#ifdef VESTIGE_CORE_VECTOR_INTEGRITY_CHECK
        .initialised = VESTIGE_CORE_VECTOR_INITIALIED_KEY,
#endif
        .length = 0,
        ._size = size,
        ._itemsize = itemsize,
        .buffer = buffer,
    };
    return v;
}

void del_vector(Vector v) {
    vector_integrity_check(v);
    free(v.buffer);
#ifdef VESTIGE_CORE_VECTOR_INTEGRITY_CHECK
    v.initialised = 0;
#endif
}

void vector_grow(Vector* v) {
    vector_integrity_check(*v);
    v->_size *= 2;
    v->buffer = realloc(v->buffer, v->_size * v->_itemsize);
    if (v->buffer == NULL) {
        perror("vector_grow: ");
        engine_crash(SHUTDOWN_LIBRARY_ERROR);
    }
}
