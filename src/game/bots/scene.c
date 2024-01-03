#include "game/bots/scene.h"
#include "game/bots/game.h"

Scene create_scene(struct GameplayState* state) {
    Scene scene;
    scene.world = new_world(&state->font_engine);
    scene.camera = malloc(sizeof(Camera));
    *scene.camera = new_camera();
    scene.camera_controller = create_camera_controller(scene.camera);
    return scene;
}

void update_scene(Scene* scene, float delta_time) {

}

void render_scene(struct GameplayState* state, Scene* scene) {
    render_world(state, &scene->world);
}
