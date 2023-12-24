#include <freetype/freetype.h>

#include "glad/glad.h"
#include "ui/font.h"

void destroy_font(Font font) {
    FT_Done_Face(font._face);
    destroy_atlas(font.glyph_atlas);
    for (int i = 0; i < font.point_count; i++) {
        del_vector(font.points[i].glyphs);
    }
    free(font.points);
    del_hashmap(font.char_glyph_map);
    destroy_texture(font._texture);
}
