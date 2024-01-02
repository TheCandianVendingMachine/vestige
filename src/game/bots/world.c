#include "game/bots/world.h"
#include "lib/math.h"

World new_world(struct FontEngine* font_engine) {
    World world;
    world.resources = create_resource_map(font_engine);

    for (int i = 0; i < 100000; i++) {
        Resource resource;
        resource.initial = 50000;
        resource.remaining = resource.initial;
        resource.position = (Vector2f) {
            .x = frandrange(-300000.f, 300000.f),
            .y = frandrange(-300000.f, 300000.f)
        };
        resource.type = irandrange(0, RESOURCE_TYPE_COUNT);
        add_resource(&world.resources, resource);
    }

    return world;
}

void render_world(struct GameplayState* state, World* world) {
    resource_map_render(state, &world->resources);
}
