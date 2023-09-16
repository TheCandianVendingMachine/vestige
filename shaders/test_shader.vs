#version 410 core

layout (location = 0) in vec3 vPos;
layout (location = 0) in vec4 vColor;

out vec4 vColorOut;

void main()
{
    gl_Position = vec4(vPos.x, vPos.y, vPos.z, 1.0);
    vColorOut = vColor;
}
