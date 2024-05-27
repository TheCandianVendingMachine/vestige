#include <stdlib.h>
#include <math.h>
#include <assert.h>

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

float fsign(float a) {
    if (a == 0.f) { return 0.f; }
    if (a == -0.f) { return 0.f; }
    return a / fabs(a);
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

int64_t max_sint(int64_t a, int64_t b) {
    return a > b ? a : b;
}

int64_t min_sint(int64_t a, int64_t b) {
    return a < b ? a : b;
}

uint64_t max_uint(uint64_t a, uint64_t b) {
    return a > b ? a : b;
}

uint64_t min_uint(uint64_t a, uint64_t b) {
    return a < b ? a : b;
}

uint8_t log2int(uint64_t x) {
    return 8 * sizeof(x) - 1 - __builtin_clzll(x);
}

float aabb_area(ShapeAABB aabb) {
    return aabb.size.x * aabb.size.y;
}

bool aabb_intersect(ShapeAABB a, ShapeAABB b) {
    return  (a.position.x + a.size.x > b.position.x && a.position.x < b.position.x + b.size.x) &&
            (a.position.y + a.size.y > b.position.y && a.position.y < b.position.y + b.size.y);
}

ShapeAABB aabb_from_points(Vector2f* points, size_t count) {
    Vector2f min = points[0];
    Vector2f max = points[0];
    for (size_t i = 1; i < count; i++) {
        Vector2f p = points[i];
        max.x = max_float(max.x, p.x);
        max.y = max_float(max.y, p.y);
        min.x = min_float(min.x, p.x);
        min.y = min_float(min.y, p.y);
    }

    return (ShapeAABB) {
        .position = min,
        .size = max
    };
}

ShapeCircle circle_from_points(Vector2f* points, size_t count) {
    Vector2f center = (Vector2f) { .x = 0.f, .y = 0.f };
    for (size_t i = 0; i < count; i++) {
        center = add_vector2f(center, points[i]);
    }
    center = mul_vector2f(center, 1.f / (float)count);

    float max_distance = 0.f;
    for (size_t i = 0; i < count; i++) {
        max_distance = max_float(max_distance, length2_vector2f(sub_vector2f(center, points[i])));
    }

    return (ShapeCircle) {
        .position = center,
        .radius = sqrt(max_distance)
    };
}

ShapePolygon convex_hull_from_points(Vector2f* points, size_t count) {
    ShapePolygon hull;
    hull.points = malloc(sizeof(Vector2f) * count);

    Vector2f point_on_hull = points[0];
    for (size_t i = 1; i < count; i++) {
        point_on_hull.x = min_float(point_on_hull.x, points[i].x);
    }

    for (size_t i = 0; i < count; i++) {
        hull.points[i] = point_on_hull;
        hull.point_count += 1;
        Vector2f endpoint = points[0];
        for (size_t j = 0; j < count; j++) {
            float a_test = dot_vector2f(normalise_vector2f(point_on_hull), normalise_vector2f(endpoint));
            float a_comp = dot_vector2f(normalise_vector2f(point_on_hull), normalise_vector2f(points[j]));
            if (
                (fequal(endpoint.x, point_on_hull.x) && fequal(endpoint.y, point_on_hull.y)) ||
                a_test < a_comp
            ) {
                endpoint = points[j];
            }
        }
        point_on_hull = endpoint;
        if (fequal(endpoint.x, points[0].x) && fequal(endpoint.y, points[0].y)) {
            break;
        }
    }

    return hull;
}

bool polygon_is_convex(ShapePolygon polygon) {
    if (polygon.point_count < 3) {
        return false;
    } else if (polygon.point_count == 3) {
        return true;
    }

    Vector2f old_point = polygon.points[polygon.point_count - 2];
    Vector2f new_point = polygon.points[polygon.point_count - 1];
    float new_direction = atan2f(new_point.y - old_point.y, new_point.x - old_point.x);
    float angle_sum = 0.f;
    float orientation;

    for (int i = 0; i < polygon.point_count; i++) {
        old_point = new_point;
        float old_direction = new_direction;
        new_point = polygon.points[i];
        new_direction = atan2f(new_point.y - old_point.y, new_point.x - old_point.x);
        if (fequal(old_point.x, new_point.x) && fequal(old_point.y, new_point.y)) {
            // repeated points => non convex
            return false;
        }
        float angle = new_direction - old_direction;
        if (angle <= -CONSTANT_PI) {
            angle += CONSTANT_TAU;
        } else if (angle > CONSTANT_PI) {
            angle -= CONSTANT_TAU;
        }

        if (i == 0) {
            if (angle == 0.f) {
                return false;
            }
            orientation = 1.f * fsign(angle);
        } else {
            if (orientation * angle <= 0.f) {
                return false;
            }
        }

        angle_sum += angle;
    }

    return fequal(fabs(roundf(angle_sum / CONSTANT_TAU)), 1.f);
}

ShapeTriangulatedPolygon triangulated_polygon_from_polygon(ShapePolygon polygon) {
    // Not implemented
    assert(false);
}
