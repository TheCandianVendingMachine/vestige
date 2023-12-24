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

    int points[] = { 72 };
    load_font_from_disk(
        s->font_engine,
        (Vector2i) { .x = 1200, .y = 520 },
        "default", "/home/bailey/.fonts/RobotoMono/RobotoMonoNerdFont-Medium.ttf",
        sizeof(points) / sizeof(int), points
    );

    Shader vs = load_vertex_shader_from_disk("shaders/text_test_shader.vert").data.ok;
    Shader fs = load_fragment_shader_from_disk("shaders/text_test_shader.frag").data.ok;
    s->test_shader = create_shader_program(vs, fs).data.ok;

    s->projection = matrix_orthographic_projection(
        0.f, -1.f * ENGINE->window.size.x,
        0.f, ENGINE->window.size.y,
        -100.f, 100.f
    );
    s->view = new_camera();
    float sx = 0.2f;
    float sy = 0.2f;
    float tx = 1280 * 0.02f - sx * 0.5f;
    float ty = 720 * 0.05f - sy * 0.5f;
    float m[] = {
        sx, 0.f, 0.f, 0.f,
        0.f, sy, 0.f, 0.f,
        0.f, 0.f,0.f, 0.f,
        tx, ty,  0.f, 1.f
    };
    memcpy(s->model.entries, m, sizeof(s->model.entries));

    glGenVertexArrays(1, &s->vao);
    bind_primitive_to_vao(primitive_quad(), s->vao);

    s->text = create_text(get_font(s->font_engine, "default"));
    text_set_string(&s->text, "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Nulla varius. Aenean tristique\nsem sed metus. Quisque orci turpis, euismod semper, volutpat sed, sagittis id, purus.\nPellentesque molestie. Nam vitae odio. Suspendisse elit magna, dapibus sed, fermentum vel,\nelementum eget, nulla. Integer eget ligula. Pellentesque erat. Proin elit mauris, semper\neu, feugiat sed, rhoncus et, wisi. Nulla pede ipsum_ornare eget, porttitor vel, nonummy\nac, nibh. Integer consectetuer faucibus dui. Vestibulum venenatis feugiat wisi. Praesent\nultricies. Ut aliquet ligula at dolor. In hac habitasse platea dictumst. In est nibh,\nelementum et, malesuada nec, semper vitae, tortor.\n\tSed est sem-molestie in, ultricies\nsit amet, varius non, lorem. Praesent eget dolor. Nullam sed purus eu diam venenatis\nullamcorper. Fusce semper nisl vel lectus fermentum aliquet. Nullam sem. Ut ultrices\nplacerat felis. Curabitur pulvinar. Integer egestas. Nam quam sem, tincidunt id, lacinia\nac, pharetra sit amet, tellus. Pellentesque eros justo, rutrum in, euismod eget, mollis a,\nmassa. Sed pretium mi a enim. Maecenas gravida. Suspendisse purus risus, consectetuer sed,\nvestibulum ut^bibendum vel, ligula $123.\n\tDonec et erat. In eget nunc eu ipsum commodo mattis. Mauris semper. Aliquam erat volutpat.\nNullam non sem nec augue convallis porta. Fusce interdum-quam quis pede; suspendisse\nadipiscing. In est. Cras risus mauris, commodo a, egestas non, pretium ut, risus. Nulla\nauctor volutpat augue. Donec wisi. Suspendisse quam orci, posuere et, volutpat quis,\ncommodo sit amet, lectus. Aliquam erat volutpat. Donec dignissim. Sed pretium arcu sit\namet dolor. Aenean mattis urna et nulla. Aenean tellus. Aliquam erat volutpat. Maecenas\nnec diam semper diam luctus commodo.\n\nThe Quick Brown Fox Jumps Over The Lazy Dog 0123456789");
    s->text._render_info.point = 0;

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
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
    draw_text(&s->text);
}
