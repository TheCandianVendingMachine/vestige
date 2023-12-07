#version 410 core

layout (location = 0) in vec3 vPos;
uniform mat4 projection;

void main() {
    gl_Position = vec4(vPos.x * 2 - 1, vPos.y * 2 - 1, vPos.z, 1.0);
}
