#ifndef BOTS_TRAUMA_GAME_H
#define BOTS_TRAUMA_GAME_H

#include "lib/math.h"
#include "game/bots/scene.h"
#include "ui/font_engine.h"

typedef struct GameplayState {
    Scene current_scene;
    Matrix4f projection;
    FontEngine font_engine;
} GameplayState;
struct GameState;

void gameplay_push(struct GameState* state);
void gameplay_pop(struct GameState* state);
void gameplay_update(struct GameState* state);
void gameplay_fixed_update(struct GameState* state, float delta_time);
void gameplay_render(struct GameState* state);

#endif
