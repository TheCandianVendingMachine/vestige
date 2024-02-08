#include <math.h>

#include "game/bots/missile_manager.h"
#include "game/bots/missile.h"
#include "game/bots/game.h"
#include "render/primitives.h"

MissileManager create_missile_manager(void) {
    MissileManager manager;
    manager.missiles = new_colony(sizeof(Missile));

    Shader vs = load_vertex_shader_from_disk("shaders/bullet.vert").data.ok;
    Shader fs = load_fragment_shader_from_disk("shaders/bullet.frag").data.ok;
    manager._render.shader = create_shader_program(vs, fs).data.ok;

    glGenVertexArrays(1, &manager._render.vao);
    bind_primitive_to_vao(&primitive_circle()->primitive, manager._render.vao);

    return manager;
}

#include "logger.h"
#include "game/bots/world.h"
#include "debug/render.h"
void missile_manager_fixed_update(World* world, MissileManager* missile_manager, float delta_time) {
    // Update guidance
    COLONY_ITER_BEGIN(Missile, missile_manager->missiles);
    Vector2f direction = sub_vector2f(*i->seeker.target, i->physics.position);
    Vector2f los_direction = normalise_vector2f(direction);
    Vector2f rotation_rate = mul_vector2f(sub_vector2f(los_direction, i->guidance.last_los), 1.f / delta_time);
    rotation_rate = mul_vector2f(rotation_rate, -1);
    rotation_rate = mul_vector2f(rotation_rate, (i->seeker.last_distance - length_vector2f(direction)) / delta_time);

    /*
        Rotation rate = angle/s
        3 * Rotation Rate = m/s
        rotation_rate * closing speed = angle * m / s^2
    */

    // Command acceleration is normal to LOS
    // just a change-of-basis matrix to convert coordinate space from local to global
    Matrix2f missile_to_world = inverse_mat2((Matrix2f) {
        .c1r1 = i->direction.x, .c2r1 = -i->direction.y,
        .c1r2 = i->direction.y, .c2r2 = i->direction.x
    });
    Vector2f direction_normal = (Vector2f) {
        .x = -i->direction.y,
        .y = i->direction.x
    };
    rotation_rate = mul_mat2vec2(missile_to_world, rotation_rate);
    /*Vector2f commanded_accel = mul_vector2f(i->direction, dot_vector2f(rotation_rate, i->direction));
    commanded_accel = sub_vector2f(rotation_rate, commanded_accel);*/
    Vector2f commanded_accel = project_vector2f(rotation_rate, direction_normal);

    i->guidance.last_los = los_direction;
    i->seeker.last_distance = length_vector2f(direction);

    i->autopilot.commanded_acceleration = commanded_accel;
    COLONY_ITER_END;

    // Update physics
    COLONY_ITER_BEGIN(Missile, missile_manager->missiles);
    if (i->motor.fuel_mass <= 0.f) {
        i->motor.fuel_mass = 0.f;
    } else {
        i->motor.fuel_mass -= i->motor.burn_rate * delta_time;
        float total_mass = i->dry_mass + i->motor.fuel_mass;
        float acceleration = i->motor.thrust / total_mass;
        i->physics.acceleration = mul_vector2f(i->motor_direction, acceleration);
        i->physics.velocity = add_vector2f(i->physics.velocity, mul_vector2f(i->physics.acceleration, delta_time));
        i->direction = normalise_vector2f(i->physics.velocity);
    }
    i->physics.position = add_vector2f(i->physics.position, mul_vector2f(i->physics.velocity, delta_time));
    COLONY_ITER_END;

    // Update autopilot
    COLONY_ITER_BEGIN(Missile, missile_manager->missiles);
    if (dot_vector2f(i->autopilot.commanded_acceleration, i->autopilot.commanded_acceleration) == 0.f) {
        continue;
    }
    Vector2f error = sub_vector2f(normalise_vector2f(i->autopilot.commanded_acceleration), normalise_vector2f(i->physics.acceleration));
    float gimbal_command_x = pid_step(&i->autopilot.integral_control[0], error.x, delta_time);
    float gimbal_command_y = pid_step(&i->autopilot.integral_control[1], error.y, delta_time);

    gimbal_command_x += pid_step(&i->autopilot.proportional_control[0], i->physics.acceleration.x, delta_time);
    gimbal_command_y += pid_step(&i->autopilot.proportional_control[1], i->physics.acceleration.y, delta_time);

    i->motor_direction.x = gimbal_command_x;
    i->motor_direction.y = gimbal_command_y;

    i->motor_direction = normalise_vector2f(i->motor_direction);

    debug_line(&world->debug_renderer, (DebugShapeLine) {
        .position = i->physics.position,
        .direction = normalise_vector2f(i->direction),
        .distance = 5000.f,
        .colour = hex_to_rgb("0x00FFFF"),
        .thickness = 1.25
    });

    debug_line(&world->debug_renderer, (DebugShapeLine) {
        .position = i->physics.position,
        .direction = normalise_vector2f(i->autopilot.commanded_acceleration),
        .distance = 10.f * length_vector2f(i->autopilot.commanded_acceleration),
        .colour = hex_to_rgb("0x0000FF"),
        .thickness = 1.25
    });

    debug_line(&world->debug_renderer, (DebugShapeLine) {
        .position = i->physics.position,
        .direction = normalise_vector2f(error),
        .distance = 10.f * length_vector2f(error),
        .colour = hex_to_rgb("0x90A000"),
        .thickness = 0.5
    });

    debug_line(&world->debug_renderer, (DebugShapeLine) {
        .position = i->physics.position,
        .direction = i->physics.acceleration,
        .distance = 10.f * length_vector2f(i->physics.acceleration),
        .colour = hex_to_rgb("0xFF0000"),
        .thickness = 1.0
    });

    printf("motor dir: (%f %f) | error: (%f %f) | accel: (%f %f) | desired: (%f %f)\n", i->motor_direction.x, i->motor_direction.y, error.x, error.y, i->physics.acceleration.x, i->physics.acceleration.y, i->autopilot.commanded_acceleration.x, i->autopilot.commanded_acceleration.y);

    COLONY_ITER_END;
}

void missile_manager_render(MissileManager* missiles, struct GameplayState* state) {
    unsigned int shader = missiles->_render.shader._program;
    int positionPosition = glGetUniformLocation(shader, "positions");
    int projectionPosition = glGetUniformLocation(shader, "projection");
    int viewPosition = glGetUniformLocation(shader, "view");

    glUseProgram(shader);
    glBindVertexArray(missiles->_render.vao);

    glUniformMatrix4fv(viewPosition, 1, false, camera_view(state->current_scene.camera).entries);
    glUniformMatrix4fv(projectionPosition, 1, false, state->projection.entries);

    const int MAX_RESOURCES_IN_PASS = 256;
    Vector2f positions[MAX_RESOURCES_IN_PASS];
    for (int i = 0; i < missiles->missiles.length; i += MAX_RESOURCES_IN_PASS) {
        int count = MAX_RESOURCES_IN_PASS;
        if (i + MAX_RESOURCES_IN_PASS >= missiles->missiles.length) {
            count = missiles->missiles.length - i;
        }
        for (int j = 0; j < count; j++) {
            Missile missile = *(Missile*)colony_get(missiles->missiles, i + j);
            positions[j] = missile.physics.position;
        }
        glUniform2fv(positionPosition, count, positions[0].entries);
        glDrawElementsInstanced(
            GL_TRIANGLES,
            primitive_circle()->primitive.index_count, GL_UNSIGNED_INT,
            0,
            count
        );
    }

    glBindVertexArray(0);
}
