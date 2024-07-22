#include "game/game_states.h"
#include "logger.h"

#include "game/logi/game.h"

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
        case GAME_STATE_GAMEPLAY:
            game_push(state);
            break;
        default:
            break;
    }
}

void game_state_on_pop(GameState* state) {
    switch (state->state_type) {
        case GAME_STATE_GAMEPLAY:
            game_pop(state);
            break;
        default:
            break;
    }

}

void game_state_update(GameState* state) {
    switch (state->state_type) {
        case GAME_STATE_GAMEPLAY:
            game_update(state);
            break;
        default:
            break;
    }
}

void game_state_fixed_update(GameState* state, float delta_time) {
    switch (state->state_type) {
        case GAME_STATE_GAMEPLAY:
            game_fixed_update(state, delta_time);
            break;
        default:
            break;
    }
}

void game_state_post_update(GameState* state) {
    switch (state->state_type) {
        case GAME_STATE_UI_TEST:
            break;
        case GAME_STATE_RENDER_TEST:
            break;
        default:
            break;
    }

}

void game_state_render(GameState* state) {
    switch (state->state_type) {
        case GAME_STATE_GAMEPLAY:
            game_render(state);
            break;
        default:
            break;
    }

}
