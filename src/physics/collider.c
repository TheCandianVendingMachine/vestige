#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "physics/collider.h"

Collider create_collider(enum BoundType bound_type, union Bound bound) {
    Collider c;
    c.bound.type = bound_type;
    c.bound.shape = bound;

    c.axes._computed = false;
    collider_compute_axes(&c);

    return c;
}

CollisionInfo collision_circle_circle(Collider a, Collider b) {
    CollisionInfo info;
    float distance_between = distance_vector2f(a.bound.shape.circle.position, b.bound.shape.circle.position);
    info.collides = distance_between < a.bound.shape.circle.radius + b.bound.shape.circle.radius;
    if (info.collides) {
        info.minimum_translation_vector = normalise_vector2f(
            sub_vector2f(b.bound.shape.circle.position, a.bound.shape.circle.position)
        );
        float distance = (b.bound.shape.circle.radius + a.bound.shape.circle.radius) - distance_between;
        info.minimum_translation_vector = mul_vector2f(info.minimum_translation_vector, distance);
    }

    return info;
}

struct Projection {
    float min;
    float max;
};

struct Projection project_onto_points(Vector2f* points, size_t count, Vector2f axis) {
    struct Projection projection;
    projection.min = dot_vector2f(points[0], axis);
    projection.max = dot_vector2f(points[0], axis);

    for (size_t i = 0; i < count; i++) {
        float dot = dot_vector2f(axis, points[i]);
        projection.min = min_float(projection.min, dot);
        projection.max = max_float(projection.max, dot);
    }

    return projection;
}

struct Projection project_aabb(ShapeAABB aabb, Vector2f axis) {
    float hx = aabb.size.x / 2.f;
    float hy = aabb.size.y / 2.f;

    Vector2f points[4];
    points[0] = (Vector2f) { .x = aabb.position.x - hx, .y = aabb.position.y - hy };
    points[1] = (Vector2f) { .x = aabb.position.x + hx, .y = aabb.position.y - hy };
    points[2] = (Vector2f) { .x = aabb.position.x + hx, .y = aabb.position.y + hy };
    points[3] = (Vector2f) { .x = aabb.position.x - hx, .y = aabb.position.y + hy };

    return project_onto_points(points, 4, axis);
}

struct Projection project_obb(ShapeOBB obb, Vector2f axis) {
    float hx = obb.size.x / 2.f;
    float hy = obb.size.y / 2.f;
    float c = cos(obb.rotation);
    float s = sin(obb.rotation);

    Vector2f points[4];
    points[0] = (Vector2f) { .x = obb.position.x - hx, .y = obb.position.y - hy };
    points[1] = (Vector2f) { .x = obb.position.x + hx, .y = obb.position.y - hy };
    points[2] = (Vector2f) { .x = obb.position.x + hx, .y = obb.position.y + hy };
    points[3] = (Vector2f) { .x = obb.position.x - hx, .y = obb.position.y + hy };

    for (size_t i = 0; i < 4; i++) {
        points[i] = sub_vector2f(points[i], obb.position);
        float x = c * points[i].x - s * points[i].y;
        float y = s * points[i].x + c * points[i].y;
        points[i].x = x + obb.position.x;
        points[i].y = y + obb.position.y;
    }

    return project_onto_points(points, 4, axis);
}

// Requires:
//  Collider origin has to be within the bounds of the collider
struct Projection project_bound(Collider collider, Vector2f axis) {
    struct Projection projection;

