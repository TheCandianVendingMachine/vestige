#include <stdlib.h>

#include "game/voxel/voxel.h"
#include "game/game_states.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_GAME
#include "logger.h"

void voxel_state_push(GameState* state) {
    state->stored_state = malloc(sizeof(VoxelState));
    VoxelState* s = (VoxelState*)state->stored_state;
    s->grid = new_grid((Vector3i){ .x = 5, .y = 5, .z = 5});

    Shader vs = load_vertex_shader_from_disk("shaders/voxel_shader.vs").data.ok;
    Shader fs = load_fragment_shader_from_disk("shaders/voxel_shader.fs").data.ok;
    s->voxel_shader = create_shader_program(vs, fs).data.ok;

    s->camera = new_camera();
}

void voxel_state_pop(GameState* state) {
    VoxelState* s = (VoxelState*)state->stored_state;
    delete_grid(&s->grid);

    free(state->stored_state);
    state->stored_state = NULL;
}

void voxel_state_render(GameState* state) {
    VoxelState* s = (VoxelState*)state->stored_state;

    glUseProgram(s->voxel_shader._program);
}
