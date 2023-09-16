#include "game/game_states.h"
#include "logger.h"

void game_state_on_init(GameState* state) {
    switch (state->state_type) {
        default:
            break;
    }
}

void game_state_on_deinit(GameState* state) {
    switch (state->state_type) {
        default:
            break;
    }
}

void game_state_on_push(GameState* state) {
    switch (state->state_type) {
        default:
            break;
    }
}

void game_state_on_pop(GameState* state) {
    switch (state->state_type) {
        default:
            break;
    }

}

void game_state_update(GameState* state, float delta_time) {
    switch (state->state_type) {
        default:
            break;
    }

}

void game_state_render(GameState* state) {
    switch (state->state_type) {
        default:
            break;
    }

}
