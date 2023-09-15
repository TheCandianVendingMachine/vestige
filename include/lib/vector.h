#ifndef _VESTIGE_LIB_VECTOR_H
#define _VESTIGE_LIB_VECTOR_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define _DEFAULT_VECTOR_SIZE 16u

#define VECTOR(t) \
    (new_vector(sizeof(t), _DEFAULT_VECTOR_SIZE))

#define VECTOR_PUSH(t, v, i) \
	if ((v)->length >= (v)->_size) vector_grow(v); \
	((t*)(v)->buffer)[(v)->length++] = (i);

#define _VECTOR_SET(t, v, ind, item) \
	((t*)(v)->buffer)[(ind)] = (item);

#define VECTOR_SET(t, v, ind, item) \
	if ((ind) < 0 || (ind) > (v)->length) { \
		fprintf(stderr, "VECTOR_SET: index %d out of bounds [0:%ld]", (ind), (v)->length); \
		exit(1); \
	} \
	_VECTOR_SET((t), (v), (ind), (item))

#define _VECTOR_POP(t, v) \
	((t*)(v)->buffer)[(v)->length--]

#define VECTOR_POP(t, v, o) \
	if ((v)->length <= 0) { \
		fprintf(stderr, "VECTOR_POP: popping from empty vector"); \
		exit(1); \
	} \
	(o) = _VECTOR_POP((t), (v));

#define _VECTOR_GET(t, v, i) \
	((t*)(v)->buffer)[i]

#define VECTOR_GET(t, v, i, o) \
	if ((i) < 0 || (i) >= (v)->length) { \
		fprintf(stderr, "VECTOR_GET: index %d out of bounds [0:%ld)", (i), (v)->length); \
		exit(1); \
	} \
	*(o) = _VECTOR_GET(t, v, i);

typedef struct Vector {
    size_t length;  // in terms of items
    size_t _size;  // in terms of items
    uint32_t _itemsize;  // size of item in bytes

    uint8_t* buffer;  // underlying byte buffer
} Vector;

Vector new_vector(uint32_t itemsize, size_t size);
void del_vector(Vector v);
void vector_grow(Vector* v);

#endif
