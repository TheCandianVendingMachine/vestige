#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "engine.h"
#include "game/bots/game.h"
#include "game/game_states.h"

#include "input/input_manager.h"

#include "logger.h"

void test_press(void* user_data, InputData data) {
    log_debug("press");
}

void test_release(void* user_data, InputData data) {
    log_debug("release");
}

void test_hold(void* user_data, InputData data) {
    log_debug("hold");
}

void test_double(void* user_data, InputData data) {
    log_debug("double");
}

void test_scroll(void* user_data, InputData data) {
    log_debug("scroll %f", data.mouse.scroll);
}

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

    register_key_action(&ENGINE->inputs, "test_action", (Key) { .key = GLFW_KEY_SPACE, .scancode = 0 });
    register_mouse_action(&ENGINE->inputs, "test_action", (Button) { .button = GLFW_MOUSE_BUTTON_LEFT });
    register_scroll_action(&ENGINE->inputs, "test_action");
    register_action_event(&ENGINE->inputs, "test_action", (InputEvent) { .on_press = test_press, .on_hold = test_hold, .on_double_press = test_double, .on_release = test_release, .on_scroll = test_scroll });
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

