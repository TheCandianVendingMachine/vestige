#include "physics/collision_world.h"
#include "physics/collider.h"

CollisionWorld create_collision_world(void) {
    CollisionWorld world;
    world.colliders = new_colony(sizeof(Collider));
    return world;
}
