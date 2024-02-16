#include <stdlib.h>
#include <math.h>

#include "debug/render.h"
#include "render/camera.h"

DebugRender new_debug_renderer(void) {
    Shader circle_vs = load_vertex_shader_from_disk("shaders/debug_draw_circle.vert").data.ok;
    Shader circle_fs = load_fragment_shader_from_disk("shaders/debug_draw_circle.frag").data.ok;
    Shader rect_vs = load_vertex_shader_from_disk("shaders/debug_draw_rect.vert").data.ok;
    Shader rect_fs = load_fragment_shader_from_disk("shaders/debug_draw_rect.frag").data.ok;
    Shader ray_vs = load_vertex_shader_from_disk("shaders/debug_draw_ray.vert").data.ok;
    Shader ray_fs = load_fragment_shader_from_disk("shaders/debug_draw_ray.frag").data.ok;
    return (DebugRender) {
        .debug_circles = VECTOR(DebugShapeCircle),
        .debug_rectangles = VECTOR(DebugShapeRectangle),
        .debug_rays = VECTOR(DebugShapeRay),
        .debug_lines = VECTOR(DebugShapeLine),
        .circle_program = create_shader_program(circle_vs, circle_fs).data.ok,
        .rect_program = create_shader_program(rect_vs, rect_fs).data.ok,
        .ray_program = create_shader_program(ray_vs, ray_fs).data.ok
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
        if (i + count >= renderer->debug_rectangles.length) {
            count = renderer->debug_rectangles.length - i;
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

void draw_rays(DebugRender* renderer, Camera camera, Matrix4f projection) {
    unsigned int shader = renderer->ray_program._program;
    int projectionPosition = glGetUniformLocation(shader, "projection");
    int viewPosition = glGetUniformLocation(shader, "view");
    int positionPosition = glGetUniformLocation(shader, "positions");
    int directionPosition = glGetUniformLocation(shader, "directions");
    int lengthPosition = glGetUniformLocation(shader, "lengths");
    int thicknessPosition = glGetUniformLocation(shader, "thicknesses");
    int colourPosition = glGetUniformLocation(shader, "colours");
    int screenToWorldPosition = glGetUniformLocation(shader, "screenToWorld");

    glUseProgram(shader);
    glUniformMatrix4fv(viewPosition, 1, false, camera_view(&camera).entries);
    glUniformMatrix4fv(projectionPosition, 1, false, projection.entries);
    Matrix4f screen_to_world = inverse_mat4(mul_mat4(camera_view(&camera), projection));
    glUniformMatrix4fv(screenToWorldPosition, 1, false, screen_to_world.entries);

    const int MAX_RESOURCES_IN_PASS = 128;
    Vector2f directions[MAX_RESOURCES_IN_PASS];
    Vector2f positions[MAX_RESOURCES_IN_PASS];
    float thicknesses[MAX_RESOURCES_IN_PASS];
    float colours[MAX_RESOURCES_IN_PASS * 3];
    float lengths[MAX_RESOURCES_IN_PASS];
    for (int i = 0; i < MAX_RESOURCES_IN_PASS; i++) {
        lengths[i] = INFINITY;
    }

    for (size_t i = 0; i < renderer->debug_rays.length; i += MAX_RESOURCES_IN_PASS) {
        int count = MAX_RESOURCES_IN_PASS;
        if (i + count >= renderer->debug_rays.length) {
            count = renderer->debug_rays.length - i;
        }
        for (int j = 0; j < count; j++) {
            DebugShapeRay ray;
            VECTOR_GET(DebugShapeRay, &renderer->debug_rays, (int)(i + j), &ray);
            directions[j] = ray.direction;
            positions[j] = ray.position;
            thicknesses[j] = ray.thickness;

            FColorRGB colour = color_rgb_to_float(ray.colour);
            colours[3 * j + 0] = colour.r;
            colours[3 * j + 1] = colour.g;
            colours[3 * j + 2] = colour.b;
        }
        glUniform2fv(directionPosition, count, directions[0].entries);
        glUniform2fv(positionPosition, count, positions[0].entries);
        glUniform3fv(colourPosition, count, colours);
        glUniform1fv(thicknessPosition, count, thicknesses);
        glUniform1fv(lengthPosition, count, lengths);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 3, count);
    }
}

void draw_lines(DebugRender* renderer, Camera camera, Matrix4f projection) {
    unsigned int shader = renderer->ray_program._program;
    int projectionPosition = glGetUniformLocation(shader, "projection");
    int viewPosition = glGetUniformLocation(shader, "view");
    int positionPosition = glGetUniformLocation(shader, "positions");
    int directionPosition = glGetUniformLocation(shader, "directions");
    int lengthPosition = glGetUniformLocation(shader, "lengths");
    int thicknessPosition = glGetUniformLocation(shader, "thicknesses");
    int colourPosition = glGetUniformLocation(shader, "colours");
    int screenToWorldPosition = glGetUniformLocation(shader, "screenToWorld");

    glUseProgram(shader);
    glUniformMatrix4fv(viewPosition, 1, false, camera_view(&camera).entries);
    glUniformMatrix4fv(projectionPosition, 1, false, projection.entries);
    Matrix4f screen_to_world = inverse_mat4(mul_mat4(camera_view(&camera), projection));
    glUniformMatrix4fv(screenToWorldPosition, 1, false, screen_to_world.entries);

    const int MAX_RESOURCES_IN_PASS = 128;
    Vector2f directions[MAX_RESOURCES_IN_PASS];
    Vector2f positions[MAX_RESOURCES_IN_PASS];
    float thicknesses[MAX_RESOURCES_IN_PASS];
    float colours[MAX_RESOURCES_IN_PASS * 3];
    float lengths[MAX_RESOURCES_IN_PASS];

    for (size_t i = 0; i < renderer->debug_lines.length; i += MAX_RESOURCES_IN_PASS) {
        int count = MAX_RESOURCES_IN_PASS;
        if (i + count >= renderer->debug_lines.length) {
            count = renderer->debug_lines.length - i;
        }
        for (int j = 0; j < count; j++) {
            DebugShapeLine line;
            VECTOR_GET(DebugShapeLine, &renderer->debug_lines, (int)(i + j), &line);
            directions[j] = line.direction;
            positions[j] = line.position;
            thicknesses[j] = line.thickness;
            lengths[j] = line.distance;

            FColorRGB colour = color_rgb_to_float(line.colour);
            colours[3 * j + 0] = colour.r;
            colours[3 * j + 1] = colour.g;
            colours[3 * j + 2] = colour.b;
        }
        glUniform2fv(directionPosition, count, directions[0].entries);
        glUniform2fv(positionPosition, count, positions[0].entries);
        glUniform3fv(colourPosition, count, colours);
        glUniform1fv(thicknessPosition, count, thicknesses);
        glUniform1fv(lengthPosition, count, lengths);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 3, count);
    }
}

void draw_debug(DebugRender* renderer, Camera camera, Matrix4f projection) {
    draw_circles(renderer, camera, projection);
    draw_rectangles(renderer, camera, projection);
    draw_rays(renderer, camera, projection);
    draw_lines(renderer, camera, projection);

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

void debug_line_from_to(DebugRender* renderer, Vector2f p0, Vector2f p1, float thickness, ColorRGB colour) {
    Vector2f direction = sub_vector2f(p1, p0);
    float distance = length_vector2f(direction);
    debug_line(renderer, (DebugShapeLine) {
        .position = p0,
        .direction = normalise_vector2f(direction),
        .thickness = thickness,
        .colour = colour,
        .distance = distance
    });
}
