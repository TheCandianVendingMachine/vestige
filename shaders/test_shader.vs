#version 410 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec4 vColor;

out vec4 vFragColor;

void main()
{
    gl_Position = vec4(vPos.x, vPos.y, vPos.z, 1.0);
    vFragColor = vColor;
}
