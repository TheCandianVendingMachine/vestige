#ifndef VESTIGE_ATLAS_H
#define VESTIGE_ATLAS_H

#include <stdint.h>

#include "lib/vector.h"
#include "lib/math.h"

typedef struct AtlasEntry {
    AABB bounds;
    void* user_data;
} AtlasEntry;

typedef struct Atlas {
    Vector packed_entries;
    Vector2i size;
} Atlas;

bool create_atlas(Atlas* atlas, Vector entries, Vector2i size);
void destroy_atlas(Atlas atlas);

uint8_t* draw_atlas(Atlas atlas, void (draw_entry)(uint8_t*, AtlasEntry));

#endif
