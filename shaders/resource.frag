#version 410 core

in vec2 ScreenPosition;

out vec4 FragColor;

void main() {
    float d = dot(ScreenPosition, ScreenPosition);
    if (d > 1.0) {
        discard;
    }
    FragColor = vec4(d, 0, 0, 1);
}

