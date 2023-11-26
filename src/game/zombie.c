#include <stdlib.h>

#include "game/zombie.h"
#include "game/game_states.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_GAME
#include "logger.h"
#include "render/vertex.h"

#include "render/primitives.h"

void zombie_push(GameState* state) {
    state->stored_state = malloc(sizeof(ZombieState));

    ZombieState* s = (ZombieState*)state->stored_state;
    Shader vs = load_vertex_shader_from_disk("shaders/zombie_shader.vs").data.ok;
    Shader fs = load_fragment_shader_from_disk("shaders/zombie_shader.fs").data.ok;
    s->zombie_shader = create_shader_program(vs, fs).data.ok;

    s->projection = matrix_orthographic_projection(-1280.f, 1280.f, 720.f, -720.f, 0.1f, 10000.f);
    s->camera = new_camera();
    s->camera.position.x = 0.f;
    s->camera.position.y = 0.f;
    s->camera.position.z = 0.f;

    log_info("Creating world");
    s->world = create_new_world();
    log_info("Done setup");
}

void zombie_pop(GameState* state) {
    ZombieState* s = (ZombieState*)state->stored_state;
    log_info("Destroying world");
    destroy_world(&s->world);
    free(state->stored_state);
    log_info("Done destruction");
}

void zombie_update(GameState* state, float delta_time) {
    ZombieState* s = (ZombieState*)state->stored_state;
    update_world(&s->world, delta_time);
}

void zombie_render(GameState* state) {
    ZombieState* s = (ZombieState*)state->stored_state;

    int projection_position = glGetUniformLocation(s->zombie_shader._program, "projection");
    int model_position = glGetUniformLocation(s->zombie_shader._program, "model");
    int view_position = glGetUniformLocation(s->zombie_shader._program, "view");

    glUseProgram(s->zombie_shader._program);
    glUniformMatrix4fv(projection_position, 1, false, s->projection.entries);
    glUniformMatrix4fv(view_position, 1, false, camera_view(&s->camera).entries);
    draw_zombies(&s->world, model_position);
}
