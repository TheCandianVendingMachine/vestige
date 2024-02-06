#include <math.h>

#include "lib/math.h"

Matrix2f identity_mat2(void) {
    return (Matrix2f) {
        .c1r1 = 1.f, .c2r1 = 0.f,
        .c1r2 = 0.f, .c2r2 = 1.f
    };
}

float det_mat2(Matrix2f A) {
    return A.c1r1 * A.c2r2 - A.c2r1 * A.c1r2;
}

float trace_mat2(Matrix2f A) {
    return A.c1r1 + A.c2r2;
}

Matrix2f add_mat2(Matrix2f lhs, Matrix2f rhs) {
    return (Matrix2f) {
        .a = lhs.a + rhs.a, .b = lhs.b + rhs.b,
        .c = lhs.c + rhs.c, .d = lhs.d + rhs.d
    };
}

Matrix2f sub_mat2(Matrix2f lhs, Matrix2f rhs) {
    return (Matrix2f) {
        .a = lhs.a - rhs.a, .b = lhs.b - rhs.b,
        .c = lhs.c - rhs.c, .d = lhs.d - rhs.d
    };
}

Vector2f mul_mat2vec2(Matrix2f lhs, Vector2f rhs) {
    Vector2f ret;
    ret.x = rhs.x * lhs.c1r1 + rhs.y * lhs.c2r1;
    ret.y = rhs.x * lhs.c1r2 + rhs.y * lhs.c2r2;
    return ret;
}

Matrix2f mul_mat2(Matrix2f lhs, Matrix2f rhs) {
    Matrix2f ret;
    ret.c1r1 = lhs.c1r1 * rhs.c1r1 + lhs.c2r1 * rhs.c1r2;
    ret.c2r1 = lhs.c1r1 * rhs.c2r1 + lhs.c2r1 * rhs.c2r2;
    ret.c1r2 = lhs.c1r2 * rhs.c1r1 + lhs.c2r2 * rhs.c1r2;
    ret.c2r2 = lhs.c1r2 * rhs.c2r1 + lhs.c2r2 * rhs.c2r2;
    return ret;
}

Matrix2f mul_scalar_mat2(Matrix2f lhs, float rhs) {
    return (Matrix2f) {
        .c1r1 = rhs * lhs.c1r1, .c2r1 = lhs.c2r1 * rhs,
        .c1r2 = rhs * lhs.c1r2, .c2r2 = lhs.c2r2 * rhs
    };
}

Matrix2f pow_mat2(Matrix2f A, unsigned int pow) {
    if (pow == 0) {
        return identity_mat2();
    } else if (pow % 2 == 0) {
        return pow_mat2(mul_mat2(A, A), pow / 2);
    } else {
        return mul_mat2(A, pow_mat2(mul_mat2(A, A), (pow - 1) / 2));
    }
}

Matrix2f transpose_mat2(Matrix2f A) {
    return (Matrix2f) {
        .c1r1 = A.r1c1, .c2r1 = A.r2c1,
        .c1r2 = A.r1c2, .c2r2 = A.r2c2
    };
}

Matrix2f inverse_mat2(Matrix2f A) {
    float i_det = 1.f / det_mat2(A);
    return (Matrix2f) {
        .c1r1 = i_det * A.c2r2,  .c2r1 = i_det * -A.c2r1,
        .c1r2 = i_det * -A.c1r2, .c2r2 = i_det * A.c1r1
    };
}

Vector2f solve_mat2(Matrix2f A, Vector2f b) {
    // Simple gaussian elimination for the 2x2 case
    float ca = 1.f - A.c / A.a;
    A.b = A.d * ca;
    b.y = b.y * ca;

    float y = b.y / A.d;
    float x = (b.x - A.b * y) / A.a;
    return (Vector2f) {
        .x = x,
        .y = y
    };
}
