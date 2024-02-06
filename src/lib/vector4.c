#include <math.h>

#include "lib/math.h"

float length_vector4f(Vector4f v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

float distance_vector4f(Vector4f lhs, Vector4f rhs) {
    return length_vector4f(sub_vector4f(lhs, rhs));
}

float dot_vector4f(Vector4f lhs, Vector4f rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

Vector4f normalise_vector4f(Vector4f v) {
    float length = length_vector4f(v);
    return (Vector4f) { {
        v.x / length,
        v.y / length,
        v.z / length,
        v.w / length
    } };
}

Vector4f add_vector4f(Vector4f lhs, Vector4f rhs) {
    return (Vector4f) { {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z,
        lhs.w + rhs.w
    } };
}

Vector4f sub_vector4f(Vector4f lhs, Vector4f rhs) {
    return (Vector4f) { {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z,
        lhs.w - rhs.w
    } };
}

Vector4f mul_vector4f(Vector4f lhs, float rhs) {
    return (Vector4f) { {
        lhs.x * rhs,
        lhs.y * rhs,
        lhs.z * rhs,
        lhs.w * rhs
    } };
}

Vector4f mul_vector4f_pair(Vector4f lhs, Vector4f rhs) {
    return (Vector4f) { {
        lhs.x * rhs.x,
        lhs.y * rhs.y,
        lhs.z * rhs.z,
        lhs.w * rhs.w
    } };
}

Vector4f clamp_vector4f(Vector4f v, float magnitude) {
    float d2 = dot_vector4f(v, v);
    if (d2 > magnitude * magnitude) {
        return mul_vector4f(normalise_vector4f(v), magnitude);
    }
    return v;
}
