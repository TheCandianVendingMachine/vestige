#ifndef VESTIGE_PRIMITIVES_H
#define VESTIGE_PRIMITIVES_H

#include "render/vertex.h"

typedef struct Primitive {
    unsigned int _vbo;
    unsigned int _ebo;

    unsigned int* indices;
    unsigned int index_count;

    Vertex* vertices;
    unsigned int vertex_count;
} Primitive;

typedef struct ResolutionPrimitive {
    Primitive _primitive;
    unsigned int resolution;
} ResolutionPrimitive;

// 2d Shapes
typedef Primitive Quad;
typedef ResolutionPrimitive Circle;

// 3d Shapes
typedef Primitive Plane;
typedef Primitive Cube;
typedef ResolutionPrimitive Sphere;

void generate_primitives(void);
void destroy_primitives(void);
void bind_primitive_to_vao(Primitive *primitive, unsigned int vao);

Quad* primitive_quad(void);
Plane* primitive_plane(void);
Cube* primitive_cube(void);
Sphere* primitive_sphere(void);

#endif
