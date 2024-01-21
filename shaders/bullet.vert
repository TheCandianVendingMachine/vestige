#version 410 core

#define MAX_RESOURCE_IN_PASS 256

layout (location = 0) in vec3 vPos;

uniform vec2 positions[MAX_RESOURCE_IN_PASS];
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec2 pos = positions[gl_InstanceID] + vPos.xy * 50.0;
    gl_Position = projection * view * vec4(pos.x, pos.y, 0.0, 1.0);
}
