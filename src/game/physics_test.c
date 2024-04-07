#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "engine.h"
#include "logger.h"
#include "game/game_states.h"
#include "game/physics_test.h"

#define GRAVITY_ACCEL 10.0f
Vector2f GRAVITY = (Vector2f) { .x = 0.f, .y = GRAVITY_ACCEL };

DebugRender *DRENDER = NULL;

void resolve_collision(RigidBody* b1, RigidBody b2, CollisionInfo info) {
    if (b1->mass == INFINITY) { return; }
    float amount = length_vector2f(b1->velocity) / length_vector2f(b2.velocity);
    if (isinf(amount)) {
        amount = 1.f;
    } else if (isnan(amount)) {
        amount = 0.f;
    } else if (amount < 0.f) {
        assert(false);
    } else {
        if (amount > 1.f) {
            amount = 1.f / amount;
            amount *= 0.5f;
            amount = 1.f - amount;
        } else {
            amount *= 0.5f;
        }
    }
    b1->position = sub_vector2f(b1->position, mul_vector2f(info.minimum_translation_vector, amount));

    Vector2f p1 = b1->position;
    Vector2f p2 = b2.position;

    Vector2f v1 = b1->velocity;
    Vector2f v2 = b2.velocity;

    // conserve energy and momentum in a collision
    float inverse_mass_sum = 1.f / (b1->mass + b2.mass);
    Vector2f collision_axis = normalise_vector2f(info.minimum_translation_vector);
    {
        Vector2f dp = sub_vector2f(p1, p2);
        Vector2f dv = sub_vector2f(v1, v2);
        if (b2.mass == INFINITY) {
            Vector2f dv_proj = project_vector2f(dv, collision_axis);
            float inverse_mass = b1->restitution / 2.f + b2.restitution;
            b1->velocity = sub_vector2f(b1->velocity, mul_vector2f(dv_proj, inverse_mass));
        } else {
            Vector2f dv_proj = project_vector2f(dv, dp);
            float inverse_mass = (b1->restitution + b2.restitution) * b2.mass * inverse_mass_sum;
            b1->velocity = sub_vector2f(b1->velocity, mul_vector2f(dv_proj, inverse_mass));
        }
    }

    {
        Vector2f normal_accel = project_vector2f(b1->acceleration, collision_axis);
        Vector2f gravity_accel = project_vector2f(GRAVITY, collision_axis);
        normal_accel = add_vector2f(normal_accel, gravity_accel);
        b1->normal_force = add_vector2f(b1->normal_force, mul_vector2f(normal_accel, -b1->mass));
        b1->normal_force = add_vector2f(b1->normal_force, project_vector2f(b1->linear_impulse, collision_axis));
    }
}

void collide_bodies(RigidBody* b1, RigidBody* b2) {
    b1->collider.position = b1->position;
    b2->collider.position = b2->position;
    CollisionInfo collision = collider_test_collision(b1->collider, b2->collider);
    if (!collision.collides) {
        return;
    }

    RigidBody b1_prev = *b1;
    RigidBody b2_prev = *b2;

    resolve_collision(b1, b2_prev, collision);
    collision.minimum_translation_vector = mul_vector2f(collision.minimum_translation_vector, -1.f);
    resolve_collision(b2, b1_prev, collision);

    float friction_coef = FRICTION_COEFFICIENT_TABLE[b1->material][b2->material];
    b1->friction_force = add_vector2f(b1->friction_force, mul_vector2f(b1->normal_force, friction_coef));
    b2->friction_force = add_vector2f(b2->friction_force, mul_vector2f(b2->normal_force, friction_coef));
}

