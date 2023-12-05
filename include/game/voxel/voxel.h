#ifndef VESTIGE_VOXEL_H
#define VESTIGE_VOXEL_H

#include "game/voxel/grid.h"
#include "render/shader.h"
#include "render/camera.h"

typedef struct VoxelState {
    VoxelGrid grid;
    ShaderProgram voxel_shader;
    Camera camera;
} VoxelState;

struct GameState;
void voxel_state_push(struct GameState* state);
void voxel_state_pop(struct GameState* state);
void voxel_state_render(struct GameState* state);

#endif
