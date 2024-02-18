#include <math.h>

#include <lib/math.h>

struct XorShift128State {
    uint32_t x[4];
};

static struct XorShift128State RANDSTATE = {{2465962, 17748479, 215684, 3137379}};

static uint32_t xorshift128(struct XorShift128State* state) {
    uint32_t t = state->x[3];

    uint32_t s = state->x[0];
    state->x[3] = state->x[2];
    state->x[2] = state->x[1];
    state->x[1] = s;

    t ^= t << 11;
    t ^= t >> 8;
    return state->x[0] = t ^ s ^ (s << 19);
}

void setseed(uint32_t seed[4]) {
    memcpy(RANDSTATE.x, seed, 4 * sizeof(uint32_t));
}

int randint(void) {
    return (int)xorshift128(&RANDSTATE);
}

int irandrange(int start, int end) {
    return start + (abs(randint()) % (end - start));
}

float frandrange(float start, float end) {
    return start + (end - start) * ((float)abs(randint()) / (float)(0x8fffffff));
}

bool fequal(float lhs, float rhs) {
    return fabsf(lhs - rhs) <= 0.0001f;
}

int abs(int i) {
    return i < 0 ? -i : i;
}

float max_float(float a, float b) {
    return a > b ? a : b;
}

float min_float(float a, float b) {
    return a < b ? a : b;
}

float aabb_area(AABB aabb) {
    return aabb.size.x * aabb.size.y;
}

bool aabb_intersect(AABB a, AABB b) {
    return  (a.position.x + a.size.x > b.position.x && a.position.x < b.position.x + b.size.x) &&
            (a.position.y + a.size.y > b.position.y && a.position.y < b.position.y + b.size.y);
}

AABB aabb_from_points(Vector2f* points, size_t count) {
    Vector2f min = points[0];
    Vector2f max = points[0];
    for (size_t i = 1; i < count; i++) {
        Vector2f p = points[i];
        max.x = max_float(max.x, p.x);
        max.y = max_float(max.y, p.y);
        min.x = min_float(min.x, p.x);
        min.y = min_float(min.y, p.y);
    }

    return (AABB) {
        .position = min,
        .size = max
    };
}

Circle circle_from_points(Vector2f* points, size_t count) {
    Vector2f center = (Vector2f) { .x = 0.f, .y = 0.f };
    for (size_t i = 0; i < count; i++) {
        center = add_vector2f(center, points[i]);
    }
    center = mul_vector2f(center, 1.f / (float)count);

    float max_distance = 0.f;
    for (size_t i = 0; i < count; i++) {
        max_distance = max_float(max_distance, length2_vector2f(sub_vector2f(center, points[i])));
    }

    return (Circle) {
        .position = center,
        .radius = sqrt(max_distance)
    };
}
