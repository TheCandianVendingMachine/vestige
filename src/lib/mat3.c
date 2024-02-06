#include <math.h>

#include "lib/math.h"

Matrix3f identity_mat3(void) {
    return (Matrix3f) {
        .c1r1 = 1.f, .c2r1 = 0.f, .c3r1 = 0.f,
        .c1r2 = 0.f, .c2r2 = 1.f, .c3r2 = 0.f,
        .c1r3 = 0.f, .c2r3 = 0.f, .c3r3 = 1.f,
    };
}

float det_mat3(Matrix3f A) {
    return A.a * A.e * A.i +
        A.b * A.f * A.g +
        A.c * A.d * A.h -
        A.c * A.e * A.g -
        A.b * A.d * A.i -
        A.a * A.f * A.h;
}

float trace_mat3(Matrix3f A) {
    return A.c1r1 + A.c2r2 + A.c3r3;
}

Matrix3f add_mat3(Matrix3f lhs, Matrix3f rhs) {
    return (Matrix3f) {
        .a = lhs.a + rhs.a, .b = lhs.b + rhs.b, .c = lhs.c + rhs.c,
        .d = lhs.d + rhs.d, .e = lhs.e + rhs.e, .f = lhs.f + rhs.f,
        .g = lhs.g + rhs.g, .h = lhs.h + rhs.h, .i = lhs.i + rhs.i,
    };
}

Matrix3f sub_mat3(Matrix3f lhs, Matrix3f rhs) {
    return (Matrix3f) {
        .a = lhs.a - rhs.a, .b = lhs.b - rhs.b, .c = lhs.c - rhs.c,
        .d = lhs.d - rhs.d, .e = lhs.e - rhs.e, .f = lhs.f - rhs.f,
        .g = lhs.g - rhs.g, .h = lhs.h - rhs.h, .i = lhs.i - rhs.i,
    };
}

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

Matrix3f mul_scalar_mat3(Matrix3f lhs, float rhs) {
    return (Matrix3f) {
        .c1r1 = rhs * lhs.c1r1, .c2r1 = lhs.c2r1 * rhs, .c3r1 = lhs.c3r1 * rhs,
        .c1r2 = rhs * lhs.c1r2, .c2r2 = lhs.c2r2 * rhs, .c3r2 = lhs.c3r2 * rhs,
        .c1r3 = rhs * lhs.c1r3, .c2r3 = lhs.c2r3 * rhs, .c3r3 = lhs.c3r3 * rhs,
    };
}

Matrix3f pow_mat3(Matrix3f A, unsigned int pow) {
    if (pow == 0) {
        return identity_mat3();
    } else if (pow % 2 == 0) {
        return pow_mat3(mul_mat3(A, A), pow / 2);
    } else {
        return mul_mat3(A, pow_mat3(mul_mat3(A, A), (pow - 1) / 2));
    }
}

Matrix3f transpose_mat3(Matrix3f A) {
    return (Matrix3f) {
        .c1r1 = A.r1c1, .c2r1 = A.r2c1, .c3r1 = A.r3c1,
        .c1r2 = A.r1c2, .c2r2 = A.r2c2, .c3r2 = A.r3c2,
        .c1r3 = A.r1c3, .c2r3 = A.r2c3, .c3r3 = A.r3c3,
    };
}

Matrix3f inverse_mat3(Matrix3f A) {
    float i_det = 1.f / det_mat3(A);
    float As=  (A.e * A.i - A.f * A.h);
    float B = -(A.d * A.i - A.f * A.g);
    float C =  (A.d * A.h - A.e * A.g);
    float D = -(A.b * A.i - A.c * A.h);
    float E =  (A.a * A.i - A.c * A.g);
    float F = -(A.a * A.h - A.b * A.g);
    float G =  (A.b * A.f - A.c * A.e);
    float H = -(A.a * A.f - A.c * A.d);
    float I =  (A.a * A.e - A.b * A.d);

    return (Matrix3f) {
        .c1r1 = i_det *As, .c2r1 = i_det * D, .c3r1 = i_det * G,
        .c1r2 = i_det * B, .c2r2 = i_det * E, .c3r2 = i_det * H,
        .c1r3 = i_det * C, .c2r3 = i_det * F, .c3r3 = i_det * I
    };
}
