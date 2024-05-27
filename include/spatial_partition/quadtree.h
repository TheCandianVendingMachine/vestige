#ifndef VESTIGE_QUADTREE_H
#define VESTIGE_QUADTREE_H

#include <stdint.h>
#include "lib/bitset.h"

// Assumptions:
// Quadtree will be recreated whenever any data is updated
typedef struct Quadtree {
    uint8_t maximum_depth : 4; // set bitsize to 4 for compiler optimisations
    // Always growing array of cells. At most 4^n cells will be present; maximum depth
    // equal to 32 in this case
    void* cells;
    size_t cell_count;
    Bitset used_cells;
    // The tree DS. TOMBSTONE indicates free, otherwise entry equals index into cell array
    // The data stored in the entry is one of: [uint8, uint16, uint32, uint64]
    void* tree;
    size_t tree_max_cell_count;
    size_t tree_index_size;

    uint64_t max_objects;
    uint64_t object_count;
    uint64_t cell_split_count;
} Quadtree;

Quadtree create_quadtree(uint8_t maximum_depth, uint64_t max_objects, uint64_t split_count);
void destroy_quadtree(Quadtree* tree);

// Clear out quadtree, does not deallocate
void quadtree_clear(Quadtree* tree);
// x, y are in range [0, 1]
void quadtree_insert(Quadtree* tree, void* object, float x, float y);

/*
    [0..1q..2q..3q..4q]
    Generic insertion scheme:
        Divide cell array into quarters
        If in top left, cell will be in range [0..1q)
        If in top right, cell will be in range [1q..2q)
        If in bottom left, cell will be in range [2q..3q)
        If in bottom right, cell will be in range [3q..4q)

        This is optimal, because it describes a z-order space filling curve at various
        LODs

    We decide d by setting maximum tree depth at creation of the quadtree. We will have a
    maximum, non-sparse, cell array of size 4^n, where n=maximum depth of tree. This grows
    expontentially, so we only store indices into the cell data structure within this array.
    We only allow a maximum of 2^32-1 objects, hence the uint32

 */

#endif
