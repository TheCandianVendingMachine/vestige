#include <stdlib.h>
#include <math.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "game/ui_test.h"
#include "game/game_states.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_GAME
#include "logger.h"
#include "render/vertex.h"
#include "engine.h"
#include "lib/clock.h"

#include "render/primitives.h"

struct _MetaGlyph {
    AABB bounds;
    uint8_t* pixels;
    char c;
};
int _compare_meta_glyphs(const void* p_lhs, const void* p_rhs) {
    struct _MetaGlyph lhs = *(struct _MetaGlyph*)p_lhs;
    struct _MetaGlyph rhs = *(struct _MetaGlyph*)p_rhs;

    float a0 = aabb_area(lhs.bounds);
    float a1 = aabb_area(rhs.bounds);

    return (a0 < a1) - (a0 > a1);
}

const char* get_ft_error_message(FT_Error err) {
    #undef FTERRORS_H_
    #define FT_ERRORDEF( e, v, s )  case e: return s;
    #define FT_ERROR_START_LIST     switch (err) {
    #define FT_ERROR_END_LIST       }
    #include FT_ERRORS_H
    return "(Unknown error)";
}

void init_font_engine(FontEngine* engine) {
    log_debug("Loading Freetype font engine");
    engine->manager.font_map = GHASHMAP(Font, cstrhash);
    FT_Error error;
    error = FT_Init_FreeType(&engine->_library);
    if (error) {
        log_error("Cannot initialise Freetype: %s", get_ft_error_message(error));
        engine_crash(SHUTDOWN_CANT_INIT_FREETYPE);
    }
}

