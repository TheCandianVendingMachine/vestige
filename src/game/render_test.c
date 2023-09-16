#include "game/render_test.h"
#include "game/game_states.h"
#include "logger.h"

void render_test_push(GameState* state) {
    log_debug("Push!");
}

void render_test_pop(GameState* state) {
    log_debug("Pop!");
}

void render_test_update(GameState* state, float delta_time) {
    log_debug("Update!");
}

void render_test_render(GameState* state) {
    log_debug("Render!");
}
