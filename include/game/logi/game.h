#ifndef LOGI_GAME_H
#define LOGI_GAME_H

#include "render/texture.h"
#include "render/shader.h"
#include "render/camera.h"
#include "lib/math.h"

typedef struct GameplayState {
    Camera camera;
    Matrix4f projection;
} GameplayState;
struct GameState;

void game_push(struct GameState* state);
void game_pop(struct GameState* state);
void game_update(struct GameState* state);
void game_fixed_update(struct GameState* state, float delta_time);
void game_render(struct GameState* state);

#endif
