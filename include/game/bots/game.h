#ifndef BOTS_TRAUMA_GAME_H
#define BOTS_TRAUMA_GAME_H

#include "render/texture.h"
#include "render/shader.h"
#include "render/camera.h"
#include "lib/math.h"

#include "ui/font_engine.h"
#include "ui/text.h"

#include "game/bots/scene.h"

typedef struct GameplayState {
    Scene current_scene;
} GameplayState;
struct GameState;

void gameplay_push(struct GameState* state);
void gameplay_pop(struct GameState* state);
void gameplay_update(struct GameState* state, float delta_time);
void gameplay_render(struct GameState* state);

#endif
