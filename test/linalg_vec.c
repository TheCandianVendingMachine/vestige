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

    r = add_vector2f(v0, v1);
    TEST_ASSERT(fequal(r.x, -3.f), info);
    TEST_ASSERT(fequal(r.y, 16.f), info);

    r = sub_vector2f(v0, v1);
    TEST_ASSERT(fequal(r.x, 5.f), info);
    TEST_ASSERT(fequal(r.y, -8.f), info);

    r = mul_vector2f(v0, 2.f);
    TEST_ASSERT(fequal(r.x, 2.f), info);
    TEST_ASSERT(fequal(r.y, 8.f), info);

    r = mul_vector2f_pair(v0, v1);
    TEST_ASSERT(fequal(r.x, -4.f), info);
    TEST_ASSERT(fequal(r.y, 48.f), info);

    r = clamp_vector2f(v0, 2.f);
    TEST_ASSERT(fequal(r.x, 2.f * 0.242535f), info);
    TEST_ASSERT(fequal(r.y, 2.f * 0.970142f), info);
}

void test_vec3(TestInfo* info) {
    Vector3f v0;
    v0.x = 1.f;
    v0.y = 4.f;
    v0.z = 3.f;

    Vector3f v1;
    v1.x = -4.f;
    v1.y = 12.f;
    v1.z = 8.f;

    TEST_ASSERT(fequal(length_vector3f(v0), 5.099019f), info);
    TEST_ASSERT(fequal(distance_vector3f(v0, v1), distance_vector3f(v1, v0)), info);
    TEST_ASSERT(fequal(distance_vector3f(v0, v1), 10.67707f), info);
    TEST_ASSERT(fequal(dot_vector3f(v0, v1), 68.f), info);

    Vector3f r = normalise_vector3f(v0);
    TEST_ASSERT(fequal(r.x, 0.196116f), info);
    TEST_ASSERT(fequal(r.y, 0.784464f), info);
    TEST_ASSERT(fequal(r.z, 0.588348f), info);

    r = cross_vector3f(v0, v1);
    TEST_ASSERT(fequal(r.x, -4.f), info);
    TEST_ASSERT(fequal(r.y, -20.f), info);
    TEST_ASSERT(fequal(r.z, 28.f), info);

    r = add_vector3f(v0, v1);
    TEST_ASSERT(fequal(r.x, -3.f), info);
    TEST_ASSERT(fequal(r.y, 16.f), info);
    TEST_ASSERT(fequal(r.z, 11.f), info);

    r = sub_vector3f(v0, v1);
    TEST_ASSERT(fequal(r.x, 5.f), info);
    TEST_ASSERT(fequal(r.y, -8.f), info);
    TEST_ASSERT(fequal(r.z, -5.f), info);

    r = mul_vector3f(v0, 2.f);
    TEST_ASSERT(fequal(r.x, 2.f), info);
    TEST_ASSERT(fequal(r.y, 8.f), info);
    TEST_ASSERT(fequal(r.z, 6.f), info);

    r = mul_vector3f_pair(v0, v1);
    TEST_ASSERT(fequal(r.x, -4.f), info);
    TEST_ASSERT(fequal(r.y, 48.f), info);
    TEST_ASSERT(fequal(r.z, 24.f), info);

    r = clamp_vector3f(v0, 2.f);
    TEST_ASSERT(fequal(r.x, 2.f * 0.196116f), info);
    TEST_ASSERT(fequal(r.y, 2.f * 0.784464f), info);
    TEST_ASSERT(fequal(r.z, 2.f * 0.588348f), info);
}

void test_vec4(TestInfo* info) {
    Vector4f v0;
    v0.x = 1.f;
    v0.y = 4.f;
    v0.z = 3.f;
    v0.w = 4.f;

    Vector4f v1;
    v1.x = -4.f;
    v1.y = 12.f;
    v1.z = 8.f;
    v1.w = -8.f;

    TEST_ASSERT(fequal(length_vector4f(v0), 6.48074f), info);
    TEST_ASSERT(fequal(distance_vector4f(v0, v1), distance_vector4f(v1, v0)), info);
    TEST_ASSERT(fequal(distance_vector4f(v0, v1), 16.0623784f), info);
    TEST_ASSERT(fequal(dot_vector4f(v0, v1), 36.f), info);

    Vector4f r = normalise_vector4f(v0);
    TEST_ASSERT(fequal(r.x, 0.154303f), info);
    TEST_ASSERT(fequal(r.y, 0.617213f), info);
    TEST_ASSERT(fequal(r.z, 0.462910f), info);
    TEST_ASSERT(fequal(r.w, 0.617213f), info);

    r = add_vector4f(v0, v1);
    TEST_ASSERT(fequal(r.x, -3.f), info);
    TEST_ASSERT(fequal(r.y, 16.f), info);
    TEST_ASSERT(fequal(r.z, 11.f), info);
    TEST_ASSERT(fequal(r.w, -4.f), info);

    r = sub_vector4f(v0, v1);
    TEST_ASSERT(fequal(r.x, 5.f), info);
    TEST_ASSERT(fequal(r.y, -8.f), info);
    TEST_ASSERT(fequal(r.z, -5.f), info);
    TEST_ASSERT(fequal(r.w, 12.f), info);

    r = mul_vector4f(v0, 2.f);
    TEST_ASSERT(fequal(r.x, 2.f), info);
    TEST_ASSERT(fequal(r.y, 8.f), info);
    TEST_ASSERT(fequal(r.z, 6.f), info);
    TEST_ASSERT(fequal(r.w, 8.f), info);

    r = mul_vector4f_pair(v0, v1);
    TEST_ASSERT(fequal(r.x, -4.f), info);
    TEST_ASSERT(fequal(r.y, 48.f), info);
    TEST_ASSERT(fequal(r.z, 24.f), info);
    TEST_ASSERT(fequal(r.w, -32.f), info);

    r = clamp_vector4f(v0, 2.f);
    TEST_ASSERT(fequal(r.x, 2.f * 0.154303f), info);
    TEST_ASSERT(fequal(r.y, 2.f * 0.617213f), info);
    TEST_ASSERT(fequal(r.z, 2.f * 0.462910f), info);
    TEST_ASSERT(fequal(r.w, 2.f * 0.617213f), info);
}

void test_vec(void* complete) {
    TestInfo* info = (TestInfo*)complete;
    test_vec2(info);
    test_vec3(info);
    test_vec4(info);

    if (info->status == INCONCLUSIVE) {
        info->status = SUCCESS;
    }
}
