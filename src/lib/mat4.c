#include <math.h>

#include "lib/math.h"

Matrix4f identity_mat4(void) {
    return (Matrix4f) {
        .c1r1 = 1.f, .c2r1 = 0.f, .c3r1 = 0.f, .c4r1 = 0.f,
        .c1r2 = 0.f, .c2r2 = 1.f, .c3r2 = 0.f, .c4r2 = 0.f,
        .c1r3 = 0.f, .c2r3 = 0.f, .c3r3 = 1.f, .c4r3 = 0.f,
        .c1r4 = 0.f, .c2r4 = 0.f, .c3r4 = 0.f, .c4r4 = 1.f
    };
}

float det_mat4(Matrix4f A) {
    float a = A.a;
    float b = A.b;
    float c = A.c;
    float d = A.d;

    float W = det_mat3((Matrix3f) {
        .a = A.c2r2, .b = A.c3r2, .c = A.c4r2,
        .d = A.c2r3, .e = A.c3r3, .f = A.c4r3,
        .g = A.c2r4, .h = A.c3r4, .i = A.c4r4,
    });

    float X = det_mat3((Matrix3f) {
        .a = A.c1r2, .b = A.c3r2, .c = A.c4r2,
        .d = A.c1r3, .e = A.c3r3, .f = A.c4r3,
        .g = A.c1r4, .h = A.c3r4, .i = A.c4r4,
    });

    float Y = det_mat3((Matrix3f) {
        .a = A.c1r2, .b = A.c2r2, .c = A.c4r2,
        .d = A.c1r3, .e = A.c2r3, .f = A.c4r3,
        .g = A.c1r4, .h = A.c2r4, .i = A.c4r4,
    });

    float Z = det_mat3((Matrix3f) {
        .a = A.c1r2, .b = A.c2r2, .c = A.c3r2,
        .d = A.c1r3, .e = A.c2r3, .f = A.c3r3,
        .g = A.c1r4, .h = A.c2r4, .i = A.c3r4,
    });

    return a * W - b * X + c * Y - d * Z;
}

float trace_mat4(Matrix4f A) {
    return A.c1r1 + A.c2r2 + A.c3r3 + A.c4r4;
}

Matrix4f add_mat4(Matrix4f lhs, Matrix4f rhs) {
    return (Matrix4f) {
        .a = lhs.a + rhs.a, .b = lhs.b + rhs.b, .c = lhs.c + rhs.c, .d = lhs.d + rhs.d,
        .e = lhs.e + rhs.e, .f = lhs.f + rhs.f, .g = lhs.g + rhs.g, .h = lhs.h + rhs.h,
        .i = lhs.i + rhs.i, .j = lhs.j + rhs.j, .k = lhs.k + rhs.k, .l = lhs.l + rhs.l,
        .m = lhs.m + rhs.m, .n = lhs.n + rhs.n, .o = lhs.o + rhs.o, .p = lhs.p + rhs.p,
    };
}

Matrix4f sub_mat4(Matrix4f lhs, Matrix4f rhs) {
    return (Matrix4f) {
        .a = lhs.a - rhs.a, .b = lhs.b - rhs.b, .c = lhs.c - rhs.c, .d = lhs.d - rhs.d,
        .e = lhs.e - rhs.e, .f = lhs.f - rhs.f, .g = lhs.g - rhs.g, .h = lhs.h - rhs.h,
        .i = lhs.i - rhs.i, .j = lhs.j - rhs.j, .k = lhs.k - rhs.k, .l = lhs.l - rhs.l,
        .m = lhs.m - rhs.m, .n = lhs.n - rhs.n, .o = lhs.o - rhs.o, .p = lhs.p - rhs.p,
    };
}

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

Matrix4f mul_scalar_mat4(Matrix4f lhs, float rhs) {
    return (Matrix4f) {
        .c1r1 = rhs * lhs.c1r1, .c2r1 = lhs.c2r1 * rhs, .c3r1 = lhs.c3r1 * rhs, .c4r1 = lhs.c4r1 * rhs,
        .c1r2 = rhs * lhs.c1r2, .c2r2 = lhs.c2r2 * rhs, .c3r2 = lhs.c3r2 * rhs, .c4r2 = lhs.c4r2 * rhs,
        .c1r3 = rhs * lhs.c1r3, .c2r3 = lhs.c2r3 * rhs, .c3r3 = lhs.c3r3 * rhs, .c4r3 = lhs.c4r3 * rhs,
        .c1r4 = rhs * lhs.c1r4, .c2r4 = lhs.c2r4 * rhs, .c3r4 = lhs.c3r4 * rhs, .c4r4 = lhs.c4r4 * rhs
    };
}

Matrix4f pow_mat4(Matrix4f A, unsigned int pow) {
    if (pow == 0) {
        return identity_mat4();
    } else if (pow % 2 == 0) {
        return pow_mat4(mul_mat4(A, A), pow / 2);
    } else {
        return mul_mat4(A, pow_mat4(mul_mat4(A, A), (pow - 1) / 2));
    }
}

Matrix4f transpose_mat4(Matrix4f A) {
    return (Matrix4f) {
        .c1r1 = A.r1c1, .c2r1 = A.r2c1, .c3r1 = A.r3c1, .c4r1 = A.r4c1,
        .c1r2 = A.r1c2, .c2r2 = A.r2c2, .c3r2 = A.r3c2, .c4r2 = A.r4c2,
        .c1r3 = A.r1c3, .c2r3 = A.r2c3, .c3r3 = A.r3c3, .c4r3 = A.r4c3,
        .c1r4 = A.r1c4, .c2r4 = A.r2c4, .c3r4 = A.r3c4, .c4r4 = A.r4c4,
    };
}

Matrix4f inverse_mat4(Matrix4f A) {
    float i_det = 1.f / det_mat4(A);
    Matrix4f A2 = pow_mat4(A, 2);
    Matrix4f A3 = mul_mat4(A, A2);
    float tr = trace_mat4(A);
    float tr2 = trace_mat4(A2);
    float tr3 = trace_mat4(A3);

    float a = (tr * tr * tr - 3.f * tr * tr2 + 2.f * tr3) / 6.f;
    float b = (tr * tr - tr2) / 2.f;
    float c = tr;

    Matrix4f X = mul_scalar_mat4(identity_mat4(), a);
    Matrix4f Y = mul_scalar_mat4(A, b);
    Matrix4f Z = mul_scalar_mat4(A2, c);

    Matrix4f inverted = sub_mat4(X, Y);
    inverted = add_mat4(inverted, Z);
    inverted = sub_mat4(inverted, A3);

    return mul_scalar_mat4(inverted, i_det);
}
