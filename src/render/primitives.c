#include <stdlib.h>
#include <math.h>

#include "render/primitives.h"
#include "glad/glad.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_RENDERER
#include "logger.h"

#define PRIMITIVE_COUNT 5
#define PI 3.1415926535

// 2d
static Quad     PRIMITIVE_TYPE_QUAD;
static Circle   PRIMITIVE_TYPE_CIRCLE;

// 3d
static Plane    PRIMITIVE_TYPE_PLANE;
static Cube     PRIMITIVE_TYPE_CUBE;
static Sphere   PRIMITIVE_TYPE_SPHERE;

static Primitive* ALL_PRIMITIVES[PRIMITIVE_COUNT] = {
    &PRIMITIVE_TYPE_QUAD,
    &PRIMITIVE_TYPE_CIRCLE.primitive,
    &PRIMITIVE_TYPE_PLANE,
    &PRIMITIVE_TYPE_CUBE,
    &PRIMITIVE_TYPE_SPHERE.primitive,
};

Quad generate_quad(void) {
    log_debug("Generating quad");
    Quad quad;

    VertexPosition p0 = { 0.0, 0.0, 0.0 };
    VertexPosition p1 = { 1.0, 0.0, 0.0 };
    VertexPosition p2 = { 1.0, 1.0, 0.0 };
    VertexPosition p3 = { 0.0, 1.0, 0.0 };

    Vector2f uv0 = { 0.0, 0.0 };
    Vector2f uv1 = { 0.0, 1.0 };
    Vector2f uv2 = { 1.0, 1.0 };
    Vector2f uv3 = { 1.0, 0.0 };

    Vector3f nm = { 0.0, 0.0, -1.0 };

    VertexColor color = { 1.0, 1.0, 1.0, 1.0 };

    quad.indices = malloc(6 * sizeof(unsigned int));
    quad.vertices = malloc(4 * sizeof(Vertex));

    memcpy(quad.indices, (unsigned int[]) {
        0, 1, 2, 0, 2, 3
    }, 6 * sizeof(unsigned int));

    memcpy(quad.vertices, (Vertex[4]) {
        { p0, uv0, nm, color },
        { p1, uv1, nm, color },
        { p2, uv2, nm, color },
        { p3, uv3, nm, color },
    }, 4 * sizeof(Vertex));

    quad.index_count = 6;
    quad.vertex_count = 4;

    log_debug("Generated quad");
    return quad;
}

Circle generate_circle(unsigned int resolution) {
    log_debug("Generating circle");
    Circle circle;
    circle.resolution = resolution;

    Vector3f nm = { 0.0, 0.0, -1.0 };
    VertexColor color = { 1.0, 1.0, 1.0, 1.0 };

    unsigned int vertex_count = 1 + 4 * resolution;
    circle.primitive.vertices = malloc(vertex_count * sizeof(Vertex));
    circle.primitive.indices = malloc(3 * (vertex_count - 1) * sizeof(unsigned int));

    circle.primitive.vertices[0] = (Vertex){ { 0.0, 0.0, 0.0 }, { 0.75, 0.75 }, nm, color };

    const double INCREMENT = 2.0 * PI / (double)(4 * resolution);
    double angle = 0.0;
    for (int i = 1; i < vertex_count; i++) {
        Vector3f vPos = { cos(angle), sin(angle), 0.0 };
        Vector2f uv = { (vPos.x + 1.0) * 0.5, (vPos.y + 1.0) * 0.5 };
        circle.primitive.vertices[i] = (Vertex){ vPos, uv, nm, color };
        circle.primitive.indices[3 * (i - 1) + 0] = 0;
        circle.primitive.indices[3 * (i - 1) + 1] = i;
        circle.primitive.indices[3 * (i - 1) + 2] = (i + 1) % vertex_count;
        if (circle.primitive.indices[3 * (i - 1) + 2] == 0) {
            circle.primitive.indices[3 * (i - 1) + 2] = 1;
        }
        angle += INCREMENT;
    }

    circle.primitive.index_count = 3 * (vertex_count - 1);
    circle.primitive.vertex_count = vertex_count;

    log_debug("Generated circle");
    return circle;
}

