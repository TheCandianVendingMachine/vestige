#version 410 core

#define MAX_RESOURCE_IN_PASS 128

uniform vec2 dimensions[MAX_RESOURCE_IN_PASS];
uniform vec2 positions[MAX_RESOURCE_IN_PASS];
uniform vec3 colours[MAX_RESOURCE_IN_PASS];
uniform float thicknesses[MAX_RESOURCE_IN_PASS];
uniform mat4 view;
uniform mat4 projection;

out vec3 Colour;
out vec2 Dimensions;
out vec2 ScreenPosition;
out float Thickness;

void main() {
    vec2 dimensions = dimensions[gl_InstanceID];
    vec2 position = positions[gl_InstanceID];

    float l = sqrt(dimensions.x);
    vec2 VERTEX_OFFSETS[3] = vec2[3](
        vec2(0.0, -(0.5 * dimensions.y + (dimensions.x * dimensions.y / l))),
        vec2(l + 0.5 * dimensions.x, 0.5 * dimensions.y),
        vec2(-(l + 0.5 * dimensions.x), 0.5 * dimensions.y)
    );
    vec2 offset = VERTEX_OFFSETS[gl_VertexID];

    vec2 vPos = position + offset * dimensions;

    gl_Position = projection * view * vec4(vPos.x, vPos.y, 0.0, 1.0);
    ScreenPosition = offset;
    Thickness = thicknesses[gl_InstanceID];
    Colour = colours[gl_InstanceID];
    Dimensions = vec2(0.5);
}
