#include "transform.h"

Transform new_transform(void) {
    return (Transform) {
        .position = (Vector3f) { .x = 0.f, .y = 0.f, .z = 0.f },
        .size = (Vector3f) { .x = 1.f, .y = 1.f, .z = 1.f },
        ._updated = false
    };
}

void transform_set_position(Transform* transform, Vector3f position) {
    transform->position = position;
    transform->_updated = true;
}

void transform_set_size(Transform* transform, Vector3f size) {
    transform->size = size;
    transform->_updated = true;
}

Matrix4f transform_get_model(Transform* transform) {
    if (transform->_updated) {
        float sx = transform->size.x;
        float sy = transform->size.y;
        float sz = transform->size.z;
        float tx = transform->position.x;
        float ty = transform->position.y;
        float tz = transform->position.z;
        float m[] = {
            sx, 0.f, 0.f, 0.f,
            0.f, sy, 0.f, 0.f,
            0.f, 0.f, sz, 0.f,
            tx,  ty,  tz, 1.f
        };
        memcpy(transform->_transform.entries, m, sizeof(m));
        transform->_updated = false;
    }
    return transform->_transform;
}
