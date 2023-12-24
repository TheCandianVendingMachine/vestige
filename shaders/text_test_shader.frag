#version 410 core

in vec4 vFragColor;
in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D fontAtlas;

void main() {
    FragColor = texture(fontAtlas, vTexCoord);
}

