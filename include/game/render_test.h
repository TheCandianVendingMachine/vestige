#ifndef VESTIGE_RENDER_TEST_H
#define VESTIGE_RENDER_TEST_H

#include "render/texture.h"
#include "render/shader.h"
#include "lib/math.h"

typedef struct RenderTestState {
    Texture test_texture;
    ShaderProgram test_shader;
    unsigned int vbo;
    unsigned int vao;
    Matrix4f projection;
} RenderTestState;
struct GameState;

void render_test_push(struct GameState* state);
void render_test_pop(struct GameState* state);
void render_test_update(struct GameState* state, float delta_time);
void render_test_render(struct GameState* state);

#endif
