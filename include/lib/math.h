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

typedef struct Vector2i {
    int x;
    int y;
} Vector2i;

typedef struct Vector2f {
    float x;
    float y;
} Vector2f;

typedef struct Vector3i {
    int x;
    int y;
    int z;
} Vector3i;

typedef struct Vector3f {
    float x;
    float y;
    float z;
} Vector3f;

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

#endif
