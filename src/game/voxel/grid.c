#include <string.h>
#include <limits.h>

#include "game/voxel/grid.h"

CellData DEFAULT_CELL_DATA[CELL_COUNT] = {
    (CellData) { .color = (ColorRGB) { .r = 0, .g = 0, .b = 0 } },
    (CellData) { .color = (ColorRGB) { .r = 38, .g = 139, .b = 34 } }
};

VoxelGrid new_grid(Vector3i size) {
    size_t voxel_count = size.x * size.y * size.z * sizeof(Cell);
    VoxelGrid grid = (VoxelGrid) {
        .cells = malloc(voxel_count),
        .size = size,
    };
    memcpy(grid.cell_data, DEFAULT_CELL_DATA, CELL_COUNT * sizeof(CellData));
    memset(grid.cells, *(int*)(&(Cell) { .type = CELL_EMPTY, .density = 0 }), voxel_count);

    return grid;
}

void delete_grid(VoxelGrid* grid) {
    free(grid->cells);
    grid->cells = NULL;
}
