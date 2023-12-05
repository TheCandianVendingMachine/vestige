#version 410 core

layout (location = 0) in vec3 vPos;
uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(vPos.x, vPos.y, vPos.z, 1.0);
}