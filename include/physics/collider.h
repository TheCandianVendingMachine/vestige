#ifndef VESTIGE_COLLIDER_H
#define VESTIGE_COLLIDER_H

#include "lib/math.h"

typedef enum BoundType {
    BOUND_TYPE_AABB,
    BOUND_TYPE_OBB,
    BOUND_TYPE_CIRCLE,
    BOUND_TYPE_CONVEX_HULL,
} BoundType;

typedef union Bound {
    ShapeAABB aabb;
    ShapeOBB obb;
    ShapeCircle circle;
    ShapePolygon hull;
} Bound;

typedef struct Collider {
    Vector2f position;
    struct {
        Vector2f* vectors;
        size_t count;
        bool _computed;
    } axes;
    struct {
        Bound shape;
        BoundType type;
    } bound;
} Collider;

Collider create_collider(enum BoundType bound_type, union Bound bound);

typedef struct CollisionInfo {
    bool collides;
    Vector2f minimum_translation_vector;
} CollisionInfo;

bool collider_compute_axes(Collider* collider);
CollisionInfo collider_test_collision(Collider a, Collider b);

#endif
