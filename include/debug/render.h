#ifndef VESTIGE_DEBUG_RENDER_H
#define VESTIGE_DEBUG_RENDER_H

#include "lib/math.h"
#include "render/color.h"
#include "render/shader.h"
#include "lib/vector.h"

typedef struct DebugShapeCircle {
    Vector2f position;
    ColorRGB colour;
    float radius;
    float thickness;
} DebugShapeCircle;

typedef struct DebugShapeRectangle {
    Vector2f position;
    ColorRGB colour;
    Vector2f dimensions;
    float thickness;
} DebugShapeRectangle;

typedef struct DebugShapeRay {
    Vector2f position;
    ColorRGB colour;
    Vector2f direction;
    float thickness;
} DebugShapeRay;

typedef struct DebugShapeLine {
    Vector2f position;
    ColorRGB colour;
    Vector2f direction;
    float distance;
    float thickness;
} DebugShapeLine;

typedef struct DebugRender {
    Vector debug_circles;
    Vector debug_rectangles;
    Vector debug_rays;
    Vector debug_lines;
    ShaderProgram circle_program;
    ShaderProgram rect_program;
    ShaderProgram ray_program;
} DebugRender;

DebugRender new_debug_renderer(void);

struct Camera;
void draw_debug(DebugRender* renderer, struct Camera camera, Matrix4f projection);

void debug_circle(DebugRender* renderer, DebugShapeCircle circle);
void debug_rectangle(DebugRender* renderer, DebugShapeRectangle rectangle);
void debug_ray(DebugRender* renderer, DebugShapeRay ray);
void debug_line(DebugRender* renderer, DebugShapeLine line);
void debug_line_from_to(DebugRender* renderer, Vector2f p0, Vector2f p1, float thickness, ColorRGB colour);

#endif
