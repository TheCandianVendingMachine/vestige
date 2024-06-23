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
    if (b1->particle.mass == INFINITY) { return; }
    float amount = length_vector2f(b1->particle.velocity) / length_vector2f(b2.particle.velocity);
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
    b1->particle.position = sub_vector2f(b1->particle.position, mul_vector2f(info.minimum_translation_vector, amount));

    Vector2f p1 = b1->particle.position;
    Vector2f p2 = b2.particle.position;

    Vector2f v1 = b1->particle.velocity;
    Vector2f v2 = b2.particle.velocity;

    // conserve energy and momentum in a collision
    float inverse_mass_sum = 1.f / (b1->particle.mass + b2.particle.mass);
    Vector2f collision_axis = normalise_vector2f(info.minimum_translation_vector);
    {
        Vector2f dp = sub_vector2f(p1, p2);
        Vector2f dv = sub_vector2f(v1, v2);
        if (b2.particle.mass == INFINITY) {
            Vector2f dv_proj = project_vector2f(dv, collision_axis);
            float inverse_mass = b1->particle.restitution / 2.f + b2.particle.restitution;
            b1->particle.velocity = sub_vector2f(b1->particle.velocity, mul_vector2f(dv_proj, inverse_mass));
        } else {
            Vector2f dv_proj = project_vector2f(dv, dp);
            float inverse_mass = (b1->particle.restitution + b2.particle.restitution) * b2.particle.mass * inverse_mass_sum;
            b1->particle.velocity = sub_vector2f(b1->particle.velocity, mul_vector2f(dv_proj, inverse_mass));
        }
    }

    {
        log_debug("%f %f", b1->particle.acceleration.x, b1->particle.acceleration.y);
        Vector2f normal_accel = project_vector2f(b1->particle.acceleration, collision_axis);
        Vector2f gravity_accel = project_vector2f(mul_vector2f(GRAVITY, -1.f), collision_axis);
        normal_accel = add_vector2f(normal_accel, gravity_accel);
        b1->particle.normal_force = add_vector2f(b1->particle.normal_force, mul_vector2f(normal_accel, -b1->particle.mass));
        b1->particle.normal_force = add_vector2f(b1->particle.normal_force, project_vector2f(b1->particle.impulse, collision_axis));
    }
}

void collide_bodies(RigidBody* b1, RigidBody* b2) {
    OPTION_ASSIGN(b1->collider).position = b1->particle.position;
    OPTION_ASSIGN(b2->collider).position = b2->particle.position;
    CollisionInfo collision = collider_test_collision(OPTION_UNWRAP(Collider, b1->collider), OPTION_UNWRAP(Collider, b2->collider));
    if (!collision.collides) {
        return;
    }

    RigidBody b1_prev = *b1;
    RigidBody b2_prev = *b2;

    resolve_collision(b1, b2_prev, collision);
    collision.minimum_translation_vector = mul_vector2f(collision.minimum_translation_vector, -1.f);
    resolve_collision(b2, b1_prev, collision);

    float friction_coef = FRICTION_COEFFICIENT_TABLE[b1->particle.material][b2->particle.material];
    b1->particle.friction_force = add_vector2f(b1->particle.friction_force, mul_vector2f(b1->particle.normal_force, friction_coef));
    b2->particle.friction_force = add_vector2f(b2->particle.friction_force, mul_vector2f(b2->particle.normal_force, friction_coef));
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
    s->floor.body->particle = create_particle();
    s->floor.body->particle.restitution = 0.1f;
    s->floor.body->flags = SIMULATION_DISABLE_GRAVITY;
    s->floor.body->particle.material = PARTICLE_MATERIAL_ICE;
    OPTION_ASSIGN(s->floor.body->collider).bound.shape.aabb = (ShapeAABB) {
        .position = (Vector2f) { .x = 0.f, .y = 0.f },
        .size = (Vector2f) {
            .x = 1000.f,
            .y = 50.f
        }
    };
    s->floor.body->particle.position = (Vector2f) {
        .x = ENGINE->window.size.x / 2.f,
        .y = ENGINE->window.size.y - 20.f - s->floor.body->collider.data.bound.shape.aabb.size.y
    };
    s->floor.body->particle.mass = INFINITY;

    s->rectangles = VECTOR(Rectangle);

    /*Rectangle r = (Rectangle) {
        .body = world_create_rigid_body(&s->dynamics)
    };
    r.body->particle = create_particle();
    r.body->particle.restitution = 0.2f;
    r.body->particle.material = PARTICLE_MATERIAL_ALUMINIUM;
    OPTION_ASSIGN(r.body->collider).bound.shape.aabb = (ShapeAABB) {
        .position = (Vector2f) { .x = 0.f, .y = 0.f },
        .size = (Vector2f) {
            .x = 100.f,
            .y = 100.f
        }
    };
    r.body->particle.position = (Vector2f) {
        .x = 0.f,
        .y = 200.f
    };
    r.body->particle.velocity.x = 80.f;
    r.body->particle.velocity.y = 5.f;
    r.body->particle.mass = 1.f;

    VECTOR_PUSH(Rectangle, &s->rectangles, r);

    RigidBody* p = r.body;
    r.body = world_create_rigid_body(&s->dynamics);
    r.body->particle = create_particle();
    r.body->particle.restitution = 0.4f;
    uint64_t id = r.body->_id;
    *r.body = *p;
    r.body->_id = id;
    r.body->particle.position = (Vector2f) {
        .x = 1280.f - 60.f,
        .y = 200.f
    };
    r.body->particle.velocity.x = -40.f;
    r.body->particle.velocity.y = 100.f;
    r.body->particle.mass = 5.f;

    VECTOR_PUSH(Rectangle, &s->rectangles, r);*/

    for (int i = 0; i < 5; i++) {
        Rectangle r = (Rectangle) {
            .body = world_create_rigid_body(&s->dynamics)
        };
        r.body->particle = create_particle();
        r.body->particle.restitution = 0.0f;
        r.body->particle.material = PARTICLE_MATERIAL_ALUMINIUM;
        OPTION_ASSIGN(r.body->collider).bound.shape.aabb = (ShapeAABB) {
            .position = (Vector2f) { .x = 0.f, .y = 0.f },
            .size = (Vector2f) {
                .x = 100.f,
                .y = 100.f
            }
        };
        r.body->particle.position = (Vector2f) {
            .x = 200.f + 10 * sin(i),
            .y = 550.f - i * 105.f
        };
        r.body->particle.velocity.x = 0.f;
        r.body->particle.velocity.y = 0.f;
        r.body->particle.mass = 10.f;

        VECTOR_PUSH(Rectangle, &s->rectangles, r);
    }

    Circle r = (Circle) {
        .body = world_create_rigid_body(&s->dynamics)
    };
    r.body->particle = create_particle();
    r.body->particle.restitution = 0.0f;
    r.body->particle.material = PARTICLE_MATERIAL_ALUMINIUM;
    OPTION_ASSIGN(r.body->collider).bound.shape.circle = (ShapeCircle) {
        .position = (Vector2f) { .x = 0.f, .y = 0.f },
        .radius = 20.f
    };
    r.body->particle.position = (Vector2f) {
        .x = 1000.f,
        .y = 200.f
    };
    r.body->particle.velocity.x = -8.f;
    r.body->particle.velocity.y = 0.f;
    r.body->particle.mass = 50.f;
    r.body->particle.restitution = 1.f;

    s->circles = VECTOR(Circle);
    VECTOR_PUSH(Circle, &s->circles, r);
}

