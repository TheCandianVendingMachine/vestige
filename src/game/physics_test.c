#include <stdlib.h>

#include "engine.h"
#include "game/game_states.h"
#include "game/physics_test.h"

void physics_push(struct GameState* state) {
    state->stored_state = malloc(sizeof(PhysicsTestState));

    PhysicsTestState* s = (PhysicsTestState*)state->stored_state;
    s->projection = matrix_orthographic_projection(
        0.f, -1.f * ENGINE->window.size.x,
        0.f, ENGINE->window.size.y,
        -1.f, 1.f
    );

    s->renderer = new_debug_renderer();
}

void physics_pop(struct GameState* state) {
    PhysicsTestState* s = (PhysicsTestState*)state->stored_state;

    free(s);
}

void physics_update(struct GameState* state) {
    //PhysicsTestState* s = (PhysicsTestState*)state->stored_state;
}

void physics_fixed_update(struct GameState* state, float delta_time) {
    //PhysicsTestState* s = (PhysicsTestState*)state->stored_state;
}

void physics_render(struct GameState* state) {
    PhysicsTestState* s = (PhysicsTestState*)state->stored_state;
    draw_debug(&s->renderer, s->camera, s->projection);
}