#include "stb_image_write.h"
void load_font_file(FontEngine* engine, const char* id, const char* path) {
    log_debug("Loading font [id: %s]", id);
    FT_Error error;
    Font font;
    error = FT_New_Face(engine->_library, path, 0, &font._face);
    if (error) {
        log_warning("Cannot load font [id: %s]: %s", id, get_ft_error_message(error));
    }

    error = FT_Set_Char_Size(
        font._face,
        0,
        24 * 64,
        ENGINE->window.dpi,
        0
    );
    if (error) {
        log_warning("Error setting character size [id: %s]: %s", id, get_ft_error_message(error));
    }

    // Packing algorithm: MAXRECTS
    // Jukka Jylänki, "A Thousand Ways to Pack the Bin - A Practical Approach to 
    // Two-Dimensional Rectangle Bin Packing", http://pds25.egloos.com/pds/201504/21/98/RectangleBinPack.pdf
    Vector meta_glyphs = VECTOR(struct _MetaGlyph);

    font.glyphs = VECTOR(Glyph);
    font.glyph_map = GHASHMAP(size_t, inthash);
    for (char c = '!'; c <= '~'; c++) {
        log_debug("Generating glyph %c for [id: %s]", c, id);
        unsigned int glyph_index = FT_Get_Char_Index(font._face, c);
        log_debug_verbose(1, "Loading");
        error = FT_Load_Glyph(font._face, glyph_index, 0);
        if (error) {
            log_warning("Error generating glyph '%c' for font [id: %s]: %s", c, id, get_ft_error_message(error));
            continue;
        }

        log_debug_verbose(1, "Rendering");
        error = FT_Render_Glyph(font._face->glyph, FT_RENDER_MODE_NORMAL);
        if (error) {
            log_warning("Error rendering glyph '%c' for font [id: %s]: %s", c, id, get_ft_error_message(error));
            continue;
        }

        log_debug_verbose(1, "Pushing", c, font.glyphs.length);
        Glyph glyph;
        // Divide by 64 to get from points -> pixels
        glyph.bounds.size.x = font._face->glyph->metrics.width / 64.f;
        glyph.bounds.size.y = font._face->glyph->metrics.height / 64.f;
        glyph.bounds.position = (Vector2f) { { 0.f, 0.f } };
        log_debug_verbose(3, "Glyph size: [%f, %f]", glyph.bounds.size.x, glyph.bounds.size.y);
        log_debug_verbose(2, "Assigning key '%c' to value '%d'", c, font.glyphs.length);
        hashmap_set(&font.glyph_map, &c, &font.glyphs.length);
        log_debug_verbose(2, "Pushing glyph to vector");
        VECTOR_PUSH(Glyph, &font.glyphs, glyph);

        struct _MetaGlyph meta_glyph;
        meta_glyph.c = c;
        meta_glyph.bounds = glyph.bounds;
        size_t pixel_length = font._face->glyph->bitmap.rows * font._face->glyph->bitmap.pitch;
        meta_glyph.pixels = malloc(pixel_length);
        memcpy(meta_glyph.pixels, font._face->glyph->bitmap.buffer, pixel_length);
        log_debug_verbose(3, "Pixel buffer [width, rows]: [%d, %d]", font._face->glyph->bitmap.width, font._face->glyph->bitmap.rows);
        log_debug_verbose(3, "Pixel buffer pitch: %d", font._face->glyph->bitmap.pitch);
        log_debug_verbose(3, "Pixel buffer size: %d bytes", pixel_length);
        VECTOR_PUSH(struct _MetaGlyph, &meta_glyphs, meta_glyph);
    }

    log_debug("Packing glyphs for font [id: %s]", id);
    Clock pack_clock = new_clock();
    // Sort glyphs according to some heuristic for an optimal packing
    qsort(meta_glyphs.buffer, meta_glyphs.length, sizeof(struct _MetaGlyph), _compare_meta_glyphs);

    struct MetaFace {
        AABB bounds;
        size_t glyph;
    };
    Vector placed_glyphs = VECTOR(struct MetaFace);
    Vector face_list = VECTOR(struct MetaFace);
    const Vector2f ATLAS_RESOLUTION = { { 216.f, 220.f } };
    VECTOR_PUSH(struct MetaFace, &face_list, ((struct MetaFace) {
        .bounds = (AABB) {
            .size = ATLAS_RESOLUTION,
            .position = (Vector2f) { { 0.f, 0.f } }
        },
        .glyph = -1
    }));

    Vector faces_to_delete = VECTOR(size_t);
    for (int i = 0; i < meta_glyphs.length; i++) {
        // Find minimum area face we can put meta-glyph into
        const struct _MetaGlyph glyph = _VECTOR_GET(struct _MetaGlyph, &meta_glyphs, i);
        float min_area = INFINITY;
        int best_face = 0;
        for (int j = 0; j < face_list.length; j++) {
            struct MetaFace f = _VECTOR_GET(struct MetaFace, &face_list, j);
            float face_area = f.bounds.size.x * f.bounds.size.y;
            // the area is minimal and the glyph can fit within the rectangle
            if (face_area < min_area &&
                f.bounds.size.x >= glyph.bounds.size.x && f.bounds.size.y >= glyph.bounds.size.y) {
                min_area = face_area;
                best_face = j;
            }
        }

        if (min_area == INFINITY) {
            log_error("Could not generate packed glyph for '%c' [id: %s]", glyph.c, id);
            continue;
        }

        log_debug_verbose(2, "Best area found to pack: %f", min_area);

        // We place the glyph in the top-left of the rectangle, so we have
        // new_width = face.width - glyph.width,
        // new_height = face.height - glyph.height

        const float best_width = _VECTOR_GET(struct MetaFace, &face_list, best_face).bounds.size.x;
        const float best_height = _VECTOR_GET(struct MetaFace, &face_list, best_face).bounds.size.y;
        const float best_x = _VECTOR_GET(struct MetaFace, &face_list, best_face).bounds.position.x;
        const float best_y = _VECTOR_GET(struct MetaFace, &face_list, best_face).bounds.position.y;

        log_debug_verbose(2, "Placing '%c' at [%f, %f]", glyph.c, best_x, best_y);

        struct MetaFace placed_glyph = (struct MetaFace) {
            .bounds = (AABB) {
                .size = glyph.bounds.size,
                .position = (Vector2f) { { best_x, best_y } }
            },
            .glyph = i
        };
        VECTOR_PUSH(struct MetaFace, &placed_glyphs, placed_glyph);
        struct MetaFace f_horizontal;
        struct MetaFace f_vertical;

        const float new_width = best_width - glyph.bounds.size.x;
        const float new_height = best_height - glyph.bounds.size.y;

        f_horizontal.bounds.position.x = best_x;
        f_horizontal.bounds.position.y = best_y + glyph.bounds.size.y;
        f_horizontal.bounds.size.x = best_width;
        f_horizontal.bounds.size.y = new_height;

        f_vertical.bounds.position.x = best_x + glyph.bounds.size.x;
        f_vertical.bounds.position.y = best_y;
        f_vertical.bounds.size.x = new_width;
        f_vertical.bounds.size.y = best_height;

        VECTOR_PUSH(struct MetaFace, &face_list, f_horizontal);
        VECTOR_PUSH(struct MetaFace, &face_list, f_vertical);

        faces_to_delete.length = 0;
        VECTOR_PUSH(size_t, &faces_to_delete, best_face);

        // When adding new faces, there can be an intersection that forms with the newly
        // placed glyph
        // If this occurs, we will cull the AABB so it not longer intersects the glyph
        for (int j = 0; j < face_list.length; j++) {
            struct MetaFace face = _VECTOR_GET(struct MetaFace, &face_list, j);
            if (j != best_face && aabb_intersect(face.bounds, placed_glyph.bounds)) {
                int dx = placed_glyph.bounds.position.x - face.bounds.position.x;
                int dy = placed_glyph.bounds.position.y - face.bounds.position.y;

                if (dx > dy) {
                    face.bounds.size.x = dx;
                    VECTOR_SET(struct MetaFace, &face_list, j, face);
                } else {
                    face.bounds.size.y = dy;
                    VECTOR_SET(struct MetaFace, &face_list, j, face);
                }

                if (face.bounds.size.y <= 0 || face.bounds.size.x <= 0) {
                    VECTOR_PUSH(size_t, &faces_to_delete, j);
                }
            }
        }

        for (int j = 0; j < faces_to_delete.length; j++) {
            size_t face_to_delete = _VECTOR_GET(size_t, &faces_to_delete, j);
            if (face_list.length > 0) {
                struct MetaFace swap_face = _VECTOR_GET(struct MetaFace, &face_list, face_list.length - 1);
                _VECTOR_SET(struct MetaFace, &face_list, face_to_delete, swap_face);
            }
            _VECTOR_POP(struct MetaFace, &face_list);
        }
    }
#define DEBUG_FONT_WRITE_ATLAS
#ifdef DEBUG_FONT_WRITE_ATLAS
    log_debug("Writing [id: %s] texture atlas to disk", id);
    const unsigned int channels = 3;
    uint8_t* pixels = malloc(ATLAS_RESOLUTION.x * ATLAS_RESOLUTION.y * channels);
    memset(pixels, 0, ATLAS_RESOLUTION.x * ATLAS_RESOLUTION.y * channels);
    for (int i = 0; i < placed_glyphs.length; i++) {
        struct MetaFace face = _VECTOR_GET(struct MetaFace, &placed_glyphs, i);
        struct _MetaGlyph glyph = _VECTOR_GET(struct _MetaGlyph, &meta_glyphs, face.glyph);
        log_debug_verbose(4, "Drawing '%c' at [%d, %d]", glyph.c, (int)face.bounds.position.x, (int)face.bounds.position.y);
        for (int y = 0; y < face.bounds.size.y; y++) {
            for (int x = 0; x < face.bounds.size.x; x++) {
                int write_index = channels * ((x + face.bounds.position.x) + (y + face.bounds.position.y) * ATLAS_RESOLUTION.x);
                int read_index = x + y * face.bounds.size.x;

                pixels[write_index] = glyph.pixels[read_index];
            }
        }
    }

#define DEBUG_FONT_WRITE_ATLAS_DRAW_FACES
#ifdef DEBUG_FONT_WRITE_ATLAS_DRAW_FACES
    log_debug_verbose(1, "Rendering face squares");
    for (int i = 0; i < face_list.length; i++) {
        struct MetaFace face = _VECTOR_GET(struct MetaFace, &face_list, i);
        int px = face.bounds.position.x;
        int py = face.bounds.position.y;
        int sx = face.bounds.size.x;
        int sy = face.bounds.size.y;
        for (int x = 0; x < face.bounds.size.x; x++) {
            int write_index = channels * (x + px + py * ATLAS_RESOLUTION.x);
            pixels[write_index + 2] = 255;
            write_index = channels * (x + px + (py + sy - 1) * ATLAS_RESOLUTION.x);
            pixels[write_index + 2] = 255;
        }
        for (int y = 0; y < face.bounds.size.y; y++) {
            int write_index = channels * (px + (y + py) * ATLAS_RESOLUTION.x);
            pixels[write_index + 2] = 255;
            write_index = channels * ((px + sx - 1) + (y + py) * ATLAS_RESOLUTION.x);
            pixels[write_index + 2] = 255;
        }
    }
#endif
    log_debug_verbose(1, "Writing to disk");
    stbi_write_bmp(id, ATLAS_RESOLUTION.x, ATLAS_RESOLUTION.y, channels, pixels);
    free(pixels);
#endif
    del_vector(placed_glyphs);
    del_vector(face_list);

    log_debug_verbose(1, "Fonts packed in %d microseconds", time_as_microseconds(get_elapsed_time(&pack_clock)));
    for (int i = 0; i < meta_glyphs.length; i++) {
        free(_VECTOR_GET(struct _MetaGlyph, &meta_glyphs, i).pixels);
    }
    del_vector(meta_glyphs);

    hashmap_set(&engine->manager.font_map, id, &font);
}

