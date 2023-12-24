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
    glGenVertexArrays(1, &text._render_info.vao_handle);
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
    if (text->_updated) {
        size_t char_count = 0;
        for (int i = 0; i < text->string.length; i++) {
            char c = text->string.buffer[i];
            if (c != '\n' && c != ' ' && c != '\t') {
                char_count++;
            }
        }
        text->_render_info.vertex_count = 6 * char_count;

        Vertex* vertices = malloc(sizeof(Vertex) * text->_render_info.vertex_count);
        memset(vertices, 0, sizeof(Vertex) * text->_render_info.vertex_count);

        Vector2f cursor_position = (Vector2f) { .x = 0.f, .y = 0.f };
        Point point = text->font.points[text->_render_info.point];

        int vertex_index = 0;
        for (int i = 0; i < text->string.length; i++) {
            char c = text->string.buffer[i];
            if (c == '\n') {
                cursor_position.x = 0.f;
                cursor_position.y += point.newline_height;
                continue;
            }
            size_t glyph_index = *(size_t*)hashmap_get(&text->font.char_glyph_map, &c);
            Glyph glyph = _VECTOR_GET(Glyph, &point.glyphs, glyph_index);
            if (c == ' ' || c == '\t') {
                int modifier = 1;
                if (c == '\t') {
                    modifier = 4;
                }
                cursor_position.x += modifier * glyph.horizontal_metrics.advance;
                continue;
            }
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

            vertices[6 * vertex_index + 0].position = tl;
            vertices[6 * vertex_index + 1].position = tr;
            vertices[6 * vertex_index + 2].position = br;
            vertices[6 * vertex_index + 3].position = tl;
            vertices[6 * vertex_index + 4].position = br;
            vertices[6 * vertex_index + 5].position = bl;

            Vector2f half_inv_uv_size = (Vector2f) {
                .x = 0.5f / (float)text->font.glyph_atlas.size.x,
                .y = 0.5f / (float)text->font.glyph_atlas.size.y
            };
            Vector2f one_inv_uv_size = (Vector2f) {
                .x = 1.f / (float)text->font.glyph_atlas.size.x,
                .y = 1.f / (float)text->font.glyph_atlas.size.y
            };

            vertices[6 * vertex_index + 0].uv_coordinate = (Vector2f) {
                .x = glyph.uv_coordinates.x + half_inv_uv_size.x,
                .y = glyph.uv_coordinates.y + half_inv_uv_size.y
            };
            vertices[6 * vertex_index + 1].uv_coordinate = (Vector2f) {
                .x = glyph.uv_coordinates.x + glyph.uv_size.x - one_inv_uv_size.x,
                .y = glyph.uv_coordinates.y + half_inv_uv_size.y
            };
            vertices[6 * vertex_index + 2].uv_coordinate = (Vector2f) {
                .x = glyph.uv_coordinates.x + glyph.uv_size.x - one_inv_uv_size.x,
                .y = glyph.uv_coordinates.y + glyph.uv_size.y - one_inv_uv_size.y
            };
            vertices[6 * vertex_index + 3].uv_coordinate = (Vector2f) {
                .x = glyph.uv_coordinates.x + half_inv_uv_size.x,
                .y = glyph.uv_coordinates.y + half_inv_uv_size.y
            };
            vertices[6 * vertex_index + 4].uv_coordinate = (Vector2f) {
                .x = glyph.uv_coordinates.x + glyph.uv_size.x - one_inv_uv_size.x,
                .y = glyph.uv_coordinates.y + glyph.uv_size.y - one_inv_uv_size.y
            };
            vertices[6 * vertex_index + 5].uv_coordinate = (Vector2f) {
                .x = glyph.uv_coordinates.x + half_inv_uv_size.x,
                .y = glyph.uv_coordinates.y + glyph.uv_size.y - one_inv_uv_size.y
            };

            vertices[6 * vertex_index + 0].color = (VertexColor) {
                .r = 1.f, .g = 1.f, .b = 1.f, .a = 1.f
            };
            vertices[6 * vertex_index + 1].color = (VertexColor) {
                .r = 1.f, .g = 1.f, .b = 1.f, .a = 1.f
            };
            vertices[6 * vertex_index + 2].color = (VertexColor) {
                .r = 1.f, .g = 1.f, .b = 1.f, .a = 1.f
            };
            vertices[6 * vertex_index + 3].color = (VertexColor) {
                .r = 1.f, .g = 1.f, .b = 1.f, .a = 1.f
            };
            vertices[6 * vertex_index + 4].color = (VertexColor) {
                .r = 1.f, .g = 1.f, .b = 1.f, .a = 1.f
            };
            vertices[6 * vertex_index + 5].color = (VertexColor) {
                .r = 1.f, .g = 1.f, .b = 1.f, .a = 1.f
            };

            cursor_position.x += glyph.horizontal_metrics.advance;

            vertex_index++;
        }

        glBindVertexArray(text->_render_info.vao_handle);

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
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, text->font._texture.id);
    glBindVertexArray(text->_render_info.vao_handle);
    glDrawArrays(GL_TRIANGLES, 0, text->_render_info.vertex_count);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void destroy_text(Text text) {
    del_string(text.string);
    glDeleteVertexArrays(1, &text._render_info.vao_handle);
    glDeleteBuffers(1, &text._render_info.vbo_handle);
}
