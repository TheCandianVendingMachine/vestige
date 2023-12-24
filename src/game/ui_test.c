#include <stdlib.h>
#include <math.h>

#include "game/ui_test.h"
#include "game/game_states.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_GAME
#include "logger.h"
#include "render/vertex.h"
#include "engine.h"
#include "lib/clock.h"

#include "render/primitives.h"
#include "ui/font.h"


void ui_test_push(GameState* state) {
    state->stored_state = malloc(sizeof(UiTestState));

    UiTestState* s = (UiTestState*)state->stored_state;

    s->font_engine = new_font_engine();

    int points[] = { 16, 32, 64 };
    load_font_from_disk(
        s->font_engine,
        (Vector2i) { .x = 1000, .y = 470 },
        "default", "/home/bailey/.fonts/RobotoMono/RobotoMonoNerdFont-Medium.ttf",
        sizeof(points) / sizeof(int), points
    );
    draw_font_atlas(s->font_engine, "default", "default_atlas.bmp");

    Shader vs = load_vertex_shader_from_disk("shaders/text_test_shader.vert").data.ok;
    Shader fs = load_fragment_shader_from_disk("shaders/text_test_shader.frag").data.ok;
    s->test_shader = create_shader_program(vs, fs).data.ok;

    s->projection = matrix_orthographic_projection(
        0.f, -1.f * ENGINE->window.size.x,
        0.f, ENGINE->window.size.y,
        -100.f, 100.f
    );
    s->view = new_camera();
    float sx = 500.f;
    float sy = 100.f;
    float tx = 1280 * 0.5f - sx * 0.5f;
    float ty = 720 * 0.5f - sy * 0.5f;
    float m[] = {
        sx, 0.f, 0.f, 0.f,
        0.f, sy, 0.f, 0.f,
        0.f, 0.f,0.f, 0.f,
        tx, ty,  0.f, 1.f
    };
    memcpy(s->model.entries, m, sizeof(s->model.entries));

    glGenVertexArrays(1, &s->vao);
    bind_primitive_to_vao(primitive_quad(), s->vao);
}

void ui_test_pop(GameState* state) {
    free(state->stored_state);
}

void ui_test_update(GameState* state, float delta_time) {
}

void ui_test_render(GameState* state) {
    UiTestState* s = (UiTestState*)state->stored_state;

    int projectionPosition = glGetUniformLocation(s->test_shader._program, "projection");
    int modelPosition = glGetUniformLocation(s->test_shader._program, "model");
    int viewPosition = glGetUniformLocation(s->test_shader._program, "view");

    glUseProgram(s->test_shader._program);
    glUniformMatrix4fv(projectionPosition, 1, false, s->projection.entries);
    glUniformMatrix4fv(modelPosition, 1, false, s->model.entries);
    glUniformMatrix4fv(viewPosition, 1, false, camera_view(&s->view).entries);
    draw_primitive(primitive_quad(), s->vao);
}
