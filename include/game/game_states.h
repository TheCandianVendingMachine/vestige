#ifndef VESTIGE_GAME_STATE_H
#define VESTIGE_GAME_STATE_H

typedef enum GameState {
    GAME_STATE_TOMBSTONE,
    GAME_STATE_RENDER_TEST
} GameState;

void game_state_on_init(GameState state);
void game_state_on_deinit(GameState state);
void game_state_on_push(GameState state);
void game_state_on_pop(GameState state);
void game_state_update(GameState state, float delta_time);
void game_state_render(GameState state);

#endif
