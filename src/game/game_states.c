#include "game/game_states.h"
#include "logger.h"

#include "game/render_test.h"

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
        case GAME_STATE_RENDER_TEST:
            render_test_push(state);
        default:
            break;
    }
}

void game_state_on_pop(GameState* state) {
    switch (state->state_type) {
        case GAME_STATE_RENDER_TEST:
            render_test_pop(state);
        default:
            break;
    }

}

void game_state_update(GameState* state, float delta_time) {
    switch (state->state_type) {
        case GAME_STATE_RENDER_TEST:
            render_test_update(state, delta_time);
        default:
            break;
    }

}

void game_state_render(GameState* state) {
    switch (state->state_type) {
        case GAME_STATE_RENDER_TEST:
            render_test_render(state);
        default:
            break;
    }

}
