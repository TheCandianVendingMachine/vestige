#include "game/game_states.h"
#include "logger.h"

#include "game/render_test.h"
#include "game/zombie.h"

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
            log_debug("RenderTest on push");
            break;
        case GAME_STATE_ZOMBIE:
            zombie_push(state);
            log_debug("Zombie on push");
            break;
        default:
            break;
    }
}

void game_state_on_pop(GameState* state) {
    switch (state->state_type) {
        case GAME_STATE_RENDER_TEST:
            render_test_pop(state);
            log_debug("RenderTest on pop");
            break;
        case GAME_STATE_ZOMBIE:
            zombie_pop(state);
            log_debug("Zombie on pop");
            break;
        default:
            break;
    }

}

void game_state_update(GameState* state, float delta_time) {
    switch (state->state_type) {
        case GAME_STATE_RENDER_TEST:
            render_test_update(state, delta_time);
            break;
        case GAME_STATE_ZOMBIE:
            zombie_update(state, delta_time);
            break;
        default:
            break;
    }

}

void game_state_render(GameState* state) {
    switch (state->state_type) {
        case GAME_STATE_RENDER_TEST:
            render_test_render(state);
            break;
        case GAME_STATE_ZOMBIE:
            zombie_render(state);
            break;
        default:
            break;
    }

}
