#include <lib/vector.h>

Vector new_vector(uint32_t itemsize, size_t size) {
    uint8_t* buffer = malloc(size * itemsize);
    if (buffer == NULL) {
        perror("new_vector: ");
        exit(1);
    }
    Vector v = {
        .length = 0,
        ._size = size,
        ._itemsize = itemsize,
        .buffer = buffer,
    };
    return v;
}

void del_vector(Vector v) {
    free(v.buffer);
}

void vector_grow(Vector* v) {
    v->_size *= 2;
    v->buffer = realloc(v->buffer, 2 * v->_size * v->_itemsize);
    if (v->buffer == NULL) {
        perror("vector_grow: ");
        exit(1);
    }
}
