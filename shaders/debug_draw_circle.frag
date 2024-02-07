#version 410 core

in vec3 Colour;
in vec2 ScreenPosition;
in float Thickness;
out vec4 FragColor;

void main() {
    float d = dot(ScreenPosition, ScreenPosition);
    if (d > 1.0 || sqrt(d) < (1.0 - Thickness)) {
        discard;
    }
    FragColor = vec4(Colour, 1);
}

