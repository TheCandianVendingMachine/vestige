#ifndef VESTIGE_RENDER_TEST_H
#define VESTIGE_RENDER_TEST_H

struct GameState;

void render_test_push(struct GameState* state);
void render_test_pop(struct GameState* state);
void render_test_update(struct GameState* state, float delta_time);
void render_test_render(struct GameState* state);

#endif
