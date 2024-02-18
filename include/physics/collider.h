#ifndef VESTIGE_COLLIDER_H
#define VESTIGE_COLLIDER_H

#include "lib/math.h"

typedef struct Collider {
    struct {
        Vector2f* vectors;
        size_t count;
    } axes;
    struct {
        enum {
            BOUND_TYPE_AABB,
            BOUND_TYPE_OBB,
            BOUND_TYPE_CIRCLE,
        } type;
        union {
            AABB aabb;
        };
    } bounds;
} Collider;

#endif