void physics_pop(struct GameState* state) {
    PhysicsTestState* s = (PhysicsTestState*)state->stored_state;

    free(s);
}

void physics_update(struct GameState* state) {
    PhysicsTestState* s = (PhysicsTestState*)state->stored_state;
    dynamics_world_pre_step(&s->dynamics);
}

void physics_fixed_update(struct GameState* state, float delta_time) {
    PhysicsTestState* s = (PhysicsTestState*)state->stored_state;
    dynamics_world_step(&s->dynamics, delta_time);

    for (int i = 0; i < s->rectangles.length; i++) {
        Rectangle *r0 = &((Rectangle*)s->rectangles.buffer)[i];
        for (int j = i + 1; j < s->rectangles.length; j++) {
            Rectangle *r1 = &((Rectangle*)s->rectangles.buffer)[j];
            collide_bodies(r0->body, r1->body);
            collide_bodies(r1->body, s->floor.body);
        }
        collide_bodies(r0->body, s->floor.body);

        for (int j = 0; j < s->circles.length; j++) {
            Circle *r1 = &((Circle*)s->circles.buffer)[j];
            collide_bodies(r0->body, r1->body);
            collide_bodies(r1->body, s->floor.body);
        }
    }

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
            .position = r.body->particle.position,
            .colour = colors[i % 3],
            .dimensions = OPTION_UNWRAP(Collider, r.body->collider).bound.shape.aabb.size,
            .thickness = 1.f
        });
    }

    debug_rectangle(&s->renderer, (DebugShapeRectangle) {
        .position = s->floor.body->particle.position,
        .colour = hex_to_rgb("0xFFFFFF"),
        .dimensions = OPTION_UNWRAP(Collider, s->floor.body->collider).bound.shape.aabb.size,
        .thickness = 1.f
    });

    for (int i = 0; i < s->circles.length; i++) {
        Circle r = _VECTOR_GET(Circle, &s->circles, i);
        debug_circle(&s->renderer, (DebugShapeCircle) {
            .position = r.body->particle.position,
            .colour = colors[i % 3],
            .radius = OPTION_UNWRAP(Collider, r.body->collider).bound.shape.circle.radius,
            .thickness = 1.f
        });
    }

    dynamics_world_debug_draw(&s->dynamics, &s->renderer);
    draw_debug(&s->renderer, s->camera, s->projection);
}

