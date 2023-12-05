#ifndef VESTIGE_VOXEL_GRID_H
#define VESTIGE_VOXEL_GRID_H

#include <stdint.h>

#include "render/color.h"
#include "lib/vector.h"
#include "lib/math.h"

/*
 * 3d grid for the voxel renderer
 * Tries to be as sparse as possible: cells have properties which facilitate rendering
 */

typedef struct Cell {
    uint16_t density;
    // If you add a cell type, make sure to update in voxel_shader.fs
    enum {
        CELL_EMPTY,
        CELL_WALL,
        CELL_COUNT
    } type: 8;
} Cell;

typedef struct CellData {
    ColorRGB color;
} CellData;

typedef struct VoxelGrid {
    CellData cell_data[CELL_COUNT];
    Cell *cells;
    Vector3i size;
} VoxelGrid;

VoxelGrid new_grid(Vector3i size);
void delete_grid(VoxelGrid* grid);

#endif
