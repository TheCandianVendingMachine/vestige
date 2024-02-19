#include <stdlib.h>
#include <math.h>

#include "atlas.h"
#include "render/texture.h"
#include "lib/clock.h"
#include "logger.h"

int _compare_entries(const void* p_lhs, const void* p_rhs) {
    struct AtlasEntry lhs = *(struct AtlasEntry*)p_lhs;
    struct AtlasEntry rhs = *(struct AtlasEntry*)p_rhs;

    float a0 = aabb_area(lhs.bounds);
    float a1 = aabb_area(rhs.bounds);

    // We sort the biggest area entry first to use as much space as possible
    return (a0 < a1) - (a0 > a1);
}

// Packing algorithm: MAXRECTS
// Jukka Jylänki, "A Thousand Ways to Pack the Bin - A Practical Approach to 
// Two-Dimensional Rectangle Bin Packing", http://pds25.egloos.com/pds/201504/21/98/RectangleBinPack.pdf
bool create_atlas(Atlas* atlas, Vector entries, Vector2i size) {
    atlas->size = size;
    log_debug("Packing new Atlas");
    Clock pack_clock = new_clock();
    // Sort entries according to some heuristic for an optimal packing
    qsort(entries.buffer, entries.length, sizeof(AtlasEntry), _compare_entries);

    typedef struct EntryRectangle {
        ShapeAABB bounds;
        size_t entry;
    } EntryRectangle;
    Vector placed_entries = VECTOR(EntryRectangle);
    Vector face_list = VECTOR(EntryRectangle);
    VECTOR_PUSH(EntryRectangle, &face_list, ((EntryRectangle) {
        .bounds = (ShapeAABB) {
            .size = (Vector2f) { .x = size.x, .y = size.y },
            .position = (Vector2f) { { 0.f, 0.f } }
        },
        .entry = -1
    }));

    bool could_pack_all_entries = true;
    Vector faces_to_delete = VECTOR(size_t);
    for (int i = 0; i < entries.length; i++) {
        // Find minimum area face we can put meta-entry into
        const AtlasEntry entry = _VECTOR_GET(AtlasEntry, &entries, i);
        float min_area = INFINITY;
        int best_face = 0;
        for (int j = 0; j < face_list.length; j++) {
            EntryRectangle f = _VECTOR_GET(EntryRectangle, &face_list, j);
            float face_area = f.bounds.size.x * f.bounds.size.y;
            // the area is minimal and the entry can fit within the rectangle
            if (face_area < min_area &&
                f.bounds.size.x >= entry.bounds.size.x && f.bounds.size.y >= entry.bounds.size.y) {
                min_area = face_area;
                best_face = j;
            }
        }

        if (min_area == INFINITY) {
            log_error("Could not pack entry into atlas");
            could_pack_all_entries = false;
            continue;
        }

        log_debug_verbose(2, "Best area found to pack: %f", min_area);

        // We place the entry in the top-left of the rectangle, so we have
        // new_width = face.width - entry.width,
        // new_height = face.height - entry.height

        const float best_width = _VECTOR_GET(EntryRectangle, &face_list, best_face).bounds.size.x;
        const float best_height = _VECTOR_GET(EntryRectangle, &face_list, best_face).bounds.size.y;
        const float best_x = _VECTOR_GET(EntryRectangle, &face_list, best_face).bounds.position.x;
        const float best_y = _VECTOR_GET(EntryRectangle, &face_list, best_face).bounds.position.y;

        log_debug_verbose(2, "Placing entry at [%f, %f]", best_x, best_y);

        EntryRectangle placed_entry = (EntryRectangle) {
            .bounds = (ShapeAABB) {
                .size = entry.bounds.size,
                .position = (Vector2f) { { best_x, best_y } }
            },
            .entry = i
        };
        VECTOR_PUSH(EntryRectangle, &placed_entries, placed_entry);
        EntryRectangle f_horizontal;
        EntryRectangle f_vertical;

        const float new_width = best_width - entry.bounds.size.x;
        const float new_height = best_height - entry.bounds.size.y;

        f_horizontal.bounds.position.x = best_x;
        f_horizontal.bounds.position.y = best_y + entry.bounds.size.y;
        f_horizontal.bounds.size.x = best_width;
        f_horizontal.bounds.size.y = new_height;

        f_vertical.bounds.position.x = best_x + entry.bounds.size.x;
        f_vertical.bounds.position.y = best_y;
        f_vertical.bounds.size.x = new_width;
        f_vertical.bounds.size.y = best_height;

        VECTOR_PUSH(EntryRectangle, &face_list, f_horizontal);
        VECTOR_PUSH(EntryRectangle, &face_list, f_vertical);

        faces_to_delete.length = 0;
        VECTOR_PUSH(size_t, &faces_to_delete, best_face);

        // When adding new faces, there can be an intersection that forms with the newly
        // placed entry
        // If this occurs, we will cull the AABB so it not longer intersects the entry
        for (int j = 0; j < face_list.length; j++) {
            EntryRectangle face = _VECTOR_GET(EntryRectangle, &face_list, j);
            if (j != best_face && aabb_intersect(face.bounds, placed_entry.bounds)) {
                int dx = placed_entry.bounds.position.x - face.bounds.position.x;
                int dy = placed_entry.bounds.position.y - face.bounds.position.y;

                if (dx > dy) {
                    face.bounds.size.x = dx;
                    VECTOR_SET(EntryRectangle, &face_list, j, face);
                } else {
                    face.bounds.size.y = dy;
                    VECTOR_SET(EntryRectangle, &face_list, j, face);
                }

                if (face.bounds.size.y <= 0 || face.bounds.size.x <= 0) {
                    VECTOR_PUSH(size_t, &faces_to_delete, j);
                }
            }
        }

        for (int j = 0; j < faces_to_delete.length; j++) {
            size_t face_to_delete = _VECTOR_GET(size_t, &faces_to_delete, j);
            if (face_list.length > 0) {
                EntryRectangle swap_face = _VECTOR_GET(EntryRectangle, &face_list, face_list.length - 1);
                _VECTOR_SET(EntryRectangle, &face_list, face_to_delete, swap_face);
            }
            _VECTOR_POP(EntryRectangle, &face_list);
        }
    }

    // Convert EntryRectangle -> AtlasEntry
    for (int i = 0; i < placed_entries.length; i++) {
        EntryRectangle entry_rect = _VECTOR_GET(EntryRectangle, &placed_entries, i);
        AtlasEntry entry = _VECTOR_GET(AtlasEntry, &entries, entry_rect.entry);
        entry.bounds = entry_rect.bounds;
        _VECTOR_SET(AtlasEntry, &entries, i, entry);
    }

    atlas->packed_entries = entries;
    log_debug_verbose(1, "Atlas packed in %d microseconds", time_as_microseconds(get_elapsed_time(&pack_clock)));
    return could_pack_all_entries;
}

void destroy_atlas(Atlas atlas) {
    del_vector(atlas.packed_entries);
}

Image draw_atlas(Atlas atlas, int channels, void (draw_entry)(uint8_t*, int, Vector2i, AtlasEntry)) {
    log_debug("Drawing atlas to CPU buffer");
    Image pixels = generate_image(atlas.size.x, atlas.size.y, channels);
    for (int i = 0; i < atlas.packed_entries.length; i++) {
        AtlasEntry entry = _VECTOR_GET(AtlasEntry, &atlas.packed_entries, i);
        log_debug_verbose(5, "Drawing entry %d", i);
        log_debug_verbose(4, "Drawing at [%d, %d]", (int)entry.bounds.position.x, (int)entry.bounds.position.y);
        draw_entry(pixels.pixels._bound, channels, atlas.size, entry);
    }

    return pixels;
}