Plane generate_plane(void) {
    log_debug("Generating plane");
    Plane plane;

    VertexPosition p0 = { 0.0, 0.0, 0.0 };
    VertexPosition p1 = { 1.0, 0.0, 0.0 };
    VertexPosition p2 = { 1.0, 1.0, 0.0 };
    VertexPosition p3 = { 0.0, 1.0, 0.0 };

    Vector2f uv0 = { 0.0, 0.0 };
    Vector2f uv1 = { 0.0, 1.0 };
    Vector2f uv2 = { 1.0, 1.0 };
    Vector2f uv3 = { 1.0, 0.0 };

    Vector3f nm = { 0.0, 1.0, 1.0 };

    VertexColor color = { 1.0, 1.0, 1.0, 1.0 };

    plane.indices = malloc(6 * sizeof(unsigned int));
    plane.vertices = malloc(4 * sizeof(Vertex));

    memcpy(plane.indices, (unsigned int[]) {
        0, 1, 2, 0, 2, 3
    }, 6 * sizeof(unsigned int));

    memcpy(plane.vertices, (Vertex[4]) {
        { p0, uv0, nm, color },
        { p1, uv1, nm, color },
        { p2, uv2, nm, color },
        { p3, uv3, nm, color },
    }, 4 * sizeof(Vertex));

    plane.index_count = 6;
    plane.vertex_count = 4;

    log_debug("Generated plane");
    return plane;
}

Cube generate_cube(void) {
    log_debug("Generating cube");
    Cube cube;

    VertexPosition p0 = {  0.5,  0.5,  0.5 };
    VertexPosition p1 = { -0.5,  0.5,  0.5 };
    VertexPosition p2 = { -0.5,  0.5, -0.5 };
    VertexPosition p3 = {  0.5,  0.5, -0.5 };

    VertexPosition p4 = {  0.5, -0.5,  0.5 };
    VertexPosition p5 = { -0.5, -0.5,  0.5 };
    VertexPosition p6 = { -0.5, -0.5, -0.5 };
    VertexPosition p7 = {  0.5, -0.5, -0.5 };

    Vector2f uv0 = { 0.0, 0.0 };
    Vector2f uv1 = { 0.0, 1.0 };
    Vector2f uv2 = { 1.0, 1.0 };
    Vector2f uv3 = { 1.0, 0.0 };

    VertexColor color = { 1.0, 1.0, 1.0, 1.0 };

    cube.indices = malloc(36 * sizeof(unsigned int));
    cube.vertices = malloc(24 * sizeof(Vertex));

    cube.index_count = 36;
    cube.vertex_count = 24;

    memcpy(cube.indices, (unsigned int[]) {
        0  + 2,  0 + 1,  0 + 0,  0 + 0,  0 + 3,  0 + 2,
        4  + 2,  4 + 3,  4 + 0,  4 + 0,  4 + 1,  4 + 2,
        8  + 2,  8 + 3,  8 + 0,  8 + 0,  8 + 1,  8 + 2,
        12 + 2, 12 + 1, 12 + 0, 12 + 0, 12 + 3, 12 + 2,
        16 + 2, 16 + 3, 16 + 0, 16 + 0, 16 + 1, 16 + 2,
        20 + 2, 20 + 1, 20 + 0, 20 + 0, 20 + 3, 20 + 2,
    }, 36 * sizeof(unsigned int));

    memcpy(cube.vertices, (Vertex[]) {
        // Bottom face
        { p0, uv0, {  0.f, -1.f,  0.f }, /*{  0.f,  0.f,  1.f },*/ color },
        { p1, uv1, {  0.f, -1.f,  0.f }, /*{  0.f,  0.f,  1.f },*/ color },
        { p2, uv2, {  0.f, -1.f,  0.f }, /*{  0.f,  0.f,  1.f },*/ color },
        { p3, uv3, {  0.f, -1.f,  0.f }, /*{  0.f,  0.f,  1.f },*/ color },

        // Top face
        { p4, uv0, {  0.f,  1.f,  0.f }, /*{  0.f,  0.f,  1.f },*/ color },
        { p5, uv1, {  0.f,  1.f,  0.f }, /*{  0.f,  0.f,  1.f },*/ color },
        { p6, uv2, {  0.f,  1.f,  0.f }, /*{  0.f,  0.f,  1.f },*/ color },
        { p7, uv3, {  0.f,  1.f,  0.f }, /*{  0.f,  0.f,  1.f },*/ color },

        // Back face
        { p0, uv0, {  0.f,  0.f, -1.f }, /*{  0.f,  1.f,  0.f },*/ color },
        { p1, uv1, {  0.f,  0.f, -1.f }, /*{  0.f,  1.f,  0.f },*/ color },
        { p5, uv2, {  0.f,  0.f, -1.f }, /*{  0.f,  1.f,  0.f },*/ color },
        { p4, uv3, {  0.f,  0.f, -1.f }, /*{  0.f,  1.f,  0.f },*/ color },

        // Front face
        { p3, uv0, {  0.f,  0.f,  1.f }, /*{  0.f, -1.f,  0.f },*/ color },
        { p2, uv1, {  0.f,  0.f,  1.f }, /*{  0.f, -1.f,  0.f },*/ color },
        { p6, uv2, {  0.f,  0.f,  1.f }, /*{  0.f, -1.f,  0.f },*/ color },
        { p7, uv3, {  0.f,  0.f,  1.f }, /*{  0.f, -1.f,  0.f },*/ color },

        // Left face
        { p4, uv0, { -1.f,  0.f,  0.f }, /*{  0.f,  0.f,  1.f },*/ color },
        { p7, uv1, { -1.f,  0.f,  0.f }, /*{  0.f,  0.f,  1.f },*/ color },
        { p3, uv2, { -1.f,  0.f,  0.f }, /*{  0.f,  0.f,  1.f },*/ color },
        { p0, uv3, { -1.f,  0.f,  0.f }, /*{  0.f,  0.f,  1.f },*/ color },

        // Right face
        { p5, uv0, {  1.f,  0.f,  0.f }, /*{  0.f,  0.f,  1.f },*/ color },
        { p6, uv1, {  1.f,  0.f,  0.f }, /*{  0.f,  0.f,  1.f },*/ color },
        { p2, uv2, {  1.f,  0.f,  0.f }, /*{  0.f,  0.f,  1.f },*/ color },
        { p1, uv3, {  1.f,  0.f,  0.f }, /*{  0.f,  0.f,  1.f },*/ color },
    }, 24 * sizeof(Vertex));

    log_debug("Generated cube");
    return cube;
}

