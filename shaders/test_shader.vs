#version 410 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec4 vColor;

out vec4 vFragColor;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(vPos.x, vPos.y, vPos.z, 1.0);
    vFragColor = vColor;
}
