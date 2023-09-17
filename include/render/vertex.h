#ifndef VESTIGE_VERTEX_H
#define VESTIGE_VERTEX_H

#include "lib/math.h"
#include "render/color.h"

typedef Vector3f VertexPosition;
typedef FColorRGBA VertexColor;

typedef struct Vertex {
    VertexPosition position;
    VertexColor color;
} Vertex;

void bind_vertex_attributes(void);

#endif
