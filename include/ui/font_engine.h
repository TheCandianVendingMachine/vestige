#ifndef VESTIGE_FONT_ENGINE_H
#define VESTIGE_FONT_ENGINE_H

#include <freetype/freetype.h>

#include "lib/math.h"
#include "lib/hashmap.h"

typedef struct FontManager {
    HashMap font_map;
} FontManager;

typedef struct FontEngine {
    FT_Library _library;
    FontManager manager;
} FontEngine;

FontEngine new_font_engine(void);
void destroy_font_engine(FontEngine engine);

void load_font_from_disk(FontEngine engine, Vector2i atlas_size, const char* id, const char* path, int point_count, int* points);

void draw_font_atlas(FontEngine engine, const char* id, const char* out_path);

#endif
