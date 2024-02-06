#include <math.h>

#include "lib/math.h"

Matrix4f matrix_orthographic_projection(float left, float right, float top, float bottom, float near, float far) {
    Matrix4f projection;
    memset(projection.entries, 0, sizeof(projection.entries));
    projection.c1r1 = 2.f / (right - left);
    projection.c2r2 = 2.f / (top - bottom);
    projection.c3r3 = -2.f / (far - near);
    projection.c1r4 = -(right + left) / (right - left);
    projection.c2r4 = -(top + bottom) / (top - bottom);
    projection.c3r4 = -(far + near) / (far - near);
    projection.c4r4 = 1.f;
    return projection;
}

Vector4f mul_mat4vec4(Matrix4f lhs, Vector4f rhs) {
    /*
    [a b c d][x]
    [e f g h][y]
    [i j k l][z]
    [m n o p][w]
    */
    Vector4f ret;
    ret.x = rhs.x * lhs.c1r1 + rhs.y * lhs.c2r1 + rhs.z * lhs.c3r1 + rhs.w * lhs.c4r1;
    ret.y = rhs.x * lhs.c1r2 + rhs.y * lhs.c2r2 + rhs.z * lhs.c3r2 + rhs.w * lhs.c4r2;
    ret.z = rhs.x * lhs.c1r3 + rhs.y * lhs.c2r3 + rhs.z * lhs.c3r3 + rhs.w * lhs.c4r3;
    ret.w = rhs.x * lhs.c1r4 + rhs.y * lhs.c2r4 + rhs.z * lhs.c3r4 + rhs.w * lhs.c4r4;
    return ret;
}

Matrix4f mul_mat4(Matrix4f lhs, Matrix4f rhs) {
    /*
    [a b c d][1 2 3 4]
    [e f g h][5 6 7 8]
    [i j k l][9 A B C]
    [m n o p][D E F G]
    */
    float values[16] = {
        lhs.c1r1 * rhs.c1r1 + lhs.c2r1 * rhs.c1r2 + lhs.c3r1 * rhs.c1r3 + lhs.c4r1 * rhs.c1r4,
        lhs.c1r1 * rhs.c2r1 + lhs.c2r1 * rhs.c2r2 + lhs.c3r1 * rhs.c2r3 + lhs.c4r1 * rhs.c2r4,
        lhs.c1r1 * rhs.c3r1 + lhs.c2r1 * rhs.c3r2 + lhs.c3r1 * rhs.c3r3 + lhs.c4r1 * rhs.c3r4,
        lhs.c1r1 * rhs.c4r1 + lhs.c2r1 * rhs.c4r2 + lhs.c3r1 * rhs.c4r3 + lhs.c4r1 * rhs.c4r4,

        lhs.c1r2 * rhs.c1r1 + lhs.c2r2 * rhs.c1r2 + lhs.c3r2 * rhs.c1r3 + lhs.c4r2 * rhs.c1r4,
        lhs.c1r2 * rhs.c2r1 + lhs.c2r2 * rhs.c2r2 + lhs.c3r2 * rhs.c2r3 + lhs.c4r2 * rhs.c2r4,
        lhs.c1r2 * rhs.c3r1 + lhs.c2r2 * rhs.c3r2 + lhs.c3r2 * rhs.c3r3 + lhs.c4r2 * rhs.c3r4,
        lhs.c1r2 * rhs.c4r1 + lhs.c2r2 * rhs.c4r2 + lhs.c3r2 * rhs.c4r3 + lhs.c4r2 * rhs.c4r4,

        lhs.c1r3 * rhs.c1r1 + lhs.c2r3 * rhs.c1r2 + lhs.c3r3 * rhs.c1r3 + lhs.c4r3 * rhs.c1r4,
        lhs.c1r3 * rhs.c2r1 + lhs.c2r3 * rhs.c2r2 + lhs.c3r3 * rhs.c2r3 + lhs.c4r3 * rhs.c2r4,
        lhs.c1r3 * rhs.c3r1 + lhs.c2r3 * rhs.c3r2 + lhs.c3r3 * rhs.c3r3 + lhs.c4r3 * rhs.c3r4,
        lhs.c1r3 * rhs.c4r1 + lhs.c2r3 * rhs.c4r2 + lhs.c3r3 * rhs.c4r3 + lhs.c4r3 * rhs.c4r4,

        lhs.c1r4 * rhs.c1r1 + lhs.c2r4 * rhs.c1r2 + lhs.c3r4 * rhs.c1r3 + lhs.c4r4 * rhs.c1r4,
        lhs.c1r4 * rhs.c2r1 + lhs.c2r4 * rhs.c2r2 + lhs.c3r4 * rhs.c2r3 + lhs.c4r4 * rhs.c2r4,
        lhs.c1r4 * rhs.c3r1 + lhs.c2r4 * rhs.c3r2 + lhs.c3r4 * rhs.c3r3 + lhs.c4r4 * rhs.c3r4,
        lhs.c1r4 * rhs.c4r1 + lhs.c2r4 * rhs.c4r2 + lhs.c3r4 * rhs.c4r3 + lhs.c4r4 * rhs.c4r4,
    };
    Matrix4f ret;
    memcpy(ret.entries, values, 16 * sizeof(float));
    return ret;
}
