#ifndef VESTIGE_FONT_H
#define VESTIGE_FONT_H

#include <freetype/freetype.h>

#include "lib/hashmap.h"
#include "lib/vector.h"
#include "render/texture.h"
#include "atlas.h"

typedef struct Font {
    FT_Face _face;
    Texture _texture;
    Atlas glyph_atlas;
    Vector glyphs;
    HashMap char_glyph_map;
} Font;

void destroy_font(Font font);

#endif
