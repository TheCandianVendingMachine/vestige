#version 410 core

in vec2 ScreenPosition;
in vec2 Origin;
in vec2 Direction;
in vec3 Colour;
in float Thickness;
in float Length;
out vec4 FragColor;

void main() {
    // Get distance from screen position to ray,
    // if within thickness, draw

    float t = dot(Direction, (ScreenPosition - Origin)) / dot(Direction, Direction);
    float d = length(ScreenPosition - (Origin + t * Direction));
    if (t < 0) {
        d = distance(ScreenPosition, Origin);
    }
    if (t >= Length || d > 100.0 * Thickness) {
        discard;
    }
    FragColor = vec4(Colour, 1);
}

