#include <lib/math.h>

#include "test_info.h"

void test_vec2(TestInfo* info) {
    Vector2f v0;
    v0.x = 1.f;
    v0.y = 4.f;

    Vector2f v1;
    v1.x = -4.f;
    v1.y = 12.f;

    TEST_ASSERT(fequal(length_vector2f(v0), 4.123105f), info);
    TEST_ASSERT(fequal(distance_vector2f(v0, v1), distance_vector2f(v1, v0)), info);
    TEST_ASSERT(fequal(distance_vector2f(v0, v1), 9.4339811f), info);
    TEST_ASSERT(fequal(dot_vector2f(v0, v1), 44.f), info);

    Vector2f r = normalise_vector2f(v0);
    TEST_ASSERT(fequal(r.x, 0.242535f), info);
    TEST_ASSERT(fequal(r.y, 0.970142f), info);
}

void test_vec(void* complete) {
    TestInfo* info = (TestInfo*)complete;
    test_vec2(info);

    if (info->status == INCONCLUSIVE) {
        info->status = SUCCESS;
    }
}
