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

    float d = det_mat2(m0);
    TEST_ASSERT(fequal(d, -37.f), info);

    Matrix2f r;
    r = add_mat2(m0, m1);
    TEST_ASSERT(fequal(r.c1r1, -4.f), info);
    TEST_ASSERT(fequal(r.c2r1, -15.f), info);
    TEST_ASSERT(fequal(r.c1r2, -1.f), info);
    TEST_ASSERT(fequal(r.c2r2, -7.f), info);

    r = sub_mat2(m0, m1);
    TEST_ASSERT(fequal(r.c1r1, 12.f), info);
    TEST_ASSERT(fequal(r.c2r1, 5.f), info);
    TEST_ASSERT(fequal(r.c1r2, -1.f), info);
    TEST_ASSERT(fequal(r.c2r2, -9.f), info);

    r = mul_mat2(m0, m1);
    TEST_ASSERT(fequal(r.c1r1, -32.f), info);
    TEST_ASSERT(fequal(r.c2r1, -45.f), info);
    TEST_ASSERT(fequal(r.c1r2, 8.f), info);
    TEST_ASSERT(fequal(r.c2r2, 2.f), info);

    r = mul_scalar_mat2(m0, 2.f);
    TEST_ASSERT(fequal(r.c1r1, 8.f), info);
    TEST_ASSERT(fequal(r.c2r1, -10.f), info);
    TEST_ASSERT(fequal(r.c1r2, -2.f), info);
    TEST_ASSERT(fequal(r.c2r2, -16.f), info);

    r = pow_mat2(m0, 5);
    TEST_ASSERT(fequal(r.c1r1, 284.f), info);
    TEST_ASSERT(fequal(r.c2r1, -17005.f), info);
    TEST_ASSERT(fequal(r.c1r2, -3401.f), info);
    TEST_ASSERT(fequal(r.c2r2, -40528.f), info);


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

    float t = trace_mat2(m0);
    TEST_ASSERT(fequal(t, -4.f), info);

    r = inverse_mat2(m0);
    TEST_ASSERT(fequal(r.c1r1, 8.f / 37.f), info);
    TEST_ASSERT(fequal(r.c2r1, -5.f / 37.f), info);
    TEST_ASSERT(fequal(r.c1r2, -1.f / 37.f), info);
    TEST_ASSERT(fequal(r.c2r2, -4.f / 37.f), info);

    r = mul_mat2(m0, inverse_mat2(m0));
    TEST_ASSERT(fequal(r.c1r1, 1.f), info);
    TEST_ASSERT(fequal(r.c2r1, 0.f), info);
    TEST_ASSERT(fequal(r.c1r2, 0.f), info);
    TEST_ASSERT(fequal(r.c2r2, 1.f), info);
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

    float d = det_mat3(m0);
    TEST_ASSERT(fequal(d, -37.f), info);

    Matrix3f r;
    r = add_mat3(m0, m1);
    TEST_ASSERT(fequal(r.c1r1, 3.f), info);
    TEST_ASSERT(fequal(r.c2r1, -14.f), info);
    TEST_ASSERT(fequal(r.c3r1, 15.f), info);
    TEST_ASSERT(fequal(r.c1r2, -2.f), info);
    TEST_ASSERT(fequal(r.c2r2, 26.f), info);
    TEST_ASSERT(fequal(r.c3r2, 7.f), info);
    TEST_ASSERT(fequal(r.c1r3, 18.f), info);
    TEST_ASSERT(fequal(r.c2r3, 1.f), info);
    TEST_ASSERT(fequal(r.c3r3, 2.f), info);

    r = sub_mat3(m0, m1);
    TEST_ASSERT(fequal(r.c1r1, 5.f), info);
    TEST_ASSERT(fequal(r.c2r1, 4.f), info);
    TEST_ASSERT(fequal(r.c3r1, 15.f), info);
    TEST_ASSERT(fequal(r.c1r2, 0.f), info);
    TEST_ASSERT(fequal(r.c2r2, -42.f), info);
    TEST_ASSERT(fequal(r.c3r2, 7.f), info);
    TEST_ASSERT(fequal(r.c1r3, -18.f), info);
    TEST_ASSERT(fequal(r.c2r3, -1.f), info);
    TEST_ASSERT(fequal(r.c3r3, 0.f), info);

    r = mul_mat3(m0, m1);
    TEST_ASSERT(fequal(r.c1r1, 271.f), info);
    TEST_ASSERT(fequal(r.c2r1, -191.f), info);
    TEST_ASSERT(fequal(r.c3r1, 15.f), info);
    TEST_ASSERT(fequal(r.c1r2, 135.f), info);
    TEST_ASSERT(fequal(r.c2r2, -256.f), info);
    TEST_ASSERT(fequal(r.c3r2, 7.f), info);
    TEST_ASSERT(fequal(r.c1r3, 18.f), info);
    TEST_ASSERT(fequal(r.c2r3, 1.f), info);
    TEST_ASSERT(fequal(r.c3r3, 1.f), info);

    r = mul_scalar_mat3(m0, 2.f);
    TEST_ASSERT(fequal(r.c1r1, 8.f), info);
    TEST_ASSERT(fequal(r.c2r1, -10.f), info);
    TEST_ASSERT(fequal(r.c3r1, 30.f), info);
    TEST_ASSERT(fequal(r.c1r2, -2.f), info);
    TEST_ASSERT(fequal(r.c2r2, -16.f), info);
    TEST_ASSERT(fequal(r.c3r2, 14.f), info);
    TEST_ASSERT(fequal(r.c1r3, 0.f), info);
    TEST_ASSERT(fequal(r.c2r3, 0.f), info);
    TEST_ASSERT(fequal(r.c3r3, 2.f), info);

    r = pow_mat3(m0, 5);
    TEST_ASSERT(fequal(r.c1r1, 284.f), info);
    TEST_ASSERT(fequal(r.c2r1, -17005.f), info);
    TEST_ASSERT(fequal(r.c3r1, 20015.f), info);
    TEST_ASSERT(fequal(r.c1r2, -3401.f), info);
    TEST_ASSERT(fequal(r.c2r2, -40528.f), info);
    TEST_ASSERT(fequal(r.c3r2, 34967.f), info);
    TEST_ASSERT(fequal(r.c1r3, 0.f), info);
    TEST_ASSERT(fequal(r.c2r3, 0.f), info);
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

    float t = trace_mat3(m0);
    TEST_ASSERT(fequal(t, -3.f), info);

    r = inverse_mat3(m0);
    TEST_ASSERT(fequal(r.c1r1, 8.f / 37.f), info);
    TEST_ASSERT(fequal(r.c2r1, -5.f / 37.f), info);
    TEST_ASSERT(fequal(r.c3r1, -85.f / 37.f), info);
    TEST_ASSERT(fequal(r.c1r2, -1.f / 37.f), info);
    TEST_ASSERT(fequal(r.c2r2, -4.f / 37.f), info);
    TEST_ASSERT(fequal(r.c3r2, 43.f / 37.f), info);
    TEST_ASSERT(fequal(r.c1r3, 0.f), info);
    TEST_ASSERT(fequal(r.c2r3, 0.f), info);
    TEST_ASSERT(fequal(r.c3r3, 1.f), info);

    r = mul_mat3(m0, inverse_mat3(m0));
    TEST_ASSERT(fequal(r.c1r1, 1.f), info);
    TEST_ASSERT(fequal(r.c2r1, 0.f), info);
    TEST_ASSERT(fequal(r.c3r1, 0.f), info);
    TEST_ASSERT(fequal(r.c1r2, 0.f), info);
    TEST_ASSERT(fequal(r.c2r2, 1.f), info);
    TEST_ASSERT(fequal(r.c3r2, 0.f), info);
    TEST_ASSERT(fequal(r.c1r3, 0.f), info);
    TEST_ASSERT(fequal(r.c2r3, 0.f), info);
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

    float d = det_mat4(m0);
    TEST_ASSERT(fequal(d, -665.f), info);

    Matrix4f r;
    r = add_mat4(m0, m1);
    TEST_ASSERT(fequal(r.c1r1, 5.f), info);
    TEST_ASSERT(fequal(r.c2r1, -20.f), info);
    TEST_ASSERT(fequal(r.c3r1, 23.f), info);
    TEST_ASSERT(fequal(r.c4r1, 10.f), info);
    TEST_ASSERT(fequal(r.c1r2, -2.f), info);
    TEST_ASSERT(fequal(r.c2r2, -16.f), info);
    TEST_ASSERT(fequal(r.c3r2, 14.f), info);
    TEST_ASSERT(fequal(r.c4r2, 3.f), info);
    TEST_ASSERT(fequal(r.c1r3, 5.f), info);
    TEST_ASSERT(fequal(r.c2r3, 3.f), info);
    TEST_ASSERT(fequal(r.c3r3, -4.f), info);
    TEST_ASSERT(fequal(r.c4r3, -11.f), info);
    TEST_ASSERT(fequal(r.c1r4, 2.f), info);
    TEST_ASSERT(fequal(r.c2r4, -20.f), info);
    TEST_ASSERT(fequal(r.c3r4, -17.f), info);
    TEST_ASSERT(fequal(r.c4r4, -14.f), info);

    r = sub_mat4(m0, m1);
    TEST_ASSERT(fequal(r.c1r1, 3.f), info);
    TEST_ASSERT(fequal(r.c2r1, 10.f), info);
    TEST_ASSERT(fequal(r.c3r1, 7.f), info);
    TEST_ASSERT(fequal(r.c4r1, 10.f), info);
    TEST_ASSERT(fequal(r.c1r2, 0.f), info);
    TEST_ASSERT(fequal(r.c2r2, 0.f), info);
    TEST_ASSERT(fequal(r.c3r2, 0.f), info);
    TEST_ASSERT(fequal(r.c4r2, 3.f), info);
    TEST_ASSERT(fequal(r.c1r3, -5.f), info);
    TEST_ASSERT(fequal(r.c2r3, -3.f), info);
    TEST_ASSERT(fequal(r.c3r3, 6.f), info);
    TEST_ASSERT(fequal(r.c4r3, -1.f), info);
    TEST_ASSERT(fequal(r.c1r4, -16.f), info);
    TEST_ASSERT(fequal(r.c2r4, -4.f), info);
    TEST_ASSERT(fequal(r.c3r4, 19.f), info);
    TEST_ASSERT(fequal(r.c4r4, 2.f), info);

    r = mul_mat4(m0, m1);
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

    r = mul_scalar_mat4(m0, 2.f);
    TEST_ASSERT(fequal(r.c1r1, 8.f), info);
    TEST_ASSERT(fequal(r.c2r1, -10.f), info);
    TEST_ASSERT(fequal(r.c3r1, 30.f), info);
    TEST_ASSERT(fequal(r.c4r1, 20.f), info);
    TEST_ASSERT(fequal(r.c1r2, -2.f), info);
    TEST_ASSERT(fequal(r.c2r2, -16.f), info);
    TEST_ASSERT(fequal(r.c3r2, 14.f), info);
    TEST_ASSERT(fequal(r.c4r2, 6.f), info);
    TEST_ASSERT(fequal(r.c1r3, 0.f), info);
    TEST_ASSERT(fequal(r.c2r3, 0.f), info);
    TEST_ASSERT(fequal(r.c3r3, 2.f), info);
    TEST_ASSERT(fequal(r.c4r3, -12.f), info);
    TEST_ASSERT(fequal(r.c1r4, -14.f), info);
    TEST_ASSERT(fequal(r.c2r4, -24.f), info);
    TEST_ASSERT(fequal(r.c3r4, 2.f), info);
    TEST_ASSERT(fequal(r.c4r4, -12.f), info);

    r = pow_mat4(m0, 5);
    TEST_ASSERT(fequal(r.c1r1, -153696.f), info);
    TEST_ASSERT(fequal(r.c2r1, -171655.f), info);
    TEST_ASSERT(fequal(r.c3r1, -42785.f), info);
    TEST_ASSERT(fequal(r.c4r1, -356065.f), info);
    TEST_ASSERT(fequal(r.c1r2, -28610.f), info);
    TEST_ASSERT(fequal(r.c2r2, 137106.f), info);
    TEST_ASSERT(fequal(r.c3r2, -195790.f), info);
    TEST_ASSERT(fequal(r.c4r2, -229485.f), info);
    TEST_ASSERT(fequal(r.c1r3, 86382.f), info);
    TEST_ASSERT(fequal(r.c2r3, 155688.f), info);
    TEST_ASSERT(fequal(r.c3r3, -35873.f), info);
    TEST_ASSERT(fequal(r.c4r3, 119808.f), info);
    TEST_ASSERT(fequal(r.c1r4, 122533.f), info);
    TEST_ASSERT(fequal(r.c2r4, 545133.f), info);
    TEST_ASSERT(fequal(r.c3r4, -417559.f), info);
    TEST_ASSERT(fequal(r.c4r4, -117911.f), info);

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

    float t = trace_mat4(m0);
    TEST_ASSERT(fequal(t, -9.f), info);

    r = inverse_mat4(m0);
    TEST_ASSERT(fequal(r.c1r1, -108.f / 133.f), info);
    TEST_ASSERT(fequal(r.c2r1, 240.f / 133.f), info);
    TEST_ASSERT(fequal(r.c3r1, 55.f / 133.f), info);
    TEST_ASSERT(fequal(r.c4r1, -115.f / 133.f), info);
    TEST_ASSERT(fequal(r.c1r2, 9.f / 19.f), info);
    TEST_ASSERT(fequal(r.c2r2, -20.f / 19.f), info);
    TEST_ASSERT(fequal(r.c3r2, -3.f / 19.f), info);
    TEST_ASSERT(fequal(r.c4r2, 8.f / 19.f), info);
    TEST_ASSERT(fequal(r.c1r3, 264.f / 665.f), info);
    TEST_ASSERT(fequal(r.c2r3, -498.f / 665.f), info);
    TEST_ASSERT(fequal(r.c3r3, -31.f / 665.f), info);
    TEST_ASSERT(fequal(r.c4r3, 222.f / 665.f), info);
    TEST_ASSERT(fequal(r.c1r4, 44.f / 665.f), info);
    TEST_ASSERT(fequal(r.c2r4, -83.f / 665.f), info);
    TEST_ASSERT(fequal(r.c3r4, -116.f / 665.f), info);
    TEST_ASSERT(fequal(r.c4r4, 37.f / 665.f), info);

    r = mul_mat4(m0, inverse_mat4(m0));
    TEST_ASSERT(fequal(r.c1r1, 1.f), info);
    TEST_ASSERT(fequal(r.c2r1, 0.f), info);
    TEST_ASSERT(fequal(r.c3r1, 0.f), info);
    TEST_ASSERT(fequal(r.c4r1, 0.f), info);
    TEST_ASSERT(fequal(r.c1r2, 0.f), info);
    TEST_ASSERT(fequal(r.c2r2, 1.f), info);
    TEST_ASSERT(fequal(r.c3r2, 0.f), info);
    TEST_ASSERT(fequal(r.c4r2, 0.f), info);
    TEST_ASSERT(fequal(r.c1r3, 0.f), info);
    TEST_ASSERT(fequal(r.c2r3, 0.f), info);
    TEST_ASSERT(fequal(r.c3r3, 1.f), info);
    TEST_ASSERT(fequal(r.c4r3, 0.f), info);
    TEST_ASSERT(fequal(r.c1r4, 0.f), info);
    TEST_ASSERT(fequal(r.c2r4, 0.f), info);
    TEST_ASSERT(fequal(r.c3r4, 0.f), info);
    TEST_ASSERT(fequal(r.c4r4, 1.f), info);
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
