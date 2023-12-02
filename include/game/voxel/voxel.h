#ifndef VESTIGE_VOXEL_H
#define VESTIGE_VOXEL_H

typedef struct VoxelState {
    int _;
} VoxelState;

struct GameState;
void voxel_state_push(struct GameState* state);
void voxel_state_pop(struct GameState* state);

#endif
