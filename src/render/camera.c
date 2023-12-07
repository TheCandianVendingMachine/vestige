#include "render/camera.h"
#include "logger.h"

Camera new_camera(void) {
    return (Camera) {
        .position = (Vector3f) { { 0.f, 0.f, 0.f } },
        .up = (Vector3f) { { 0.f, 1.f, 0.f } },
        .direction = (Vector3f) { { 0.f, 0.f, 1.f } },
        ._updated = true
    };
}

Matrix4f camera_view(Camera* camera) {
    if (camera->_updated) {
        Vector3f f = normalise_vector3f(camera->direction);
        Vector3f u = normalise_vector3f(camera->up);
        Vector3f s = normalise_vector3f(cross_vector3f(f, u));
        u = cross_vector3f(s, f);

        float d0 = dot_vector3f(s, camera->position);
        float d1 = dot_vector3f(u, camera->position);
        float d2 = dot_vector3f(f, camera->position);

        camera->_projection = (Matrix4f) {
            {
                s.x, u.x, -f.x, 0.f,
                s.y, u.y, -f.y, 0.f,
                s.z, u.z, -f.z, 0.f,
                -d0, -d1,  d2,  1.f
            }
        };
        camera->_updated = false;
    }

    return camera->_projection;
}
