#include "game/bots/world.h"

void render_world(struct GameplayState* state, World* world) {
    resource_map_render(state, &world->resources);
}
