#ifndef VESTIGE_GLYPH_H
#define VESTIGE_GLYPH_H

#include "lib/math.h"

typedef struct GlyphMetrics {
    Vector2f bearing;
    float advance;
} GlyphMetrics;

typedef struct Glyph {
    ShapeAABB bounds;
    Vector2f uv_coordinates;
    Vector2f uv_size;
    GlyphMetrics horizontal_metrics;
    GlyphMetrics vertical_metrics;
} Glyph;

#endif
