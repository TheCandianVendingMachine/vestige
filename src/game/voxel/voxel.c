#include <stdlib.h>
#include <math.h>

#include "game/voxel/voxel.h"
#include "game/game_states.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_GAME
#include "logger.h"
#include "engine.h"

#include "render/primitives.h"
#include "lib/math.h"

void voxel_state_push(GameState* state) {
    state->stored_state = malloc(sizeof(VoxelState));
    VoxelState* s = (VoxelState*)state->stored_state;
    s->grid.cells = new_grid((Vector3i){ .x = 256, .y = 256, .z = 256});

    Shader vs = load_vertex_shader_from_disk("shaders/voxel_shader.vert").data.ok;
    Shader fs = load_fragment_shader_from_disk("shaders/voxel_shader.frag").data.ok;
    s->voxel_shader = create_shader_program(vs, fs).data.ok;

    s->camera = new_camera();
    s->camera.position = (Vector3f){ { 64.f, 64.f, 0.f } };

    glGenVertexArrays(1, &s->voxel_render_target);
    bind_primitive_to_vao(primitive_quad(), s->voxel_render_target);

    s->shader_update_clock = new_clock();

    glGenTextures(1, &s->grid.grid_texture);
    glBindTexture(GL_TEXTURE_3D, s->grid.grid_texture);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_3D, 0);

    set_cell(&s->grid.cells, (Vector3i) { { 0, 0, 0 } }, (Cell) {.density = 65535, .type = CELL_WALL});

    for (int i = 0; i < 256; i++) {
        set_cell(&s->grid.cells, (Vector3i) { { i, 0, 0 } }, (Cell) {.density = 65535, .type = CELL_WALL});
        set_cell(&s->grid.cells, (Vector3i) { { 0, i, 0 } }, (Cell) {.density = 65535, .type = CELL_WALL});
        set_cell(&s->grid.cells, (Vector3i) { { 0, 0, i } }, (Cell) {.density = 65535, .type = CELL_WALL});
    }
}

void voxel_state_pop(GameState* state) {
    VoxelState* s = (VoxelState*)state->stored_state;
    delete_grid(&s->grid.cells);

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

    static float acc = 0.f;
    s->camera.direction.x = cos(acc);
    s->camera.direction.y = sin(acc);
    s->camera.direction.z = 0.f;
    acc += delta_time / 2.f;
    s->camera._updated = true;
}

void voxel_state_render(GameState* state) {
    VoxelState* s = (VoxelState*)state->stored_state;

    glBindTexture(GL_TEXTURE_3D, s->grid.grid_texture);
    glTexImage3D(
        GL_TEXTURE_3D, 0, GL_RGB8,
        s->grid.cells.size.x, s->grid.cells.size.y, s->grid.cells.size.z,
        0, GL_RGB, GL_UNSIGNED_BYTE, s->grid.cells.cells
    );

    int screenSizePosition = glGetUniformLocation(s->voxel_shader._program, "screenSize");
    int projectionPosition = glGetUniformLocation(s->voxel_shader._program, "projection");
    int cameraPosition = glGetUniformLocation(s->voxel_shader._program, "cameraProjection");

    Matrix4f projection = matrix_orthographic_projection(-1.f, 1.f, 1.f, -1.f, -1.f, 1.f);

    glUseProgram(s->voxel_shader._program);
    glUniform2iv(screenSizePosition, 1, ENGINE->window.size.entries);
    glUniformMatrix4fv(cameraPosition, 1, false, camera_view(&s->camera).entries);
    glUniformMatrix4fv(projectionPosition, 1, false, projection.entries);
    glBindTexture(GL_TEXTURE_3D, s->grid.grid_texture);
    draw_primitive(primitive_quad(), s->voxel_render_target);
    glBindTexture(GL_TEXTURE_3D, 0);
}
