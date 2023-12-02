#include <stdlib.h>

#include "game/voxel/voxel.h"
#include "game/game_states.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_GAME
#include "logger.h"

void voxel_state_push(GameState* state) {
    state->stored_state = malloc(sizeof(VoxelState));
    //VoxelState* s = (VoxelState*)state->stored_state;
}

void voxel_state_pop(GameState* state) {
    //VoxelState* s = (VoxelState*)state->stored_state;

    free(state->stored_state);
    state->stored_state = NULL;
}
