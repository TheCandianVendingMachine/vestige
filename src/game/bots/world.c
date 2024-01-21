#include "game/bots/world.h"
#include "lib/math.h"
#include "game/bots/bot.h"
#include "game/bots/bullet.h"
#include "game/bots/game.h"
#include "render/camera.h"
#include "render/primitives.h"


#include "math.h"
#include "logger.h"

World new_world(struct FontEngine* font_engine) {
    World world;
    world.resources = create_resource_map(font_engine);
    world.bullet_manager = create_bullet_manager();

    for (int i = 0; i < 10; i++) {
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

    world.bots = new_colony(sizeof(Bot));
    size_t id;
    {
        Bot b = create_bot();
        id = colony_insert(&world.bots, &b);
    }
    Bot* b = (Bot*)colony_get(world.bots, id);
    b->id = id;
    transform_set_size(&b->transform, (Vector3f) { .x = 500.f, .y = 1000.f, .z = 0.f });

    Shader vs = load_vertex_shader_from_disk("shaders/bot.vert").data.ok;
    Shader fs = load_fragment_shader_from_disk("shaders/bot.frag").data.ok;
    world.bot_render_data.shader = create_shader_program(vs, fs).data.ok;

    glGenVertexArrays(1, &world.bot_render_data.vao);
    bind_primitive_to_vao(primitive_quad(), world.bot_render_data.vao);

    world.fire_time = new_clock();

    return world;
}

void update_world(World* world) {
    // Update bot's brain
    COLONY_ITER_BEGIN(Bot, world->bots);
    bot_think(world, i);
    COLONY_ITER_END;

    if (time_as_seconds(get_elapsed_time(&world->fire_time)) > 1.f / 75.f) {
        Bot b = *(Bot*)colony_get(world->bots, 0);
        float speed = 4000.f;
        Vector2f origin = (Vector2f) { .x = -10000.f, .y = -35000.f };

        Vector2f target = b.physics.position;
        float time_to_target = 4.f;
        // Compare how long it takes for our bullet to get there vs target
        // Adjusted based on this
        // Integrate target acceleration to get velocity
        Vector2f position = b.physics.position;
        Vector2f velocity = b.physics.velocity;
        const int max_integration_steps = 1000;
        const float dt = time_to_target / max_integration_steps;

        float min_dt = 1e10;
        float t = 0.f;
        for (int j = 0; j < max_integration_steps; j++) {
            velocity = add_vector2f(velocity, mul_vector2f(b.physics.acceleration, dt));
            position = add_vector2f(position, mul_vector2f(velocity, dt));

            float target_distance = distance_vector2f(origin, position);
            float bullet_tti = target_distance / speed;

            t += dt;

            float test_dt = fabs(t - bullet_tti);
            if (test_dt < min_dt) {
                target = position;
                min_dt = test_dt;
            }
        }

        if (min_dt < 0.3f) {
            world->fire_time = new_clock();
            bullet_manager_fire(
                &world->bullet_manager,
                origin,
                normalise_vector2f(sub_vector2f(target, origin)),
                speed
            );
        }
    }
}

void update_world_fixed(World* world, float delta_time) {
    COLONY_ITER_BEGIN(Bot, world->bots);
    bot_controller(i, delta_time);
    COLONY_ITER_END;

    COLONY_ITER_BEGIN(Bot, world->bots);
    bot_physics(i, delta_time);
    COLONY_ITER_END;

    bullet_manager_fixed_update(&world->bullet_manager, delta_time);
}

void render_world(struct GameplayState* state, World* world) {
    resource_map_render(state, &world->resources);

    {
        unsigned int shader = world->bot_render_data.shader._program;
        int modelPosition = glGetUniformLocation(shader, "model");
        int projectionPosition = glGetUniformLocation(shader, "projection");
        int viewPosition = glGetUniformLocation(shader, "view");

        glUseProgram(shader);
        glUniformMatrix4fv(viewPosition, 1, false, camera_view(state->current_scene.camera).entries);
        glUniformMatrix4fv(projectionPosition, 1, false, state->projection.entries);

        COLONY_ITER_BEGIN(Bot, world->bots);
        glUniformMatrix4fv(modelPosition, 1, false, transform_get_model(&i->transform).entries);
        draw_primitive(primitive_quad(), world->bot_render_data.vao);
        COLONY_ITER_END;
    }

    bullet_manager_render(&world->bullet_manager, state);
}
