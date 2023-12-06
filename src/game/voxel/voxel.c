#include <stdlib.h>

#include "game/voxel/voxel.h"
#include "game/game_states.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_GAME
#include "logger.h"

#include "render/primitives.h"
#include "lib/math.h"

void voxel_state_push(GameState* state) {
    state->stored_state = malloc(sizeof(VoxelState));
    VoxelState* s = (VoxelState*)state->stored_state;
    s->grid = new_grid((Vector3i){ .x = 5, .y = 5, .z = 5});

    Shader vs = load_vertex_shader_from_disk("shaders/voxel_shader.vert").data.ok;
    Shader fs = load_fragment_shader_from_disk("shaders/voxel_shader.frag").data.ok;
    s->voxel_shader = create_shader_program(vs, fs).data.ok;

    s->camera = new_camera();

    glGenVertexArrays(1, &s->voxel_render_target);
    bind_primitive_to_vao(primitive_quad(), s->voxel_render_target);

    glGenBuffers(1, &s->grid_ssbo);

    s->shader_update_clock = new_clock();
}

void voxel_state_pop(GameState* state) {
    VoxelState* s = (VoxelState*)state->stored_state;
    delete_grid(&s->grid);

    free(state->stored_state);
    state->stored_state = NULL;
}

void voxel_state_update(struct GameState* state, float delta_time) {
    VoxelState* s = (VoxelState*)state->stored_state;
    const float UPDATE_TIME = 0.1f;
    if (time_as_seconds(get_elapsed_time(&s->shader_update_clock)) > UPDATE_TIME) {
        update_shader_program(&s->voxel_shader);
        s->shader_update_clock = new_clock();
    }
}

void voxel_state_render(GameState* state) {
    VoxelState* s = (VoxelState*)state->stored_state;

    int projectionPosition = glGetUniformLocation(s->voxel_shader._program, "projection");
    int cameraPosition = glGetUniformLocation(s->voxel_shader._program, "cameraProjection");
    //int cellDataPosition = glGetUniformLocation(s->voxel_shader._program, "cellData");
    int gridPosition = glGetProgramResourceIndex(s->voxel_shader._program, GL_SHADER_STORAGE_BLOCK, "grid");

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, s->grid_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, s->grid.cell_count * sizeof(Cell), s->grid.cells, GL_STATIC_DRAW);

    Matrix4f projection = matrix_orthographic_projection(-1.f, 1.f, 1.f, -1.f, -1.f, 1.f);

    glUseProgram(s->voxel_shader._program);
    glUniformMatrix4fv(cameraPosition, 1, false, camera_view(&s->camera).entries);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, gridPosition, s->grid_ssbo);
    glUniformMatrix4fv(projectionPosition, 1, false, projection.entries);
    draw_primitive(primitive_quad(), s->voxel_render_target);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind
}
