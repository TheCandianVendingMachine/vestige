#include <math.h>

#include "lib/math.h"

Vector3f mul_mat3vec3(Matrix3f lhs, Vector3f rhs) {
    /*
    [a b c][x]
    [d e f][y]
    [g h i][z]
    */
    Vector3f ret;
    ret.x = rhs.x * lhs.c1r1 + rhs.y * lhs.c2r1 + rhs.z * lhs.c3r1;
    ret.y = rhs.x * lhs.c1r2 + rhs.y * lhs.c2r2 + rhs.z * lhs.c3r2;
    ret.z = rhs.x * lhs.c1r3 + rhs.y * lhs.c2r3 + rhs.z * lhs.c3r3;
    return ret;
}

Matrix3f mul_mat3(Matrix3f lhs, Matrix3f rhs) {
    /*
    [a b c][j k l]
    [d e f][m n o]
    [g h i][p q r]
    */
    float values[9] = {
        lhs.c1r1 * rhs.c1r1 + lhs.c2r1 * rhs.c1r2 + lhs.c3r1 * rhs.c1r3,
        lhs.c1r1 * rhs.c2r1 + lhs.c2r1 * rhs.c2r2 + lhs.c3r1 * rhs.c2r3,
        lhs.c1r1 * rhs.c3r1 + lhs.c2r1 * rhs.c3r2 + lhs.c3r1 * rhs.c3r3,

        lhs.c1r2 * rhs.c1r1 + lhs.c2r2 * rhs.c1r2 + lhs.c3r2 * rhs.c1r3,
        lhs.c1r2 * rhs.c2r1 + lhs.c2r2 * rhs.c2r2 + lhs.c3r2 * rhs.c2r3,
        lhs.c1r2 * rhs.c3r1 + lhs.c2r2 * rhs.c3r2 + lhs.c3r2 * rhs.c3r3,

        lhs.c1r3 * rhs.c1r1 + lhs.c2r3 * rhs.c1r2 + lhs.c3r3 * rhs.c1r3,
        lhs.c1r3 * rhs.c2r1 + lhs.c2r3 * rhs.c2r2 + lhs.c3r3 * rhs.c2r3,
        lhs.c1r3 * rhs.c3r1 + lhs.c2r3 * rhs.c3r2 + lhs.c3r3 * rhs.c3r3,
    };
    Matrix3f ret;
    memcpy(ret.entries, values, 9 * sizeof(float));
    return ret;
}
