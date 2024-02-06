#include <lib/math.h>

#include "test_info.h"

void test_mat2(TestInfo* info) {
    Matrix2f m0 = (Matrix2f) {
        .c1r1 = 4.f, .c2r1 = -5.f,
        .c1r2 = -1.f, .c2r2 = -8.f
    };

    Matrix2f m1 = (Matrix2f) {
        .c1r1 = -8.f, .c2r1 = -10.f,
        .c1r2 = 0.f, .c2r2 = 1.f
    };

    Matrix2f r = inverse_mat2(m0);
    TEST_ASSERT(fequal(r.c1r1, 8.f / 37.f), info);
    TEST_ASSERT(fequal(r.c2r1, -5.f / 37.f), info);
    TEST_ASSERT(fequal(r.c1r2, -1.f / 37.f), info);
    TEST_ASSERT(fequal(r.c2r2, -4.f / 37.f), info);

    r = mul_mat2(m0, m1);
    TEST_ASSERT(fequal(r.c1r1, -32.f), info);
    TEST_ASSERT(fequal(r.c2r1, -45.f), info);
    TEST_ASSERT(fequal(r.c1r2, 8.f), info);
    TEST_ASSERT(fequal(r.c2r2, 2.f), info);

    Vector2f v = (Vector2f) {
        .x = 7.f,
        .y = -4.f
    };
    Vector2f vr = mul_mat2vec2(m0, v);
    TEST_ASSERT(fequal(vr.x, 48.f), info);
    TEST_ASSERT(fequal(vr.y, 25.f), info);

    r = transpose_mat2(m0);
    TEST_ASSERT(fequal(r.c1r1, 4.f), info);
    TEST_ASSERT(fequal(r.c2r1, -1.f), info);
    TEST_ASSERT(fequal(r.c1r2, -5.f), info);
    TEST_ASSERT(fequal(r.c2r2, -8.f), info);
}

void test_mat3(TestInfo* info) {
    Matrix3f m0 = (Matrix3f) {
        .c1r1 = 4.f, .c2r1 = -5.f, .c3r1 = 15.f,
        .c1r2 = -1.f, .c2r2 = -8.f, .c3r2 = 7.f,
        .c1r3 = 0.f, .c2r3 = 0.f, .c3r3 = 1.f,
    };

    Matrix3f m1 = (Matrix3f) {
        .c1r1 = -1.f, .c2r1 = -9.f, .c3r1 = 0.f,
        .c1r2 = -1.f, .c2r2 = 34.f, .c3r2 = 0.f,
        .c1r3 = 18.f, .c2r3 = 1.f, .c3r3 = 1.f,
    };

    Matrix3f r = mul_mat3(m0, m1);
    TEST_ASSERT(fequal(r.c1r1, 271.f), info);
    TEST_ASSERT(fequal(r.c2r1, -191.f), info);
    TEST_ASSERT(fequal(r.c3r1, 15.f), info);
    TEST_ASSERT(fequal(r.c1r2, 135.f), info);
    TEST_ASSERT(fequal(r.c2r2, -256.f), info);
    TEST_ASSERT(fequal(r.c3r2, 7.f), info);
    TEST_ASSERT(fequal(r.c1r3, 18.f), info);
    TEST_ASSERT(fequal(r.c2r3, 1.f), info);
    TEST_ASSERT(fequal(r.c3r3, 1.f), info);

    Vector3f v = (Vector3f) {
        .x = 7.f,
        .y = -4.f,
        .z = 3.f
    };
    Vector3f vr = mul_mat3vec3(m0, v);
    TEST_ASSERT(fequal(vr.x, 93.f), info);
    TEST_ASSERT(fequal(vr.y, 46.f), info);
    TEST_ASSERT(fequal(vr.z, 3.f), info);

    r = transpose_mat3(m0);
    TEST_ASSERT(fequal(r.c1r1, 4.f), info);
    TEST_ASSERT(fequal(r.c2r1, -1.f), info);
    TEST_ASSERT(fequal(r.c3r1, 0.f), info);
    TEST_ASSERT(fequal(r.c1r2, -5.f), info);
    TEST_ASSERT(fequal(r.c2r2, -8.f), info);
    TEST_ASSERT(fequal(r.c3r2, 0.f), info);
    TEST_ASSERT(fequal(r.c1r3, 15.f), info);
    TEST_ASSERT(fequal(r.c2r3, 7.f), info);
    TEST_ASSERT(fequal(r.c3r3, 1.f), info);
}

