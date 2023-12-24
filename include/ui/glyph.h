#ifndef VESTIGE_GLYPH_H
#define VESTIGE_GLYPH_H

#include "lib/math.h"

typedef struct Glyph {
    AABB bounds;
    Vector2f uv_coordinates;
    Vector2f uv_size;
} Glyph;

#endif
