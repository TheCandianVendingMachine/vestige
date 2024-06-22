#ifndef GAME_PHYSICS_H
#define GAME_PHYSICS_H

#include "lib/math.h"
#include "lib/vector.h"
#include "render/camera.h"
#include "debug/render.h"

#include "physics/rigid_body.h"
#include "physics/dynamics_world.h"
#include "physics/collision_world.h"

typedef struct {
    RigidBody* body;
} Floor;

typedef struct {
    RigidBody* body;
} Rectangle;

typedef struct {
    RigidBody* body;
} Circle;

typedef struct PhysicsTestState {
    DynamicsWorld dynamics;
    CollisionWorld collision;

    Vector rectangles;
    Vector circles;
    Floor floor;

    DebugRender renderer;
    Camera camera;
    Matrix4f projection;
} PhysicsTestState;
struct GameState;

void physics_push(struct GameState* state);
void physics_pop(struct GameState* state);
void physics_update(struct GameState* state);
void physics_fixed_update(struct GameState* state, float delta_time);
void physics_render(struct GameState* state);

#endif
