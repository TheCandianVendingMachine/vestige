#version 430

in vec4 vFragColor;
out vec4 FragColor;

struct CellData {
    vec3 color;
};

struct Ray {
    vec3 position;
    vec3 direction;
};

uniform mat4 cameraProjection;
uniform mat4 inverseCameraProjection;
uniform CellData cell_data[2];
buffer grid {
    uint density;
    uint type;
};

Ray createCameraRay(in vec2 uv) {
    vec3 origin = (cameraProjection * vec4(0, 0, 0, 1)).xyz;
    vec3 direction = (inverseCameraProjection * vec4(uv, 0, 1)).xyz;
    direction = (cameraProjection * vec4(direction, 0)).xyz;
    direction = normalize(direction);

    Ray r;
    r.position = origin;
    r.direction = direction;
    return r;
}

void main()
{
    FragColor = vFragColor;
}

