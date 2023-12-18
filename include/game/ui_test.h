#ifndef VESTIGE_UI_TEST_H
#define VESTIGE_UI_TEST_H

#include "render/texture.h"
#include "render/shader.h"
#include "render/camera.h"
#include "lib/math.h"

typedef struct UiTestState {
    ShaderProgram test_shader;
    unsigned int vao;
    Matrix4f model;
    Camera view;
    Matrix4f projection;
} UiTestState;
struct GameState;

void ui_test_push(struct GameState* state);
void ui_test_pop(struct GameState* state);
void ui_test_update(struct GameState* state, float delta_time);
void ui_test_render(struct GameState* state);

#endif
