#include "game/bots/scene.h"

void update_scene(Scene* scene, float delta_time) {

}

void render_scene(struct GameplayState* state, Scene* scene) {
    render_world(state, &scene->world);
}