    switch (collider.bound.type) {
        case BOUND_TYPE_AABB: {
            ShapeAABB aabb = collider.bound.shape.aabb;
            aabb.position = add_vector2f(collider.position, aabb.position);
            projection = project_aabb(aabb, axis);
            break;
        }
        case BOUND_TYPE_OBB: {
            ShapeOBB obb = collider.bound.shape.obb;
            obb.position = add_vector2f(collider.position, obb.position);
            projection = project_obb(obb, axis);
            break;
        }
        case BOUND_TYPE_CIRCLE:
            // ruh roh
            assert(false);
            break;
        case BOUND_TYPE_CONVEX_HULL: {
            ShapePolygon p = collider.bound.shape.hull;
            for (size_t i = 0; i < p.point_count; i++) {
                p.points[i] = add_vector2f(collider.position, p.points[i]);
            }
            projection = project_onto_points(p.points, p.point_count, axis);
            for (size_t i = 0; i < p.point_count; i++) {
                p.points[i] = sub_vector2f(p.points[i], collider.position);
            }
            break;
        }
    }

    return projection;
}

bool collider_compute_axes(Collider* collider) {
    if (collider->axes._computed) {
        return false;
    }

    switch (collider->bound.type) {
        case BOUND_TYPE_AABB:
            collider->axes.count = 2;
            collider->axes.vectors = malloc(sizeof(Vector2f) * collider->axes.count);
            collider->axes.vectors[0] = (Vector2f) { .x = 0.f, .y = 1.f };
            collider->axes.vectors[1] = (Vector2f) { .x = 1.f, .y = 0.f };
            break;
        case BOUND_TYPE_OBB:
            collider->axes.count = 2;
            collider->axes.vectors = malloc(sizeof(Vector2f) * collider->axes.count);
            float c = cos(collider->bound.shape.obb.rotation);
            float s = sin(collider->bound.shape.obb.rotation);
            collider->axes.vectors[0] = (Vector2f) { .x = c, .y = s };
            collider->axes.vectors[1] = (Vector2f) { .x = -s, .y = c };
            break;
        case BOUND_TYPE_CIRCLE:
            break;
        case BOUND_TYPE_CONVEX_HULL:
            collider->axes.count = collider->bound.shape.hull.point_count;
            collider->axes.vectors = malloc(sizeof(Vector2f) * collider->axes.count);
            Vector2f p_prev = collider->bound.shape.hull.points[0];
            for (size_t i = 1; i < collider->axes.count; i++) {
                Vector2f p = collider->bound.shape.hull.points[i];
                Vector2f d = sub_vector2f(p, p_prev);
                Vector2f n = (Vector2f) { .x = -d.y, .y = d.x };
                collider->axes.vectors[i - 1] = n;
                p_prev = p;
            }
            Vector2f p = collider->bound.shape.hull.points[0];
            Vector2f d = sub_vector2f(p, p_prev);
            Vector2f n = (Vector2f) { .x = -d.y, .y = d.x };
            collider->axes.vectors[collider->axes.count - 1] = n;
            break;
    }

    collider->axes._computed = true;
    return true;
}

CollisionInfo collider_test_collision(Collider a, Collider b) {
    if (a.bound.type == BOUND_TYPE_CIRCLE && b.bound.type == BOUND_TYPE_CIRCLE) {
        return collision_circle_circle(a, b);
    }

    // if the axes are not computed before we check, we have to compute them. because
    // it's malloc'd we need to free after, only if we allocated
    bool free_axes = collider_compute_axes(&a);

    CollisionInfo info;

    float min_overlap = INFINITY;
    Vector2f overlap_vector;
    for (size_t i = 0; i < a.axes.count; i++) {
        Vector2f axis = a.axes.vectors[i];

        struct Projection p1 = project_bound(a, axis);
        struct Projection p2 = project_bound(b, axis);

        if (p1.max <= p2.min || p1.min >= p2.max) {
            info.collides = false;
            return info;
        } else {
            float overlap = p1.max - p2.min;
            if (overlap < min_overlap) {
                min_overlap = overlap;
                overlap_vector = mul_vector2f(axis, overlap);
            }
            overlap = p2.max - p1.min;
            if (overlap < min_overlap) {
                min_overlap = overlap;
                overlap_vector = mul_vector2f(axis, overlap);
            }
        }
    }

    info.collides = true;
    info.minimum_translation_vector = overlap_vector;

    if (free_axes) {
        free(a.axes.vectors);
    }
    return info;
}
