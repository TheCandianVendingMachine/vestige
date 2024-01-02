#version 410 core

#define MAX_RESOURCE_IN_PASS 256

uniform float radii[MAX_RESOURCE_IN_PASS];
uniform vec2 positions[MAX_RESOURCE_IN_PASS];
uniform mat4 view;
uniform mat4 projection;

const vec2 VERTEX_OFFSETS[3] = vec2[3](
    vec2(-1.7321, -1.0),
    vec2(1.7321, -1.0),
    vec2(0.0, 2.0)
);

out vec2 ScreenPosition;

void main() {
    float radius = radii[gl_InstanceID];
    vec2 position = positions[gl_InstanceID];
    vec2 offset = VERTEX_OFFSETS[gl_VertexID];

    vec2 vPos = position + offset * radius;

    gl_Position = projection * view * vec4(vPos.x, vPos.y, 0.0, 1.0);
    ScreenPosition = VERTEX_OFFSETS[gl_VertexID];
}
