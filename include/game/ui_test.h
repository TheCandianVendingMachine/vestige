#ifndef VESTIGE_UI_TEST_H
#define VESTIGE_UI_TEST_H

#include "render/texture.h"
#include "render/shader.h"
#include "render/camera.h"
#include "lib/math.h"

#include <freetype/freetype.h>

#include "lib/hashmap.h"
#include "lib/vector.h"
#include "lib/math.h"

typedef struct Glyph {
    AABB bounds;
} Glyph;

typedef struct Font {
    FT_Face _face;
    Vector glyphs;
    HashMap glyph_map;
} Font;

typedef struct FontManager {
    HashMap font_map;
} FontManager;

typedef struct FontEngine {
    FT_Library _library;
    FontManager manager;
} FontEngine;

typedef struct UiTestState {
    ShaderProgram test_shader;
    unsigned int vao;
    Matrix4f model;
    Camera view;
    Matrix4f projection;
    FontEngine font_engine;
} UiTestState;
struct GameState;

void ui_test_push(struct GameState* state);
void ui_test_pop(struct GameState* state);
void ui_test_update(struct GameState* state, float delta_time);
void ui_test_render(struct GameState* state);

#endif
