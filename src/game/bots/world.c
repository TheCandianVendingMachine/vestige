#include "game/bots/world.h"

World new_world(struct FontEngine* font_engine) {
    World world;
    world.resources = create_resource_map(font_engine);
    return world;
}

void render_world(struct GameplayState* state, World* world) {
    resource_map_render(state, &world->resources);
}
