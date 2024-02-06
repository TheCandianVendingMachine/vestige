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
}

void test_mat(void* complete) {
    TestInfo* info = (TestInfo*)complete;
    test_mat2(info);

    if (info->status == INCONCLUSIVE) {
        info->status = SUCCESS;
    }
}
