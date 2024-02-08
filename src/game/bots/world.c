#include "game/bots/world.h"
#include "lib/math.h"
#include "game/bots/bot.h"
#include "game/bots/bullet.h"
#include "game/bots/game.h"
#include "render/camera.h"
#include "render/primitives.h"

#include "game/bots/predictors.h"
#include "game/bots/missile.h"
#include "math.h"
#include "logger.h"

World new_world(struct FontEngine* font_engine) {
    World world;
    world.resources = create_resource_map(font_engine);
    world.bullet_manager = create_bullet_manager();
    world.missile_manager = create_missile_manager();

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
    world.fired = 0;

    world.debug_renderer = new_debug_renderer();

    return world;
}

void update_world(World* world) {
    // Update bot's brain
    COLONY_ITER_BEGIN(Bot, world->bots);
    bot_think(world, i);
    COLONY_ITER_END;

    if (time_as_seconds(get_elapsed_time(&world->fire_time)) > 3.f && world->fired == 0) {
        world->fired = 1;
        Bot* b = (Bot*)colony_get(world->bots, 0);
        Vector2f origin = (Vector2f) { .x = -55000.f, .y = -15000.f };

        Missile m;
        m.direction = (Vector2f) { .x = 1.f, .y = 0.f };
        m.physics.position = origin;
        m.physics.velocity = (Vector2f) { .x = 0.f, .y = 0.f };
        m.motor_direction = normalise_vector2f((Vector2f) { .x = 1, .y = -1.f });
        m.dry_mass = 500.f;
        m.motor.fuel_mass = 60.f;
        m.motor.burn_rate = 60.f / 120.f;
        m.motor.thrust = 300.f * 1000.f;

        m.seeker.target = &b->physics.position;
        m.seeker.last_distance = length_vector2f(sub_vector2f(*m.seeker.target, m.physics.position));

        m.guidance.last_los = normalise_vector2f(sub_vector2f(*m.seeker.target, m.physics.position));
        m.guidance.gain = 3;

        float kp = 10.f;
        float ki = 1.f;

        m.autopilot.integral_control[0] = new_pid(0.f, 0.f, ki);
        m.autopilot.integral_control[1] = new_pid(0.f, 0.f, ki);

        m.autopilot.proportional_control[0] = new_pid(kp, 0.f, 0.f);
        m.autopilot.proportional_control[1] = new_pid(kp, 0.f, 0.f);

        colony_insert(&world->missile_manager.missiles, &m);
        log_info("Fired!");
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
    missile_manager_fixed_update(&world->missile_manager, delta_time);
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
    missile_manager_render(&world->missile_manager, state);

    draw_debug(&world->debug_renderer, *state->current_scene.camera, state->projection);
}
