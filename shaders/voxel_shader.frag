#version 430

out vec4 FragColor;

struct CellData {
    vec3 color;
};

struct Ray {
    vec3 position;
    vec3 direction;
};

uniform mat4 cameraProjection;
//uniform CellData cell_data[2];
buffer grid {
    uint density;
    uint type;
};

Ray createCameraRay(in vec2 uv) {
    mat4 inverseCameraProjection = inverse(cameraProjection);
    vec3 origin = (cameraProjection * vec4(0, 0, 0, 1)).xyz;
    vec3 direction = (inverseCameraProjection * vec4(uv, 0, 1)).xyz;
    direction = (cameraProjection * vec4(direction, 0)).xyz;
    direction = normalize(direction);

    Ray r;
    r.position = origin;
    r.direction = direction;
    return r;
}

/* Sphube: https://arxiv.org/pdf/1604.02174.pdf */
void main() {
    Ray ray = createCameraRay(gl_FragCoord.xy);

    FragColor = vec4(1 * ray.direction, 1);
}

