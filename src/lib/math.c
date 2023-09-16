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
