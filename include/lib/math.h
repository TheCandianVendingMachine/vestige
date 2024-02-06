#ifndef _VESTIGE_LIB_MATH_H
#define _VESTIGE_LIB_MATH_H

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// void setseed(long)  // set seed at prog start
// int irandrange(int start, int end)  // uniform dist
// float frandrange(float start, float end)  // uniform dist
// int randint()  // stateful pseudo random

int abs(int);

float max_float(float a, float b);
float min_float(float a, float b);

void setseed(uint32_t[4]);
int randint(void);
int irandrange(int start, int end);
float frandrange(float start, float end);

bool fequal(float lhs, float rhs);

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

typedef union Vector4f {
    struct {
        float x;
        float y;
        float z;
        float w;
    };
    float entries[4];
} Vector4f;

float length_vector2f(Vector2f v);
float distance_vector2f(Vector2f lhs, Vector2f rhs);
float dot_vector2f(Vector2f lhs, Vector2f rhs);
Vector2f normalise_vector2f(Vector2f v);
Vector2f add_vector2f(Vector2f lhs, Vector2f rhs);
Vector2f sub_vector2f(Vector2f lhs, Vector2f rhs);
Vector2f mul_vector2f(Vector2f lhs, float rhs);
Vector2f mul_vector2f_pair(Vector2f lhs, Vector2f rhs);
Vector2f clamp_vector2f(Vector2f v, float magnitude);

float length_vector3f(Vector3f v);
float distance_vector3f(Vector3f lhs, Vector3f rhs);
float dot_vector3f(Vector3f lhs, Vector3f rhs);
Vector3f normalise_vector3f(Vector3f v);
Vector3f cross_vector3f(Vector3f lhs, Vector3f rhs);
Vector3f add_vector3f(Vector3f lhs, Vector3f rhs);
Vector3f sub_vector3f(Vector3f lhs, Vector3f rhs);
Vector3f mul_vector3f(Vector3f lhs, float rhs);
Vector3f mul_vector3f_pair(Vector3f lhs, Vector3f rhs);
Vector3f clamp_vector3f(Vector3f v, float magnitude);

float length_vector4f(Vector4f v);
float distance_vector4f(Vector4f lhs, Vector4f rhs);
float dot_vector4f(Vector4f lhs, Vector4f rhs);
Vector4f normalise_vector4f(Vector4f v);
Vector4f add_vector4f(Vector4f lhs, Vector4f rhs);
Vector4f sub_vector4f(Vector4f lhs, Vector4f rhs);
Vector4f mul_vector4f(Vector4f lhs, float rhs);
Vector4f mul_vector4f_pair(Vector4f lhs, Vector4f rhs);
Vector4f clamp_vector4f(Vector4f v, float magnitude);

typedef union Matrix2f {
    struct {
        float c1r1; float c2r1;
        float c1r2; float c2r2;
    };
    struct {
        float r1c1; float r1c2;
        float r2c1; float r2c2;
    };
    float entries[4];
} Matrix2f;

typedef union Matrix3f {
    struct {
        float c1r1; float c2r1; float c3r1;
        float c1r2; float c2r2; float c3r2;
        float c1r3; float c2r3; float c3r3;
    };
    struct {
        float r1c1; float r1c2; float r1c3;
        float r2c1; float r2c2; float r2c3;
        float r3c1; float r3c2; float r3c3;
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
    struct {
        float r1c1; float r1c2; float r1c3; float r1c4;
        float r2c1; float r2c2; float r2c3; float r2c4;
        float r3c1; float r3c2; float r3c3; float r3c4;
        float r4c1; float r4c2; float r4c3; float r4c4;
    };
    float entries[16];
} Matrix4f;

Vector2f mul_mat2vec2(Matrix2f lhs, Vector2f rhs);
Matrix2f mul_mat2(Matrix2f lhs, Matrix2f rhs);
Matrix2f transpose_mat2(Matrix2f A);
Matrix2f inverse_mat2(Matrix2f A);

Vector3f mul_mat3vec3(Matrix3f lhs, Vector3f rhs);
Matrix3f mul_mat3(Matrix3f lhs, Matrix3f rhs);
Matrix3f transpose_mat3(Matrix3f A);

Vector4f mul_mat4vec4(Matrix4f lhs, Vector4f rhs);
Matrix4f mul_mat4(Matrix4f lhs, Matrix4f rhs);
Matrix4f transpose_mat4(Matrix4f A);

Matrix4f matrix_orthographic_projection(float left, float right, float top, float bottom, float near, float far);

typedef struct AABB {
    Vector2f position;
    Vector2f size;
} AABB;

float aabb_area(AABB aabb);
bool aabb_intersect(AABB a, AABB b);

#endif