void physics_push(struct GameState* state) {
    state->stored_state = malloc(sizeof(PhysicsTestState));

    PhysicsTestState* s = (PhysicsTestState*)state->stored_state;
    s->projection = matrix_orthographic_projection(
        0.f, -1.f * ENGINE->window.size.x,
        0.f, ENGINE->window.size.y,
        -1.f, 1.f
    );
    s->camera = new_camera();

    s->renderer = new_debug_renderer();
    DRENDER = &s->renderer;

    s->dynamics = create_dynamics_world();
    s->collision = create_collision_world();

    s->floor = (Floor) {
        .body = world_create_rigid_body(&s->dynamics)
    };
    s->floor.body->flags = SIMULATION_DISABLE_GRAVITY;
    s->floor.body->material = RIGID_BODY_MATERIAL_ICE;
    s->floor.body->collider.bound.shape.aabb = (ShapeAABB) {
        .position = (Vector2f) { .x = 0.f, .y = 0.f },
        .size = (Vector2f) {
            .x = 1000.f,
            .y = 50.f
        }
    };
    s->floor.body->position = (Vector2f) {
        .x = ENGINE->window.size.x / 2.f,
        .y = ENGINE->window.size.y - 20.f - s->floor.body->collider.bound.shape.aabb.size.y
    };
    s->floor.body->mass = INFINITY;

    s->rectangles = VECTOR(Rectangle);

    Rectangle r = (Rectangle) {
        .body = world_create_rigid_body(&s->dynamics)
    };
    r.body->material = RIGID_BODY_MATERIAL_ALUMINIUM;
    r.body->collider.bound.shape.aabb = (ShapeAABB) {
        .position = (Vector2f) { .x = 0.f, .y = 0.f },
        .size = (Vector2f) {
            .x = 100.f,
            .y = 100.f
        }
    };
    r.body->position = (Vector2f) {
        .x = 0.f,
        .y = 200.f
    };
    r.body->velocity.x = 80.f;
    r.body->velocity.y = 5.f;
    r.body->mass = 1.f;

    VECTOR_PUSH(Rectangle, &s->rectangles, r);

    RigidBody* p = r.body;
    r.body = world_create_rigid_body(&s->dynamics);
    uint64_t id = r.body->_id;
    *r.body = *p;
    r.body->_id = id;
    r.body->position = (Vector2f) {
        .x = 1280.f - 60.f,
        .y = 200.f
    };
    r.body->velocity.x = -40.f;
    r.body->velocity.y = 100.f;
    r.body->mass = 5.f;

    VECTOR_PUSH(Rectangle, &s->rectangles, r);
}

void physics_pop(struct GameState* state) {
    PhysicsTestState* s = (PhysicsTestState*)state->stored_state;

    free(s);
}

void physics_update(struct GameState* state) {
    //PhysicsTestState* s = (PhysicsTestState*)state->stored_state;
}

void physics_fixed_update(struct GameState* state, float delta_time) {
    PhysicsTestState* s = (PhysicsTestState*)state->stored_state;
    dynamics_world_step(&s->dynamics, delta_time);

    Rectangle *r0 = &((Rectangle*)s->rectangles.buffer)[0];
    Rectangle *r1 = &((Rectangle*)s->rectangles.buffer)[1];

    collide_bodies(r0->body, r1->body);

    collide_bodies(r0->body, s->floor.body);
    collide_bodies(r1->body, s->floor.body);
}

void physics_render(struct GameState* state) {
    PhysicsTestState* s = (PhysicsTestState*)state->stored_state;

    ColorRGB colors[3] = {
        hex_to_rgb("0xAA0000"),
        hex_to_rgb("0x00AA00"),
        hex_to_rgb("0x0000AA")
    };

    for (int i = 0; i < s->rectangles.length; i++) {
        Rectangle r = _VECTOR_GET(Rectangle, &s->rectangles, i);
        debug_rectangle(&s->renderer, (DebugShapeRectangle) {
            .position = r.body->position,
            .colour = colors[i % 3],
            .dimensions = r.body->collider.bound.shape.aabb.size,
            .thickness = 1.f
        });
    }

    debug_rectangle(&s->renderer, (DebugShapeRectangle) {
        .position = s->floor.body->position,
        .colour = hex_to_rgb("0xFFFFFF"),
        .dimensions = s->floor.body->collider.bound.shape.aabb.size,
        .thickness = 1.f
    });

    draw_debug(&s->renderer, s->camera, s->projection);
}

