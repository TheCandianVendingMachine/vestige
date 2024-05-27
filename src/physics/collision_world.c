#include "physics/collision_world.h"
#include "physics/collider.h"

CollisionWorld create_collision_world(void) {
    CollisionWorld world;
    world.collider_id = 0;
    world.colliders = new_colony(sizeof(Collider));
    return world;
}

void collision_world_step(CollisionWorld* world) {

}

Collider* world_create_collider(CollisionWorld* world, enum BoundType bound_type, union Bound bound) {
    Collider collider = create_collider(bound_type, bound);
    collider._id = 1 + world->collider_id;
    size_t index = colony_insert(&world->colliders, &collider);
    world->collider_id += 1;
    return colony_get(world->colliders, index);
}
