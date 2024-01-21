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

float max_float(float a, float b) {
    return a > b ? a : b;
}

float min_float(float a, float b) {
    return a < b ? a : b;
}

float length_vector2f(Vector2f v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

float distance_vector2f(Vector2f lhs, Vector2f rhs) {
    return length_vector2f(sub_vector2f(lhs, rhs));
}

Vector2f normalise_vector2f(Vector2f v) {
    float length = length_vector2f(v);
    return (Vector2f) { {
        v.x / length,
        v.y / length
    } };
}

float dot_vector2f(Vector2f lhs, Vector2f rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

Vector2f add_vector2f(Vector2f lhs, Vector2f rhs) {
    return (Vector2f) { {
        lhs.x + rhs.x,
        lhs.y + rhs.y
    } };
}

Vector2f sub_vector2f(Vector2f lhs, Vector2f rhs) {
    return (Vector2f) { {
        lhs.x - rhs.x,
        lhs.y - rhs.y
    } };
}

Vector2f mul_vector2f(Vector2f lhs, float rhs) {
    return (Vector2f) { {
        lhs.x * rhs,
        lhs.y * rhs
    } };
}

Vector2f clamp_vector2f(Vector2f v, float magnitude) {
    float d2 = dot_vector2f(v, v);
    if (d2 > magnitude * magnitude) {
        return mul_vector2f(normalise_vector2f(v), magnitude);
    }
    return v;
}

float length_vector3f(Vector3f v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

float distance_vector3f(Vector3f lhs, Vector3f rhs) {
    return length_vector3f(sub_vector3f(lhs, rhs));
}

float dot_vector3f(Vector3f lhs, Vector3f rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

Vector3f normalise_vector3f(Vector3f v) {
    float length = length_vector3f(v);
    return (Vector3f) { {
        v.x / length,
        v.y / length,
        v.z / length
    } };
}

Vector3f cross_vector3f(Vector3f lhs, Vector3f rhs) {
    return (Vector3f) { {
        lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.z * rhs.x - lhs.x * rhs.z,
        lhs.x * rhs.y - lhs.y * rhs.x
    } };
}

Vector3f add_vector3f(Vector3f lhs, Vector3f rhs) {
    return (Vector3f) { {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z
    } };
}

Vector3f sub_vector3f(Vector3f lhs, Vector3f rhs) {
    return (Vector3f) { {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z
    } };
}

Vector3f mul_vector3f(Vector3f lhs, float rhs) {
    return (Vector3f) { {
        lhs.x * rhs,
        lhs.y * rhs,
        lhs.z * rhs
    } };
}

Vector3f clamp_vector3f(Vector3f v, float magnitude) {
    float d2 = dot_vector3f(v, v);
    if (d2 > magnitude * magnitude) {
        return mul_vector3f(normalise_vector3f(v), magnitude);
    }
    return v;
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

float aabb_area(AABB aabb) {
    return aabb.size.x * aabb.size.y;
}

bool aabb_intersect(AABB a, AABB b) {
    return  (a.position.x + a.size.x > b.position.x && a.position.x < b.position.x + b.size.x) &&
            (a.position.y + a.size.y > b.position.y && a.position.y < b.position.y + b.size.y);
}