void test_mat4(TestInfo* info) {
    Matrix4f m0 = (Matrix4f) {
        .c1r1 = 4.f, .c2r1 = -5.f, .c3r1 = 15.f, .c4r1 = 10.f,
        .c1r2 = -1.f, .c2r2 = -8.f, .c3r2 = 7.f, .c4r2 = 3.f,
        .c1r3 = 0.f, .c2r3 = 0.f, .c3r3 = 1.f, .c4r3 = -6.f,
        .c1r4 = -7.f, .c2r4 = -12.f, .c3r4 = 1.f, .c4r4 = -6.f
    };

    Matrix4f m1 = (Matrix4f) {
        .c1r1 = 1.f, .c2r1 = -15.f, .c3r1 = 8.f, .c4r1 = 0.f,
        .c1r2 = -1.f, .c2r2 = -8.f, .c3r2 = 7.f, .c4r2 = 0.f,
        .c1r3 = 5.f, .c2r3 = 3.f, .c3r3 = -5.f, .c4r3 = -5.f,
        .c1r4 = 9.f, .c2r4 = -8.f, .c3r4 = -18.f, .c4r4 = -8.f
    };

    Matrix4f r = mul_mat4(m0, m1);
    TEST_ASSERT(fequal(r.c1r1, 174.f), info);
    TEST_ASSERT(fequal(r.c2r1, -55.f), info);
    TEST_ASSERT(fequal(r.c3r1, -258.f), info);
    TEST_ASSERT(fequal(r.c4r1, -155.f), info);
    TEST_ASSERT(fequal(r.c1r2, 69.f), info);
    TEST_ASSERT(fequal(r.c2r2, 76.f), info);
    TEST_ASSERT(fequal(r.c3r2, -153.f), info);
    TEST_ASSERT(fequal(r.c4r2, -59.f), info);
    TEST_ASSERT(fequal(r.c1r3, -49.f), info);
    TEST_ASSERT(fequal(r.c2r3, 51.f), info);
    TEST_ASSERT(fequal(r.c3r3, 103.f), info);
    TEST_ASSERT(fequal(r.c4r3, 43.f), info);
    TEST_ASSERT(fequal(r.c1r4, -44.f), info);
    TEST_ASSERT(fequal(r.c2r4, 252.f), info);
    TEST_ASSERT(fequal(r.c3r4, -37.f), info);
    TEST_ASSERT(fequal(r.c4r4, 43.f), info);

    Vector4f v = (Vector4f) {
        .x = 7.f,
        .y = -4.f,
        .z = 3.f,
        .w = -10.f
    };
    Vector4f vr = mul_mat4vec4(m0, v);
    TEST_ASSERT(fequal(vr.x, -7.f), info);
    TEST_ASSERT(fequal(vr.y, 16.f), info);
    TEST_ASSERT(fequal(vr.z, 63.f), info);
    TEST_ASSERT(fequal(vr.w, 62.f), info);

    r = transpose_mat4(m0);
    TEST_ASSERT(fequal(r.c1r1, 4.f), info);
    TEST_ASSERT(fequal(r.c2r1, -1.f), info);
    TEST_ASSERT(fequal(r.c3r1, 0.f), info);
    TEST_ASSERT(fequal(r.c4r1, -7.f), info);
    TEST_ASSERT(fequal(r.c1r2, -5.f), info);
    TEST_ASSERT(fequal(r.c2r2, -8.f), info);
    TEST_ASSERT(fequal(r.c3r2, 0.f), info);
    TEST_ASSERT(fequal(r.c4r2, -12.f), info);
    TEST_ASSERT(fequal(r.c1r3, 15.f), info);
    TEST_ASSERT(fequal(r.c2r3, 7.f), info);
    TEST_ASSERT(fequal(r.c3r3, 1.f), info);
    TEST_ASSERT(fequal(r.c4r3, 1.f), info);
    TEST_ASSERT(fequal(r.c1r4, 10.f), info);
    TEST_ASSERT(fequal(r.c2r4, 3.f), info);
    TEST_ASSERT(fequal(r.c3r4, -6.f), info);
    TEST_ASSERT(fequal(r.c4r4, -6.f), info);
}

void test_mat(void* complete) {
    TestInfo* info = (TestInfo*)complete;
    test_mat2(info);
    test_mat3(info);
    test_mat4(info);

    if (info->status == INCONCLUSIVE) {
        info->status = SUCCESS;
    }
}
