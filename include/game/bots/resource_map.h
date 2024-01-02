#ifndef BOTS_RESOURCE_MAP_H
#define BOTS_RESOURCE_MAP_H

#include "lib/vector.h"
#include "render/shader.h"
#include "game/bots/resource.h"

typedef struct ResourceMap {
    Vector* resources;
    struct {
        ShaderProgram render_resources;
    } _render;
} ResourceMap;

struct GameplayState;
struct FontEngine;

ResourceMap create_resource_map(struct FontEngine* font_engine);
void destroy_resource_map(ResourceMap* resource_map);

void add_resource(ResourceMap* resource_map, Resource resource);
void resource_map_render(struct GameplayState* state, ResourceMap* resource_map);

#endif
