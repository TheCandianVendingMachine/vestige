#ifndef VESTIGE_TRAUMA_GAME_H
#define VESTIGE_TRAUMA_GAME_H

#include "render/texture.h"
#include "render/shader.h"
#include "render/camera.h"
#include "lib/math.h"

#include "ui/font_engine.h"
#include "ui/text.h"

typedef struct GameplayState {
    ShaderProgram test_shader;
    unsigned int vao;
    Matrix4f model;
    Camera view;
    Matrix4f projection;
    FontEngine font_engine;
    Text text;
} GameplayState;
struct GameState;

void gameplay_push(struct GameState* state);
void gameplay_pop(struct GameState* state);
void gameplay_update(struct GameState* state, float delta_time);
void gameplay_render(struct GameState* state);

#endif
