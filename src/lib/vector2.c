#include <math.h>

#include "lib/math.h"

float length_vector2f(Vector2f v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

float distance_vector2f(Vector2f lhs, Vector2f rhs) {
    return length_vector2f(sub_vector2f(lhs, rhs));
}

Vector2f normalise_vector2f(Vector2f v) {
    float length = length_vector2f(v);
    return (Vector2f) { {
        v.x / length,
        v.y / length
    } };
}

float dot_vector2f(Vector2f lhs, Vector2f rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

Vector2f add_vector2f(Vector2f lhs, Vector2f rhs) {
    return (Vector2f) { {
        lhs.x + rhs.x,
        lhs.y + rhs.y
    } };
}

Vector2f sub_vector2f(Vector2f lhs, Vector2f rhs) {
    return (Vector2f) { {
        lhs.x - rhs.x,
        lhs.y - rhs.y
    } };
}

Vector2f mul_vector2f(Vector2f lhs, float rhs) {
    return (Vector2f) { {
        lhs.x * rhs,
        lhs.y * rhs
    } };
}

Vector2f mul_vector2f_pair(Vector2f lhs, Vector2f rhs) {
    return (Vector2f) { {
        lhs.x * rhs.x,
        lhs.y * rhs.y
    } };
}

Vector2f clamp_vector2f(Vector2f v, float magnitude) {
    float d2 = dot_vector2f(v, v);
    if (d2 > magnitude * magnitude) {
        return mul_vector2f(normalise_vector2f(v), magnitude);
    }
    return v;
}
