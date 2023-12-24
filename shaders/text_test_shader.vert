#version 410 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vUV;
layout (location = 2) in vec3 vNormal;
layout (location = 3) in vec4 vColor;

out vec4 vFragColor;
out vec2 vTexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(vPos.x, vPos.y, vPos.z, 1.0);
    vFragColor = vColor;
    vTexCoord = vUV;
}
