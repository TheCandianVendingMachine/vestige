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

int abs(int i) {
    return i < 0 ? -i : i;
}

float length_vector2f(Vector2f v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

Vector2f normalise_vector2f(Vector2f v) {
    float length = length_vector2f(v);
    return (Vector2f) {
        v.x / length,
        v.y / length
    };
}

Vector2f add_vector2f(Vector2f lhs, Vector2f rhs) {
    return (Vector2f) {
        lhs.x + rhs.x,
        lhs.y + rhs.y
    };
}

Vector2f sub_vector2f(Vector2f lhs, Vector2f rhs) {
    return (Vector2f) {
        lhs.x - rhs.x,
        lhs.y - rhs.y
    };
}

Vector2f mul_vector2f(Vector2f lhs, float rhs) {
    return (Vector2f) {
        lhs.x * rhs,
        lhs.y * rhs
    };
}

float length_vector3f(Vector3f v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vector3f normalise_vector3f(Vector3f v) {
    float length = length_vector3f(v);
    return (Vector3f) {
        v.x / length,
        v.y / length,
        v.z / length
    };
}

Matrix4f matrix_orthographic_projection(float left, float right, float top, float bottom, float near, float far) {
    Matrix4f projection;
    memset(projection.entries, 0, sizeof(projection.entries));
    projection.c1r1 = 2.f / (right - left);
    projection.c2r2 = 2.f / (top - bottom);
    projection.c3r3 = -2.f / (far - near);
    projection.c4r1 = -(right + left) / (right - left);
    projection.c4r2 = -(top + bottom) / (top - bottom);
    projection.c4r3 = -(far + near) / (far - near);
    projection.c4r4 = 1.f;
    return projection;
}
