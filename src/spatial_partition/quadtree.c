#define TOMBSTONE (~(uint8_t)0)

#include <assert.h>
#include <stdlib.h>

#include "lib/math.h"
#include "lib/vector.h"
#include "spatial_partition/quadtree.h"

typedef struct Object {
    void* object;
    float x;
    float y;
} Object;

typedef struct Cell {
    uint8_t initialised;
    uint8_t is_leaf;
    Vector objects;
} Cell;

Quadtree create_quadtree(uint8_t maximum_depth, uint64_t max_objects, uint64_t split_count) {
    assert(maximum_depth < 16);
    assert(max_objects > 0);
    assert(split_count > 0);

    Quadtree tree;
    tree.maximum_depth = maximum_depth;
    tree.max_objects = max_objects;
    tree.object_count = 0;
    tree.cell_split_count = split_count;

    // To get tree index size, we take the log_2(max_objects) to get index byte count
    tree.tree_index_size = log2int(max_objects);

    // 4^n = (2^2)^n = 2^(2 * n)
    // This assumes every cell is filled. The pigeonhole principle tells us that:
    // if max_objects > 4^depth, then we should allocate based on 4^depth
    // if max_objects <= 4^depth, then we should allocate based on max_objects
    // Therefore, maximum number of cells = min(max_objects, 4^depth)
    uint64_t maximum_cells = ((uint64_t)1) << (2 * maximum_depth);
    if (max_objects < maximum_cells) {
        // Round max_objects to next power of 4 so we can quarter indefinitely
        // Through log laws and integer log_2 properties,
        // 4^ceil(log_4(x)) == 2^(2*(floor(log_2(x) / 2) + 1))
        // = 1 << (2 * ((floor_log_2(x) >> 1) + 1))
        uint64_t y = log2int(max_objects) >> 1; // floor_log_2(x) >> 1
        y = y + 1;                              // (floor_log_2(x) >> 1) + 1
        y = 2 * y;                              // 2 * ((floor_log_2(x) >> 1) + 1)
        y = ((uint64_t)1) << y;                 // 1 << (2 * ((floor_log_2(x) >> 1) + 1))
        tree.tree_max_cell_count = y;
    } else {
        tree.tree_max_cell_count = maximum_cells;
    }
    tree.tree = malloc(tree.tree_index_size * tree.tree_max_cell_count);

    // Initially allocate 8 cells to put data into
    tree.cell_count = 8;
    tree.cells = malloc(sizeof(Cell) * tree.cell_count);
    memset(tree.cells, 0, sizeof(Cell) * tree.cell_count);
    tree.used_cells = create_bitset(tree.tree_max_cell_count);

    quadtree_clear(&tree);

    return tree;
}

void destroy_quadtree(Quadtree* tree) {
    free(tree->cells);
    free(tree->tree);
    destroy_bitset(&tree->used_cells);
}

// Clear out quadtree, does not deallocate
void quadtree_clear(Quadtree* tree) {
    for (size_t i = 0; i < tree->cell_count; i++) {
        ((Cell*)tree->cells)[i].objects.length = 0;
    }
    tree->used_cells = bitset_clear(tree->used_cells);
    memset(tree->tree, TOMBSTONE, tree->tree_index_size * tree->tree_max_cell_count);
    tree->object_count = 0;

    Cell* root = &((Cell*)tree->cells)[0];
    if (!root->initialised) {
        root->initialised = 1;
        // vector will never grow since we set the size to what we want
        root->objects = new_vector(sizeof(Object), tree->cell_split_count);
    }
    root->is_leaf = 1;
    bitset_set_bit(&tree->used_cells, 0, 1);
}

// x, y are in range [0, 1]
void quadtree_insert(Quadtree* tree, void* object, float x, float y) {
    assert(tree->object_count < tree->max_objects);

    size_t base_index = 0;
    size_t index_count = tree->tree_max_cell_count;
    float min_x = 0.f;
    float max_x = 1.f;
    float min_y = 0.f;
    float max_y = 1.f;
    for (int i = 0; i < tree->maximum_depth; i++) {
        float middle_x = (min_x + max_x) / 2.f;
        float middle_y = (min_y + max_y) / 2.f;

        size_t indices[4] = {
            base_index + 0 * (index_count / 4),
            base_index + 1 * (index_count / 4),
            base_index + 2 * (index_count / 4),
            base_index + 3 * (index_count / 4),
        };
        uint8_t selected_index = 0b00001111;

        // Todo: optimise to bit bit-twiddling
        // If this is slow in hotpath, we will need to do this
        if (y < middle_y) {
            max_y = middle_y;
            // in the top index
            selected_index = selected_index & 0b00000011;
        } else {
            min_y = middle_y;
            // in the bottom index
            selected_index = selected_index & 0b00001100;
        }

        if (x < middle_x) {
            max_x = middle_x;
            // No change for base_index, we don't move if we are the left-most
            selected_index = selected_index & 0b00000101;
        } else {
            min_x = middle_x;
            // Otherwise, set the base_index to the right point
            selected_index = selected_index & 0b00001010;
        }

        size_t cell_index = indices[log2int(selected_index)];

        void* v = ((uint8_t*)tree->tree) + (cell_index * tree->tree_index_size);
        uint64_t cell_value;
        memcpy(&cell_value, v, tree->tree_index_size);

        // If we are a leaf, insert. Otherwise, keep descending
        // determine if we split into this cell
        Cell* cell = &((Cell*)tree->cells)[cell_value];
        if (cell->is_leaf) {
            if (
                cell->objects.length == tree->cell_split_count &&
                i < (tree->maximum_depth - 1)
            ) {
                // split cell, and insert all cell objects into new children
                // we continue the descent for the object we want, because fuck it we are
                // already in the loop
                for (int i = 0; i < 4; i++) {
                    Cell* new_leaf = &((Cell*)tree->cells)[indices[i]];
                    if (!new_leaf->initialised) {
                        new_leaf->initialised = 1;
                        new_leaf->objects = new_vector(sizeof(Object), tree->cell_split_count);
                    }
                    new_leaf->is_leaf = 1;
                }
                cell->is_leaf = 0;

                for (int i = 0; i < cell->objects.length; i++) {
                    Object obj = _VECTOR_GET(Object, &cell->objects, i);
                    quadtree_insert(tree, obj.object, obj.x, obj.y);
                }
            } else {
                // insert into this cell and return
                Object obj;
                obj.x = x;
                obj.y = y;
                obj.object = object;
                VECTOR_PUSH(Object, &cell->objects, obj);
                break;
            }
        }
    }

    tree->object_count += 1;
}
