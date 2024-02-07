#version 410 core

in vec3 Colour;
in vec2 ScreenPosition;
in vec2 Dimensions;
in float Thickness;
out vec4 FragColor;

void main() {
    vec2 a = abs(ScreenPosition) - Dimensions;
    float d = length(max(a, 0.0)) + min(max(a.x, a.y), 0.0);
    if (d > 0.0 || d < -max(Dimensions.x, Dimensions.y) * Thickness) {
        discard;
    }
    FragColor = vec4(Colour, 1);
}

