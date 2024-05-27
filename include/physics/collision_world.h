#ifndef VESTIGE_COLLISION_WORLD_H
#define VESTIGE_COLLISION_WORLD_H

#include <stdint.h>

#include "lib/colony.h"
#include "physics/collider.h"

typedef struct CollisionWorld {
    uint64_t collider_id;
    Colony colliders;
} CollisionWorld;

CollisionWorld create_collision_world(void);

void collision_world_step(CollisionWorld* world);

struct Collider;
struct Collider* world_create_collider(CollisionWorld* world, enum BoundType bound_type, union Bound bound);

#endif
