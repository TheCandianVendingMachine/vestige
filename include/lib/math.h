#ifndef _VESTIGE_LIB_MATH_H
#define _VESTIGE_LIB_MATH_H

#include <stdint.h>
#include <string.h>

// void setseed(long)  // set seed at prog start
// int irandrange(int start, int end)  // uniform dist
// float frandrange(float start, float end)  // uniform dist
// int randint()  // stateful pseudo random

int abs(int);

void setseed(uint32_t[4]);
int randint(void);
int irandrange(int start, int end);
float frandrange(float start, float end);

typedef union Vector2i {
    struct {
        int x;
        int y;
    };
    int entries[2];
} Vector2i;

typedef union Vector2f {
    struct {
        float x;
        float y;
    };
    float entries[2];
} Vector2f;

typedef union Vector3i {
    struct {
        int x;
        int y;
        int z;
    };
    int entries[3];
} Vector3i;

typedef union Vector3f {
    struct {
        float x;
        float y;
        float z;
    };
    float entries[3];
} Vector3f;

float length_vector2f(Vector2f v);
float distance_vector2f(Vector2f lhs, Vector2f rhs);
float dot_vector2f(Vector2f lhs, Vector2f rhs);
Vector2f normalise_vector2f(Vector2f v);
Vector2f add_vector2f(Vector2f lhs, Vector2f rhs);
Vector2f sub_vector2f(Vector2f lhs, Vector2f rhs);
Vector2f mul_vector2f(Vector2f lhs, float rhs);

float length_vector3f(Vector3f v);
float distance_vector3f(Vector3f lhs, Vector3f rhs);
float dot_vector3f(Vector3f lhs, Vector3f rhs);
Vector3f normalise_vector3f(Vector3f v);
Vector3f cross_vector3f(Vector3f lhs, Vector3f rhs);
Vector3f add_vector3f(Vector3f lhs, Vector3f rhs);
Vector3f sub_vector3f(Vector3f lhs, Vector3f rhs);
Vector3f mul_vector3f(Vector3f lhs, float rhs);

typedef union Matrix3f {
    struct {
        float c1r1; float c2r1; float c3r1;
        float c1r2; float c2r2; float c3r2;
        float c1r3; float c2r3; float c3r3;
    };
    float entries[9];
} Matrix3f;

typedef union Matrix4f {
    struct {
        float c1r1; float c2r1; float c3r1; float c4r1;
        float c1r2; float c2r2; float c3r2; float c4r2;
        float c1r3; float c2r3; float c3r3; float c4r3;
        float c1r4; float c2r4; float c3r4; float c4r4;
    };
    float entries[16];
} Matrix4f;

Matrix4f matrix_orthographic_projection(float left, float right, float top, float bottom, float near, float far);

typedef struct AABB {
    Vector2f position;
    Vector2f size;
} AABB;

#endif