void ui_test_push(GameState* state) {
    state->stored_state = malloc(sizeof(UiTestState));

    UiTestState* s = (UiTestState*)state->stored_state;

    init_font_engine(&s->font_engine);
    load_font_file(&s->font_engine, "default", "/home/bailey/.fonts/RobotoMono/RobotoMonoNerdFont-Medium.ttf");

    Shader vs = load_vertex_shader_from_disk("shaders/text_test_shader.vert").data.ok;
    Shader fs = load_fragment_shader_from_disk("shaders/text_test_shader.frag").data.ok;
    s->test_shader = create_shader_program(vs, fs).data.ok;

    s->projection = matrix_orthographic_projection(
        0.f, -1.f * ENGINE->window.size.x,
        0.f, ENGINE->window.size.y,
        -100.f, 100.f
    );
    s->view = new_camera();
    float sx = 500.f;
    float sy = 100.f;
    float tx = 1280 * 0.5f - sx * 0.5f;
    float ty = 720 * 0.5f - sy * 0.5f;
    float m[] = {
        sx, 0.f, 0.f, 0.f,
        0.f, sy, 0.f, 0.f,
        0.f, 0.f,0.f, 0.f,
        tx, ty,  0.f, 1.f
    };
    memcpy(s->model.entries, m, sizeof(s->model.entries));

    glGenVertexArrays(1, &s->vao);
    bind_primitive_to_vao(primitive_quad(), s->vao);
}

void ui_test_pop(GameState* state) {
    free(state->stored_state);
}

void ui_test_update(GameState* state, float delta_time) {
}

void ui_test_render(GameState* state) {
    UiTestState* s = (UiTestState*)state->stored_state;

    int projectionPosition = glGetUniformLocation(s->test_shader._program, "projection");
    int modelPosition = glGetUniformLocation(s->test_shader._program, "model");
    int viewPosition = glGetUniformLocation(s->test_shader._program, "view");

    glUseProgram(s->test_shader._program);
    glUniformMatrix4fv(projectionPosition, 1, false, s->projection.entries);
    glUniformMatrix4fv(modelPosition, 1, false, s->model.entries);
    glUniformMatrix4fv(viewPosition, 1, false, camera_view(&s->view).entries);
    draw_primitive(primitive_quad(), s->vao);
}
