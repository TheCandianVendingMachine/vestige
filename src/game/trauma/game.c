#include "game/trauma/game.h"
#include "game/game_states.h"

void gameplay_push(struct GameState* state) {
    state->stored_state = malloc(sizeof(GameplayState));

    GameplayState* s = (GameplayState*)state->stored_state;
    s->current_scene = (Scene) {
        .player = (Player) {
            .transform = new_transform()
        }
    };
}

void gameplay_pop(struct GameState* state) {
    free(state->stored_state);
}

void gameplay_update(struct GameState* state, float delta_time) {
    GameplayState* s = (GameplayState*)state->stored_state;
    update_scene(&s->current_scene, delta_time);
}

void gameplay_render(struct GameState* state) {
    GameplayState* s = (GameplayState*)state->stored_state;
    render_scene(&s->current_scene);
}