void create_sphere_triangle(int *index, Primitive *p, unsigned int i0, unsigned int i1, unsigned int i2) {
    p->indices[(*index)++] = i0;
    p->indices[(*index)++] = i1;
    p->indices[(*index)++] = i2;

    /*
    // generate vertex tangents
    Vertex v1 = p->vertices[i0];
    Vertex v2 = p->vertices[i1];
    Vertex v3 = p->vertices[i2];

    glm::vec3 edge1 = v2.position - v1.position;
    glm::vec3 edge2 = v3.position - v1.position;
    glm::vec2 dUV1 = v2.textureCoordinate - v1.textureCoordinate;
    glm::vec2 dUV2 = v3.textureCoordinate - v1.textureCoordinate;

    float f = 1.f / (dUV1.x * dUV2.y - dUV1.y * dUV2.x);

    v1.tangent.x = f * (dUV2.y * edge1.x - dUV1.y * edge2.x);
    v1.tangent.y = f * (dUV2.y * edge1.y - dUV1.y * edge2.y);
    v1.tangent.z = f * (dUV2.y * edge1.z - dUV1.y * edge2.z);
    */
}

Sphere generate_sphere(unsigned int resolution) {
    log_debug("Generating sphere");
    // A procedural sphere shape with alright control over vertex count
    // The sphere is created by first creating an octahedron through triangles subdivided
    // along the edges as vertices
    // The vertices are then transformed to be distance 1 away from the centre
    Sphere sphere;

    const int total_vertices_for_last_row = resolution * 4;
    const int total_vertex_count = (2 * resolution) * (resolution + 1) + 1;

    sphere.primitive.indices = malloc(2 * 3 * total_vertex_count * sizeof(unsigned int));
    sphere.primitive.vertices = malloc(2 * total_vertex_count * sizeof(Vertex));

    VertexColor color = { 1.0, 1.0, 1.0, 1.0 };
    // Generate top half vertices
    {
        const float position_offset = 1.f / (float)resolution;

        Vertex v;
        v.position = (Vector3f){ 0.f, -1.f, 0.f };
        v.normal = (Vector3f){ 0.f, -1.f, 0.f };
        v.uv_coordinate.x = asin(v.normal.x) / PI + 0.5f;
        v.uv_coordinate.y = asin(v.normal.y) / PI + 0.5f;
        v.color = color;
        sphere.primitive.vertices[0] = v;

        // the direction which we offset for each vertex
        const int windDirections[] = {
            0, 1,
            1, 0,
            0, -1,
            -1, 0
        };

        for (int row = 1; row <= resolution; row++) {
            v.position.y += position_offset;

            v.position.z -= position_offset;
            v.position.x -= position_offset;

            int vertex_counter = 0;
            int index = 2;
            int x_direction = 0;
            int z_direction = 1;
            for (int j = 0; j < row * 4; j++) {
                v.normal = normalise_vector3f(v.position);
                v.color = color;

                v.uv_coordinate.x = asin(v.normal.x) / PI + 0.5f;
                v.uv_coordinate.y = asin(v.normal.y) / PI + 0.5f;

                sphere.primitive.vertices[row] = v;
                v.position.x += 2.f * position_offset * x_direction;
                v.position.z += 2.f * position_offset * z_direction;

                if (++vertex_counter == row) {
                    x_direction = windDirections[index + 0];
                    z_direction = windDirections[index + 1];
                    index += 2;

                    vertex_counter = 0;
                }
            }
        }
    }

    // Generate top half indices
    {
        int index_counter = 0;
        int current_vertex_up = 1;
        int current_vertex_down = 1;
        for (int i = 1; i <= resolution; i++) {
            // v(i) = 2r(i + 1) + 1
            const int row_total_vertex_count = (2 * i) * (i + 1) + 1;
            // = v(i) - v(i - 1)
            const int total_vertices_to_process = (4 * i);
            // = v(i - 1)
            const int wrap_vertex = (2 * i) * (i - 1) + 1;
            // = v(i - 2)
            const int previous_vertex = (i > 1) ? ((2 * i) * (i - 3) + 5) : 0;

            // add first index to list since it is always the non pattern conforming
            create_sphere_triangle(&index_counter, &sphere.primitive, wrap_vertex, row_total_vertex_count - 1, previous_vertex);

            // up triangles
            int vertex_counter = 0;
            int increment = 0;
            for (int j = 0; j < total_vertices_to_process - 1; j++) {
                create_sphere_triangle(&index_counter, &sphere.primitive, current_vertex_up + 1, current_vertex_up, previous_vertex + increment);
                current_vertex_up++;

                // if we are on a corner we have 2 triangles under us so generate them
                if (i != 1) {
                    if (++vertex_counter >= i) {
                        create_sphere_triangle(&index_counter, &sphere.primitive, current_vertex_up + 1, current_vertex_up, previous_vertex + increment);

                        // We are adding another vertex so we want to increment all values pertaining to the +1 vertex
                        // vertex_counter = 1 since this is a processed vertex
                        j++;
                        current_vertex_up++;
                        vertex_counter = 1;
                    }
                    increment++;
                }
            }
            current_vertex_up++;

            // down triangles
            if (i < 2) { continue; }
            create_sphere_triangle(&index_counter, &sphere.primitive, row_total_vertex_count - 1, wrap_vertex - 1, previous_vertex);

            vertex_counter = 1;
            for (int j = 1; j < total_vertices_to_process - 1; j++) {
                // if we are not on a corner we have down-triangles associated
                if ((vertex_counter % i) != 0) {
                    create_sphere_triangle(&index_counter, &sphere.primitive, wrap_vertex + vertex_counter, current_vertex_down, current_vertex_down + 1);

                    // We are adding another vertex so we want to increment all values pertaining to the +1 vertex
                    // vertex_counter = 1 since this is a processed vertex
                    current_vertex_down++;
                }
                vertex_counter++;
            }
            current_vertex_down++;
        }
    }

    // Mirror vertices and indices
    {
        for (int i = 0; i < total_vertex_count - total_vertices_for_last_row; i++ ) {
            Vertex v = sphere.primitive.vertices[i];
            v.position.y *= -1.0;
            v.normal.y *= -1.0;
            v.uv_coordinate.y = asin(v.normal.y) / PI + 0.5;
            sphere.primitive.vertices[total_vertex_count + i] = v;
        }

        unsigned int half_index_count = 3 * total_vertex_count;
        for (unsigned int i = 0; i < half_index_count; i += 3) {
            sphere.primitive.indices[i + half_index_count] = sphere.primitive.indices[i + 2];
            sphere.primitive.indices[i + 2 + half_index_count] = sphere.primitive.indices[i];
            for (unsigned int j = 0; j < 3; j++) {
                unsigned int index = sphere.primitive.indices[half_index_count + i + j];
                if (total_vertex_count - index > total_vertices_for_last_row) {
                    sphere.primitive.indices[half_index_count + i + j] += total_vertex_count;
                }
            }
        }
    }

    // Ensure all points are at unit distance 1 from origin
    {
        for (int i = 0; i < 2 * total_vertex_count; i++) {
            Vertex v = sphere.primitive.vertices[i];
            float modifier = sqrt(1.f / (v.position.x * v.position.x + v.position.y * v.position.y + v.position.z * v.position.z));
            sphere.primitive.vertices[i].position.x *= modifier;
            sphere.primitive.vertices[i].position.y *= modifier;
            sphere.primitive.vertices[i].position.z *= modifier;
        }
    }

    sphere.primitive.vertex_count = 2 * total_vertex_count;
    sphere.primitive.index_count = 2 * 3 * total_vertex_count;
    sphere.resolution = resolution;

    log_debug("Generated sphere");
    return sphere;
}

