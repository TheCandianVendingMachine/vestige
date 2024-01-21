#include <assert.h>

#include "game/bots/bullet_manager.h"
#include "game/bots/bullet.h"
#include "game/bots/game.h"
#include "render/primitives.h"

BulletManager create_bullet_manager(void) {
    BulletManager bullets;

    Shader vs = load_vertex_shader_from_disk("shaders/bullet.vert").data.ok;
    Shader fs = load_fragment_shader_from_disk("shaders/bullet.frag").data.ok;
    bullets._render.shader = create_shader_program(vs, fs).data.ok;

    bullets.bullets = new_colony(sizeof(Bullet));
    glGenVertexArrays(1, &bullets._render.vao);
    bind_primitive_to_vao(&primitive_circle()->primitive, bullets._render.vao);

    bullets._clock = new_clock();

    return bullets;
}

void bullet_manager_fixed_update(BulletManager* bullets, float delta_time) {
    COLONY_ITER_BEGIN(Bullet, bullets->bullets);
    i->position = add_vector2f(i->position, mul_vector2f(i->velocity, delta_time));
    COLONY_ITER_END;

    COLONY_ITER_BEGIN(Bullet, bullets->bullets);
    Time time_alive = time_elapsed(i->init_time, get_elapsed_time(&bullets->_clock));
    if (time_as_seconds(time_alive) >= 4.f) {
        assert(colony_remove(&bullets->bullets, _i) != NULL);
    }
    COLONY_ITER_END;
}

void bullet_manager_render(BulletManager* bullets, struct GameplayState* state) {
    unsigned int shader = bullets->_render.shader._program;
    int positionPosition = glGetUniformLocation(shader, "positions");
    int projectionPosition = glGetUniformLocation(shader, "projection");
    int viewPosition = glGetUniformLocation(shader, "view");

    glUseProgram(shader);
    glBindVertexArray(bullets->_render.vao);

    glUniformMatrix4fv(viewPosition, 1, false, camera_view(state->current_scene.camera).entries);
    glUniformMatrix4fv(projectionPosition, 1, false, state->projection.entries);

    const int MAX_RESOURCES_IN_PASS = 256;
    Vector2f positions[MAX_RESOURCES_IN_PASS];
    for (int i = 0; i < bullets->bullets.length; i += MAX_RESOURCES_IN_PASS) {
        int count = MAX_RESOURCES_IN_PASS;
        if (i + MAX_RESOURCES_IN_PASS >= bullets->bullets.length) {
            count = bullets->bullets.length - i;
        }
        for (int j = 0; j < count; j++) {
            Bullet bullet = *(Bullet*)colony_get(bullets->bullets, i + j);
            positions[j] = bullet.position;
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

void bullet_manager_fire(BulletManager* bullets, Vector2f origin, Vector2f direction, float speed) {
    Bullet b;
    b.init_time = get_elapsed_time(&bullets->_clock);
    b.position = origin;
    b.velocity = mul_vector2f(direction, speed);

    colony_insert(&bullets->bullets, &b);
}
