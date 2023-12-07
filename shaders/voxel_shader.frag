#version 430

out vec4 FragColor;

struct Cell {
    uint density;
    uint type;
};

struct CellData {
    vec3 color;
};

struct Ray {
    vec3 position;
    vec3 direction;
};

uniform ivec2 screenSize;
uniform mat4 cameraProjection;
//uniform CellData cell_data[2];
uniform sampler3D grid;

Ray createCameraRay(vec2 uv) {
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

Cell getCell(ivec3 position) {
    uvec3 data = uvec3(texelFetch(grid, position, 0).rgb * 255.0);
    Cell c;
    c.density = (data.x << 8) | data.y;
    c.type = data.z;
    return c;
}

/* Sphube: https://arxiv.org/pdf/1604.02174.pdf */
void main() {
    vec2 screenPos = gl_FragCoord.xy / screenSize;
    Ray ray = createCameraRay(screenPos);

    ivec3 mapPos = ivec3(floor(ray.position + 0.));
    vec3 deltaDist = abs(vec3(length(ray.direction)) / ray.direction);
    ivec3 rayStep = ivec3(sign(ray.direction));
    vec3 sideDist = (sign(ray.direction) * (vec3(mapPos) - ray.position) + (sign(ray.direction) * 0.5) + 0.5) * deltaDist;
    bvec3 mask;

    vec3 relativePosition;
    for (int i = 0; i < 32; i++) {
        if (getCell(mapPos).type != 0) {
            vec3 color;
            if (mask.x) {
                color = vec3(0.5);
            }
            if (mask.y) {
                color = vec3(1.0);
            }
            if (mask.z) {
                color = vec3(0.75);
            }
            FragColor = vec4(color, 1);
            break;
        }
        mask = lessThanEqual(sideDist.xyz, min(sideDist.yzx, sideDist.zxy));
        sideDist += vec3(mask) * deltaDist;
        mapPos += ivec3(vec3(mask)) * rayStep;
    }
    FragColor = vec4(abs(mapPos / 256.f), 1);
}

