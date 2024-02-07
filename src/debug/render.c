#include <stdlib.h>

#include "debug/render.h"
#include "render/camera.h"

DebugRender new_debug_renderer(void) {
    Shader circle_vs = load_vertex_shader_from_disk("shaders/debug_draw_circle.vert").data.ok;
    Shader circle_fs = load_fragment_shader_from_disk("shaders/debug_draw_circle.frag").data.ok;
    Shader rect_vs = load_vertex_shader_from_disk("shaders/debug_draw_rect.vert").data.ok;
    Shader rect_fs = load_fragment_shader_from_disk("shaders/debug_draw_rect.frag").data.ok;
    return (DebugRender) {
        .debug_circles = VECTOR(DebugShapeCircle),
        .debug_rectangles = VECTOR(DebugShapeRectangle),
        .debug_rays = VECTOR(DebugShapeRay),
        .debug_lines = VECTOR(DebugShapeLine),
        .circle_program = create_shader_program(circle_vs, circle_fs).data.ok,
        .rect_program = create_shader_program(rect_vs, rect_fs).data.ok
    };
}

void draw_circles(DebugRender* renderer, Camera camera, Matrix4f projection) {
    unsigned int shader = renderer->circle_program._program;
    int projectionPosition = glGetUniformLocation(shader, "projection");
    int viewPosition = glGetUniformLocation(shader, "view");
    int radiiPosition = glGetUniformLocation(shader, "radii");
    int positionPosition = glGetUniformLocation(shader, "positions");
    int thicknessPosition = glGetUniformLocation(shader, "thicknesses");
    int colourPosition = glGetUniformLocation(shader, "colours");

    glUseProgram(shader);
    glUniformMatrix4fv(viewPosition, 1, false, camera_view(&camera).entries);
    glUniformMatrix4fv(projectionPosition, 1, false, projection.entries);

    const int MAX_RESOURCES_IN_PASS = 128;
    float radii[MAX_RESOURCES_IN_PASS];
    Vector2f positions[MAX_RESOURCES_IN_PASS];
    float thicknesses[MAX_RESOURCES_IN_PASS];
    float colours[MAX_RESOURCES_IN_PASS * 3];

    for (size_t i = 0; i < renderer->debug_circles.length; i += MAX_RESOURCES_IN_PASS) {
        int count = MAX_RESOURCES_IN_PASS;
        if (i + count >= renderer->debug_circles.length) {
            count = renderer->debug_circles.length - i;
        }
        for (int j = 0; j < count; j++) {
            DebugShapeCircle circle;
            VECTOR_GET(DebugShapeCircle, &renderer->debug_circles, (int)(i + j), &circle);
            radii[j] = circle.radius;
            positions[j] = circle.position;
            thicknesses[j] = circle.thickness;

            FColorRGB colour = color_rgb_to_float(circle.colour);
            colours[3 * j + 0] = colour.r;
            colours[3 * j + 1] = colour.g;
            colours[3 * j + 2] = colour.b;
        }
        glUniform1fv(radiiPosition, count, radii);
        glUniform2fv(positionPosition, count, positions[0].entries);
        glUniform3fv(colourPosition, count, colours);
        glUniform1fv(thicknessPosition, count, thicknesses);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 3, count);
    }
}

void draw_rectangles(DebugRender* renderer, Camera camera, Matrix4f projection) {
    unsigned int shader = renderer->rect_program._program;
    int projectionPosition = glGetUniformLocation(shader, "projection");
    int viewPosition = glGetUniformLocation(shader, "view");
    int dimensionPosition = glGetUniformLocation(shader, "dimensions");
    int positionPosition = glGetUniformLocation(shader, "positions");
    int thicknessPosition = glGetUniformLocation(shader, "thicknesses");
    int colourPosition = glGetUniformLocation(shader, "colours");

    glUseProgram(shader);
    glUniformMatrix4fv(viewPosition, 1, false, camera_view(&camera).entries);
    glUniformMatrix4fv(projectionPosition, 1, false, projection.entries);

    const int MAX_RESOURCES_IN_PASS = 128;
    Vector2f dimensions[MAX_RESOURCES_IN_PASS];
    Vector2f positions[MAX_RESOURCES_IN_PASS];
    float thicknesses[MAX_RESOURCES_IN_PASS];
    float colours[MAX_RESOURCES_IN_PASS * 3];

    for (size_t i = 0; i < renderer->debug_rectangles.length; i += MAX_RESOURCES_IN_PASS) {
        int count = MAX_RESOURCES_IN_PASS;
        if (i + count >= renderer->debug_circles.length) {
            count = renderer->debug_circles.length - i;
        }
        for (int j = 0; j < count; j++) {
            DebugShapeRectangle rect;
            VECTOR_GET(DebugShapeRectangle, &renderer->debug_rectangles, (int)(i + j), &rect);
            dimensions[j] = rect.dimensions;
            positions[j] = rect.position;
            thicknesses[j] = rect.thickness;

            FColorRGB colour = color_rgb_to_float(rect.colour);
            colours[3 * j + 0] = colour.r;
            colours[3 * j + 1] = colour.g;
            colours[3 * j + 2] = colour.b;
        }
        glUniform2fv(dimensionPosition, count, dimensions[0].entries);
        glUniform2fv(positionPosition, count, positions[0].entries);
        glUniform3fv(colourPosition, count, colours);
        glUniform1fv(thicknessPosition, count, thicknesses);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 3, count);
    }
}

void draw_debug(DebugRender* renderer, Camera camera, Matrix4f projection) {
    draw_circles(renderer, camera, projection);
    draw_rectangles(renderer, camera, projection);

    renderer->debug_circles.length = 0;
    renderer->debug_rectangles.length = 0;
    renderer->debug_rays.length = 0;
    renderer->debug_lines.length = 0;
}

void debug_circle(DebugRender* renderer, DebugShapeCircle circle) {
    VECTOR_PUSH(DebugShapeCircle, &renderer->debug_circles, circle);
}

void debug_rectangle(DebugRender* renderer, DebugShapeRectangle rectangle) {
    VECTOR_PUSH(DebugShapeRectangle, &renderer->debug_rectangles, rectangle);
}

void debug_ray(DebugRender* renderer, DebugShapeRay ray) {
    VECTOR_PUSH(DebugShapeRay, &renderer->debug_rays, ray);
}

void debug_line(DebugRender* renderer, DebugShapeLine line) {
    VECTOR_PUSH(DebugShapeLine, &renderer->debug_lines, line);
}
