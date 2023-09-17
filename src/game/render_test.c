#include <stdlib.h>

#include "game/render_test.h"
#include "game/game_states.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_GAME
#include "logger.h"
#include "render/vertex.h"

#include "render/primitives.h"

void render_test_push(GameState* state) {
    state->stored_state = malloc(sizeof(RenderTestState));

    RenderTestState* s = (RenderTestState*)state->stored_state;
    Shader vs = load_vertex_shader_from_disk("shaders/test_shader.vs").data.ok;
    Shader fs = load_fragment_shader_from_disk("shaders/test_shader.fs").data.ok;
    s->test_shader = create_shader_program(vs, fs).data.ok;

    Image i = load_image_from_file("assets/test_texture.txt");
    s->test_texture = generate_texture();
    bind_image_to_texture(&s->test_texture, i);
    destroy_image(i);

    s->projection = matrix_orthographic_projection(-1.f, 1.f, 1.f, -1.f, -100.f, 100.f);

    glGenVertexArrays(1, &s->vao);
    bind_primitive_to_vao(primitive_quad(), s->vao);
}

void render_test_pop(GameState* state) {
    free(state->stored_state);
}

void render_test_update(GameState* state, float delta_time) {
}

void render_test_render(GameState* state) {
    RenderTestState* s = (RenderTestState*)state->stored_state;

    int projectionPosition = glGetUniformLocation(s->test_shader._program, "projection");

    glUseProgram(s->test_shader._program);
    glUniformMatrix4fv(projectionPosition, 1, false, s->projection.entries);
    glBindVertexArray(s->vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
