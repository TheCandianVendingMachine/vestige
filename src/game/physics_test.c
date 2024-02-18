#include <stdlib.h>
#include <math.h>

#include "engine.h"
#include "logger.h"
#include "game/game_states.h"
#include "game/physics_test.h"

Vector2f GRAVITY = (Vector2f) { .x = 0.f, .y = 1.0f };

void simulate_body(RigidBody* body, float delta_time) {
    //body->acceleration = add_vector2f(body->acceleration, GRAVITY);
    rigid_body_step(body, delta_time);
}

DebugRender *DRENDER = NULL;

void collide_bodies(RigidBody* b1, RigidBody* b2) {
    b1->bounds.position = b1->position;
    b2->bounds.position = b2->position;

    if (!aabb_intersect(b1->bounds, b2->bounds)) {
        return;
    }

    Vector2f p1 = b1->position;
    Vector2f p2 = b2->position;

    Vector2f v1 = b1->velocity;
    Vector2f v2 = b2->velocity;

    // conserve energy and momentum in a collision
    float inverse_mass_sum = 1.f / (b1->mass + b2->mass);
    {
        Vector2f dp = sub_vector2f(p1, p2);
        Vector2f dv = sub_vector2f(v1, v2);
        Vector2f dv_proj = project_vector2f(dv, dp);
        if (b2->mass == INFINITY) {
            b1->velocity = sub_vector2f(b1->velocity, mul_vector2f(dv_proj, 1.f + b2->restitution));
        } else {
            float inverse_mass = (1.f + b2->restitution) * b2->mass * inverse_mass_sum;
            b1->velocity = sub_vector2f(b1->velocity, mul_vector2f(dv_proj, inverse_mass));
        }
    }

    {
        Vector2f dp = sub_vector2f(p2, p1);
        Vector2f dv = sub_vector2f(v2, v1);
        Vector2f dv_proj = project_vector2f(dv, dp);
        if (b1->mass == INFINITY) {
            b2->velocity = sub_vector2f(b2->velocity, mul_vector2f(dv_proj, 1.f + b1->restitution));
        } else {
            float inverse_mass = (1.f + b1->restitution) * b1->mass * inverse_mass_sum;
            b2->velocity = sub_vector2f(b2->velocity, mul_vector2f(dv_proj, inverse_mass));
        }
    }
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

    s->floor = (Floor) {
        .body = create_rigid_body()
    };
    s->floor.body.bounds = (AABB) {
        .position = (Vector2f) { .x = 0.f, .y = 0.f },
        .size = (Vector2f) {
            .x = 1000.f,
            .y = 50.f
        }
    };
    s->floor.body.position = (Vector2f) {
        .x = ENGINE->window.size.x / 2.f,
        .y = ENGINE->window.size.y - 20.f - s->floor.body.bounds.size.y
    };
    s->floor.body.mass = INFINITY;

    s->rectangles = VECTOR(Rectangle);

    Rectangle r = (Rectangle) {
        .body = create_rigid_body()
    };
    r.body.bounds = (AABB) {
        .position = (Vector2f) { .x = 0.f, .y = 0.f },
        .size = (Vector2f) {
            .x = 120.f,
            .y = 120.f
        }
    };
    r.body.position = (Vector2f) {
        .x = 0.f,
        .y = 200.f
    };
    r.body.velocity.x = 150.f;
    r.body.velocity.y = 5.f;
    r.body.mass = 1.f;

    VECTOR_PUSH(Rectangle, &s->rectangles, r);

    r.body.position = (Vector2f) {
        .x = 1280.f - 60.f,
        .y = 200.f
    };
    r.body.velocity.x = -50.f;
    r.body.velocity.y = 0.f;
    r.body.mass = 10.f;

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
    for (int i = 0; i < s->rectangles.length; i++) {
        Rectangle *r = &_VECTOR_GET(Rectangle, &s->rectangles, i);
        simulate_body(&r->body, delta_time);
    }

    collide_bodies(&((Rectangle*)s->rectangles.buffer)[0].body, &((Rectangle*)s->rectangles.buffer)[1].body);

    collide_bodies(&((Rectangle*)s->rectangles.buffer)[0].body, &s->floor.body);
    collide_bodies(&((Rectangle*)s->rectangles.buffer)[1].body, &s->floor.body);
}

void physics_render(struct GameState* state) {
    PhysicsTestState* s = (PhysicsTestState*)state->stored_state;

    for (int i = 0; i < s->rectangles.length; i++) {
        Rectangle r = _VECTOR_GET(Rectangle, &s->rectangles, i);
        debug_rectangle(&s->renderer, (DebugShapeRectangle) {
            .position = r.body.position,
            .colour = hex_to_rgb("0xAA0000"),
            .dimensions = r.body.bounds.size,
            .thickness = 1.f
        });
    }

    debug_rectangle(&s->renderer, (DebugShapeRectangle) {
        .position = s->floor.body.position,
        .colour = hex_to_rgb("0xFFFFFF"),
        .dimensions = s->floor.body.bounds.size,
        .thickness = 1.f
    });

    draw_debug(&s->renderer, s->camera, s->projection);
}