void generate_primitives(void) {
    log_info("Generating primititves...");
    PRIMITIVE_TYPE_QUAD =       generate_quad();
    PRIMITIVE_TYPE_CIRCLE =     generate_circle(50);
    PRIMITIVE_TYPE_PLANE =      generate_plane();
    PRIMITIVE_TYPE_CUBE =       generate_cube();
    PRIMITIVE_TYPE_SPHERE =     generate_sphere(10);

    log_info("Binding primititve data");
    for (int i = 0; i < PRIMITIVE_COUNT; i++) {
        Primitive *primitive = ALL_PRIMITIVES[i];
        glGenBuffers(1, &primitive->_ebo);
        glGenBuffers(1, &primitive->_vbo);
    }
}

void destroy_primitives(void) {
    log_info("Destroying all primitives");
    for (int i = 0; i < PRIMITIVE_COUNT; i++) {
        free(ALL_PRIMITIVES[i]->vertices);
        free(ALL_PRIMITIVES[i]->indices);
    }
}

void bind_primitive_to_vao(Primitive *primitive, unsigned int vao) {
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, primitive->_vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        primitive->vertex_count * sizeof(Vertex),
        primitive->vertices,
        GL_STATIC_DRAW
    );
    bind_vertex_attributes();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitive->_ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        primitive->index_count * sizeof(unsigned int),
        primitive->indices,
        GL_STATIC_DRAW
    );

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_primitive(Primitive *primitive, unsigned int vao) {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, primitive->index_count, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Quad* primitive_quad(void) {
    return &PRIMITIVE_TYPE_QUAD;
}

Circle* primitive_circle(void) {
    return &PRIMITIVE_TYPE_CIRCLE;
}

Plane* primitive_plane(void) {
    return &PRIMITIVE_TYPE_PLANE;
}

Cube* primitive_cube(void) {
    return &PRIMITIVE_TYPE_CUBE;
}

Sphere* primitive_sphere(void) {
    return &PRIMITIVE_TYPE_SPHERE;
}

