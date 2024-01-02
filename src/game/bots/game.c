#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "engine.h"
#include "game/bots/game.h"
#include "game/game_states.h"

void gameplay_push(struct GameState* state) {
    state->stored_state = malloc(sizeof(GameplayState));

    GameplayState* s = (GameplayState*)state->stored_state;
    s->font_engine = new_font_engine();
    s->current_scene = (Scene) {
        .world = new_world(&s->font_engine),
        .camera = new_camera()
    };
    s->projection = matrix_orthographic_projection(
        0.f, -1.f * ENGINE->window.size.x,
        0.f, ENGINE->window.size.y,
        -1.f, 1.f
    );

    int points[] = { 32 };
    load_font_from_disk(
        s->font_engine, (Vector2i) { .x = 500, .y = 500 },
        "default", "assets/helios.ttf",
        sizeof(points) / sizeof(points[0]), points
    );
}

void gameplay_pop(struct GameState* state) {
    GameplayState* s = (GameplayState*)state->stored_state;

    destroy_font_engine(s->font_engine);
    destroy_resource_map(&s->current_scene.world.resources);

    free(s);
}

void gameplay_update(struct GameState* state, float delta_time) {
    GameplayState* s = (GameplayState*)state->stored_state;
    update_scene(&s->current_scene, delta_time);
}

void gameplay_render(struct GameState* state) {
    GameplayState* s = (GameplayState*)state->stored_state;
    render_scene(s, &s->current_scene);
}

