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
            BOUND_TYPE_CONVEX_HULL,
        } type;
        union {
            AABB aabb;
            OBB obb;
            Circle circle;
        };
    } bounds;
} Collider;

typedef struct CollisionInfo {
    bool collides;
    Vector2f minimum_translation_vector;
} CollisionInfo;

CollisionInfo collider_test_collision(Collider a, Collider b);

#endif
