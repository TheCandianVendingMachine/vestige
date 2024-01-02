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

void add_resource(ResourceMap* resource_map, Resource resource) {
    Vector* resources = &resource_map->resources[resource.type];
    VECTOR_PUSH(Resource, resources, resource);
}

void resource_map_render(GameplayState* state, ResourceMap* resource_map) {
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

    float radii[128];
    Vector2f positions[128];
    for (int i = 0; i < RESOURCE_TYPE_COUNT; i++) {
        Vector resources = resource_map->resources[i];
        for (int j = 0; j < resources.length; j += 128) {
            int count = 128;
            if (j + 128 >= resources.length) {
                count = resources.length - j;
            }
            for (int j = 0; j < count; j++) {
                Resource resource = _VECTOR_GET(Resource, &resources, i + j);
                float percent_left = (float)resource.remaining / resource.initial;
                radii[j] = 50.f + percent_left * 450.f;
                positions[j] = resource.position;
            }
            glUniform2fv(positionPosition, 1, positions->entries);
            glUniform1fv(radiiPosition, 1, radii);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 3, count);
        }
    }
}
