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

    radii[0] = 200.f;
    radii[1] = 100.f;
    radii[2] = 100.f;
    positions[0] = (Vector2f) { .x = 200.f, .y = 200.f };
    positions[1] = (Vector2f) { .x = 400.f, .y = 200.f };
    positions[2] = (Vector2f) { .x = 200.f, .y = 400.f };

    unsigned int shader = resource_map->_render.render_resources._program;
    int projectionPosition = glGetUniformLocation(shader, "projection");
    int viewPosition = glGetUniformLocation(shader, "view");
    int radiiPosition = glGetUniformLocation(shader, "radii");
    int positionPosition = glGetUniformLocation(shader, "positions");

    glUseProgram(shader);
    glUniformMatrix4fv(viewPosition, 1, false, camera_view(&state->current_scene.camera).entries);
    glUniformMatrix4fv(projectionPosition, 1, false, state->projection.entries);
    glUniform2fv(positionPosition, 1, positions->entries);
    glUniform1fv(radiiPosition, 1, radii);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 1);
}
