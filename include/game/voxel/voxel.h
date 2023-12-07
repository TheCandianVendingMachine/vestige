#ifndef VESTIGE_VOXEL_H
#define VESTIGE_VOXEL_H

#include "game/voxel/grid.h"
#include "render/shader.h"
#include "render/camera.h"
#include "lib/clock.h"

typedef struct VoxelState {
    struct {
        VoxelGrid cells;
        unsigned int grid_texture;
    } grid;
    ShaderProgram voxel_shader;
    unsigned int voxel_render_target;
    Camera camera;
    Clock shader_update_clock;
} VoxelState;

struct GameState;
void voxel_state_push(struct GameState* state);
void voxel_state_pop(struct GameState* state);
void voxel_state_update(struct GameState* state, float delta_time);
void voxel_state_render(struct GameState* state);

#endif
