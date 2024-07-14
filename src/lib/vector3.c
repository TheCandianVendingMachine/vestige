#include <math.h>

#include "lib/math.h"

float length_vector3f(Vector3f v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

float length2_vector3f(Vector3f v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

float distance_vector3f(Vector3f lhs, Vector3f rhs) {
    return length_vector3f(sub_vector3f(lhs, rhs));
}

float dot_vector3f(Vector3f lhs, Vector3f rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

Vector3f flip_vector3f(Vector3f v) {
    return (Vector3f) {
        .x = -v.x,
        .y = -v.y,
        .z = -v.z,
    };
}

Vector3f normalise_vector3f(Vector3f v) {
    float length = length_vector3f(v);
    return (Vector3f) { {
        v.x / length,
        v.y / length,
        v.z / length
    } };
}

Vector3f cross_vector3f(Vector3f lhs, Vector3f rhs) {
    return (Vector3f) { {
        lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.z * rhs.x - lhs.x * rhs.z,
        lhs.x * rhs.y - lhs.y * rhs.x
    } };
}

Vector3f add_vector3f(Vector3f lhs, Vector3f rhs) {
    return (Vector3f) { {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z
    } };
}

Vector3f sub_vector3f(Vector3f lhs, Vector3f rhs) {
    return (Vector3f) { {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z
    } };
}

Vector3f mul_vector3f(Vector3f lhs, float rhs) {
    return (Vector3f) { {
        lhs.x * rhs,
        lhs.y * rhs,
        lhs.z * rhs
    } };
}

Vector3f mul_vector3f_pair(Vector3f lhs, Vector3f rhs) {
    return (Vector3f) { {
        lhs.x * rhs.x,
        lhs.y * rhs.y,
        lhs.z * rhs.z
    } };
}

Vector3f clamp_vector3f(Vector3f v, float magnitude) {
    float d2 = dot_vector3f(v, v);
    if (d2 > magnitude * magnitude) {
        return mul_vector3f(normalise_vector3f(v), magnitude);
    }
    return v;
}

Vector3f project_vector3f(Vector3f v, Vector3f a) {
    float dot = dot_vector3f(v, a);
    float d2 = dot_vector3f(a, a);
    return mul_vector3f(a, dot / d2);
}
