#version 410 core

in vec2 ScreenPosition;
in vec2 Origin;
in vec2 Direction;
in vec3 Colour;
in float Thickness;
in float Length;
out vec4 FragColor;

void main() {
    float t = dot(Direction, (ScreenPosition - Origin)) / dot(Direction, Direction);
    float d;
    if (t < 0) {
        d = distance(ScreenPosition, Origin);
    } else if (t >= Length) {
        d = distance(ScreenPosition, Origin + Length * Direction);
    } else {
        d = length(ScreenPosition - (Origin + t * Direction));
    }

    if (d > 100.0 * Thickness) {
        discard;
    }
    FragColor = vec4(Colour, 1);
}

