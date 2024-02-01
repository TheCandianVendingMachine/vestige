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
void missile_manager_fixed_update(MissileManager* missile_manager, float delta_time) {
    // Update guidance
    COLONY_ITER_BEGIN(Missile, missile_manager->missiles);
    Vector2f los_direction = normalise_vector2f(sub_vector2f(*i->seeker.target, i->position));
    log_debug("%f %f", los_direction.x, los_direction.y);
    log_debug("%f %f", i->seeker.target->x, i->seeker.target->y);
    log_debug("%f %f", i->position.x, i->position.y);
    i->guidance.last_los = los_direction;
    COLONY_ITER_END;


    // Update physics
    COLONY_ITER_BEGIN(Missile, missile_manager->missiles);
    if (i->motor.fuel_mass <= 0.f) {
        i->motor.fuel_mass = 0.f;
    } else {
        i->motor.fuel_mass -= i->motor.burn_rate * delta_time;
        float total_mass = i->dry_mass + i->motor.fuel_mass;
        float acceleration = i->motor.thrust / total_mass;
        Vector2f acceleration_v = mul_vector2f(i->motor_direction, acceleration);
        i->velocity = add_vector2f(i->velocity, mul_vector2f(acceleration_v, delta_time));
        i->direction = normalise_vector2f(i->velocity);
    }
    i->position = add_vector2f(i->position, mul_vector2f(i->velocity, delta_time));
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
            positions[j] = missile.position;
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
