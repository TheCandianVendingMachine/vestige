#include <math.h>
#include <stdlib.h>

#include "logger.h"
#include "glad/glad.h"
#include "render/vertex.h"
#include "ui/glyph.h"
#include "ui/text.h"

Text create_text(Font font) {
    Text text;
    text.font = font;
    text._updated = true;
    text.string = string_from_cstr("");
    glGenBuffers(1, &text._render_info.vao_handle);
    glGenBuffers(1, &text._render_info.vbo_handle);
    text._render_info.point = 0;
    return text;
}

void text_set_string(Text* text, const char* str) {
    del_string(text->string);
    text->string = string_from_cstr((char*)str);
    text->_updated = true;
}

void draw_text(Text* text) {
    glBindVertexArray(text->_render_info.vao_handle);
    if (text->_updated) {
        log_debug("6");
        text->_render_info.vertex_count = 6 * text->string.length;
        Vertex* vertices = malloc(sizeof(Vertex) * text->_render_info.vertex_count);
        memset(vertices, 0, sizeof(Vertex) * text->_render_info.vertex_count);

        Vector2f max_size = (Vector2f) { .x = 0.f, .y = 0.f };
        Vector2f cursor_position = (Vector2f) { .x = 0.f, .y = 0.f };
        Point point = text->font.points[text->_render_info.point];
        log_debug("5");
        for (int i = 0; i < text->string.length; i++) {
            log_debug("a, %d", i);
            char c = text->string.buffer[i];
            log_debug("char: %c", c);
            size_t glyph_index = *(size_t*)hashmap_get(&text->font.char_glyph_map, &c);
            log_debug("glyph index %d", glyph_index);
            Glyph glyph = _VECTOR_GET(Glyph, &point.glyphs, glyph_index);
            log_debug("glyph: %f", glyph.horizontal_metrics.advance);

            log_debug("b");
            const Vector2f origin = add_vector2f(cursor_position, glyph.horizontal_metrics.bearing);
            const Vector3f tl = (Vector3f) {
                .x = origin.x,
                .y = origin.y,
                .z = 0.f
            };
            const Vector3f tr = (Vector3f) {
                .x = origin.x + glyph.bounds.size.x,
                .y = origin.y,
                .z = 0.f
            };
            const Vector3f br = (Vector3f) {
                .x = origin.x + glyph.bounds.size.x,
                .y = origin.y + glyph.bounds.size.y,
                .z = 0.f
            };
            const Vector3f bl = (Vector3f) {
                .x = origin.x,
                .y = origin.y + glyph.bounds.size.y,
                .z = 0.f
            };

            log_debug("c");
            vertices[6 * i + 0].position = tl;
            vertices[6 * i + 1].position = tr;
            vertices[6 * i + 2].position = br;
            vertices[6 * i + 3].position = tl;
            vertices[6 * i + 4].position = br;
            vertices[6 * i + 5].position = bl;
            log_debug("d");

            vertices[6 * i + 0].uv_coordinate = (Vector2f) {
                .x = glyph.uv_coordinates.x,
                .y = glyph.uv_coordinates.y
            };
            vertices[6 * i + 1].uv_coordinate = (Vector2f) {
                .x = glyph.uv_coordinates.x + glyph.uv_size.x,
                .y = glyph.uv_coordinates.y
            };
            vertices[6 * i + 2].uv_coordinate = (Vector2f) {
                .x = glyph.uv_coordinates.x + glyph.uv_size.x,
                .y = glyph.uv_coordinates.y + glyph.uv_size.y
            };
            vertices[6 * i + 3].uv_coordinate = (Vector2f) {
                .x = glyph.uv_coordinates.x,
                .y = glyph.uv_coordinates.y
            };
            vertices[6 * i + 4].uv_coordinate = (Vector2f) {
                .x = glyph.uv_coordinates.x + glyph.uv_size.x,
                .y = glyph.uv_coordinates.y + glyph.uv_size.y
            };
            vertices[6 * i + 5].uv_coordinate = (Vector2f) {
                .x = glyph.uv_coordinates.x,
                .y = glyph.uv_coordinates.y + glyph.uv_size.y
            };
            log_debug("e");

            vertices[6 * i + 0].color = (VertexColor) {
                .r = 1.f, .g = 1.f, .b = 1.f, .a = 1.f
            };
            vertices[6 * i + 1].color = (VertexColor) {
                .r = 1.f, .g = 1.f, .b = 1.f, .a = 1.f
            };
            vertices[6 * i + 2].color = (VertexColor) {
                .r = 1.f, .g = 1.f, .b = 1.f, .a = 1.f
            };
            vertices[6 * i + 3].color = (VertexColor) {
                .r = 1.f, .g = 1.f, .b = 1.f, .a = 1.f
            };
            vertices[6 * i + 4].color = (VertexColor) {
                .r = 1.f, .g = 1.f, .b = 1.f, .a = 1.f
            };
            vertices[6 * i + 5].color = (VertexColor) {
                .r = 1.f, .g = 1.f, .b = 1.f, .a = 1.f
            };
            log_debug("f");

            if (c == '\n') {
                cursor_position.x = 0.f;
                cursor_position.y += point.newline_height;
            } else {
                cursor_position.x += glyph.horizontal_metrics.advance;
            }
            log_debug("g");

            max_size.x = max_size.x < br.x ? br.x : max_size.x;
            max_size.y = max_size.y < br.y ? br.y : max_size.y;
            log_debug("h");
        }
        log_debug("4");

        for (int i = 0; i < text->_render_info.vertex_count; i++) {
            vertices[i].position.x /= max_size.x;
            vertices[i].position.y /= max_size.y;
        }
        log_debug("3");

        glBindBuffer(GL_ARRAY_BUFFER, text->_render_info.vbo_handle);
        glBufferData(
            GL_ARRAY_BUFFER,
            text->_render_info.vertex_count * sizeof(Vertex),
            vertices,
            GL_STATIC_DRAW
        );
        bind_vertex_attributes();

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        text->_updated = false;
        free(vertices);
        glBindVertexArray(text->_render_info.vao_handle);
        log_debug("2");
    }
    glDrawArrays(GL_TRIANGLES, text->_render_info.vertex_count, GL_UNSIGNED_INT);
    glBindVertexArray(0);
    log_debug("1");
}

void destroy_text(Text text) {
    del_string(text.string);
    glDeleteBuffers(1, &text._render_info.vao_handle);
    glDeleteBuffers(1, &text._render_info.vbo_handle);
}
