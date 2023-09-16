#ifndef VESTIGE_GAME_STATE_H
#define VESTIGE_GAME_STATE_H

typedef enum GameStateEnum {
    GAME_STATE_RENDER_TEST,
    GAME_STATE_TOMBSTONE = ~0,
} GameStateEnum;

typedef struct GameState {
    GameStateEnum state_type;
    // The game state owns this and can store whatever it wants at this pointer
    void* stored_state;
} GameState;

void game_state_on_init(GameState* state);
void game_state_on_deinit(GameState* state);
void game_state_on_push(GameState* state);
void game_state_on_pop(GameState* state);
void game_state_update(GameState* state, float delta_time);
void game_state_render(GameState* state);

#endif
