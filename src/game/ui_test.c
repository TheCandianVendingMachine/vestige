#include <stdlib.h>
#include <math.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "game/ui_test.h"
#include "game/game_states.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_GAME
#include "logger.h"
#include "render/vertex.h"
#include "engine.h"

#include "render/primitives.h"

const char* get_ft_error_message(FT_Error err) {
    #undef FTERRORS_H_
    #define FT_ERRORDEF( e, v, s )  case e: return s;
    #define FT_ERROR_START_LIST     switch (err) {
    #define FT_ERROR_END_LIST       }
    #include FT_ERRORS_H
    return "(Unknown error)";
}

void init_font_engine(FontEngine* engine) {
    FT_Error error;
    error = FT_Init_FreeType(&engine->_library);
    if (error) {
        log_error("Cannot initialise Freetype: %s", get_ft_error_message(error));
        engine_crash(SHUTDOWN_CANT_INIT_FREETYPE);
    }
}

void load_font_file(FontEngine* engine, const char* path) {
    FT_Error error;
    FT_Face face;
    error = FT_New_Face(engine->_library, path, 0, &face);
    if (error) {
        log_warning("Cannot load font: %s", get_ft_error_message(error));
    }
}

void ui_test_push(GameState* state) {
    state->stored_state = malloc(sizeof(UiTestState));

    UiTestState* s = (UiTestState*)state->stored_state;

    init_font_engine(&s->font_engine);
    load_font_file(&s->font_engine, "/home/bailey/.fonts/RobotoMono/RobotoMonoNerdFont-Medium.ttf");

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
