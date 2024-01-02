#include <stdlib.h>

#include <glad/glad.h>

#include "engine.h"
#include "game/bots/game.h"
#include "game/bots/resource_map.h"
#include "game/bots/resource.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_GAME
#include "logger.h"
#include "ui/text.h"
#include "transform.h"
#include "render/shader.h"
#include "lib/math.h"

ResourceMap create_resource_map(struct FontEngine* font_engine) {
    ResourceMap map;
    map.resources = malloc(sizeof(Vector) * RESOURCE_TYPE_COUNT);
    for (int i = 0; i < RESOURCE_TYPE_COUNT; i++) {
        map.resources[i] = VECTOR(Resource);
    }

    Shader vs = load_vertex_shader_from_disk("shaders/resource.vert").data.ok;
    Shader fs = load_fragment_shader_from_disk("shaders/resource.frag").data.ok;
    map._render.render_resources = create_shader_program(vs, fs).data.ok;


    return map;
}

void destroy_resource_map(ResourceMap* resource_map) {
    for (int i = 0; i < RESOURCE_TYPE_COUNT; i++) {
        Vector resources = resource_map->resources[i];
        del_vector(resources);
    }
    free(resource_map->resources);
}

void resource_map_render(GameplayState* state, ResourceMap* resource_map) {
    float radii[128];
    Vector2f positions[128];
    for (int i = 0; i < RESOURCE_TYPE_COUNT; i++) {
        Vector resources = resource_map->resources[i];
        for (int j = 0; j < resources.length; j++) {
        }
    }

    unsigned int shader = resource_map->_render.render_resources._program;
    int projectionPosition = glGetUniformLocation(shader, "projection");
    int viewPosition = glGetUniformLocation(shader, "view");
    int radiiPosition = glGetUniformLocation(shader, "radii");
    int positionPosition = glGetUniformLocation(shader, "positions");

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glUseProgram(shader);
    glUniformMatrix4fv(viewPosition, 1, false, camera_view(&state->current_scene.camera).entries);
    glUniformMatrix4fv(projectionPosition, 1, false, state->projection.entries);
    int max = 128 * 1000 + 123;
    for (int i = 0; i < max; i += 128) {
        int count = 128;
        if (i + 128 >= max) {
            count = max - i;
        }
        for (int j = 0; j < count; j++) {
            radii[j] = frandrange(50.f, 720.f);
            float x = frandrange(-10000.f, 10000.f);
            float y = frandrange(-10000.f, 10000.f);
            positions[j] = (Vector2f) { .x = x, .y = y };
        }
        glUniform2fv(positionPosition, 1, positions->entries);
        glUniform1fv(radiiPosition, 1, radii);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 3, count);
    }
}
