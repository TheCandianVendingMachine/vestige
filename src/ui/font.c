#include <freetype/freetype.h>

#include "glad/glad.h"
#include "ui/font.h"

void destroy_font(Font font) {
    FT_Done_Face(font._face);
    destroy_atlas(font.glyph_atlas);
    del_vector(font.glyphs);
    del_hashmap(font.char_glyph_map);
    destroy_texture(font._texture);
}
