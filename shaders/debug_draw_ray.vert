#version 410 core

#define MAX_RESOURCE_IN_PASS 128

uniform vec2 directions[MAX_RESOURCE_IN_PASS];
uniform vec2 positions[MAX_RESOURCE_IN_PASS];
uniform vec3 colours[MAX_RESOURCE_IN_PASS];
uniform float thicknesses[MAX_RESOURCE_IN_PASS];
uniform float lengths[MAX_RESOURCE_IN_PASS];
uniform mat4 view;
uniform mat4 projection;
uniform mat4 screenToWorld;

out vec3 Colour;
out vec2 Direction;
out vec2 Origin;
out vec2 ScreenPosition;
out float Thickness;
out float Length;

void main() {
    vec2 uv = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
    gl_Position = vec4(uv * vec2(2, -2) + vec2(-1, 1), 0.0, 1.0);

    ScreenPosition = (screenToWorld * gl_Position).xy;
    Origin = (vec4(positions[gl_InstanceID], 0.0, 0.0)).xy;
    Direction = directions[gl_InstanceID];
    Colour = colours[gl_InstanceID];
    Thickness = thicknesses[gl_InstanceID];
    Length = lengths[gl_InstanceID];
}
