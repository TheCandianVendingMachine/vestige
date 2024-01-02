#version 410 core

uniform vec3 colour;

in vec2 ScreenPosition;
out vec4 FragColor;

const float distance_threshold = 0.75;

void main() {
    float d = dot(ScreenPosition, ScreenPosition);
    if (d > 1.0) {
        discard;
    }
    float a = 1.0;
    if (d > distance_threshold) {
        a = 1.0 - ((d - distance_threshold) / (1.0 - distance_threshold));
    }
    FragColor = vec4(colour, a);
}

