#include "stb_image_write.h"
#include "ui/font_engine.h"
#include "ui/font.h"
#include "ui/glyph.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_UI
#include "logger.h"
#include "engine.h"

typedef struct _MetaGlyph {
    uint8_t* pixels;
    int pitch;
    int glyph_index;
    char c;
} _MetaGlyph;

void draw_meta_glyph(uint8_t* pixels, int channels, Vector2i atlas_size, AtlasEntry entry) {
    _MetaGlyph* meta_glyph = (_MetaGlyph*)entry.user_data;
    log_debug_verbose(4, "Drawing '%c' with size [%d, %d]", meta_glyph->c, (int)entry.bounds.size.x, (int)entry.bounds.size.y);
    for (int y = 0; y < entry.bounds.size.y; y++) {
        for (int x = 0; x < entry.bounds.size.x; x++) {
            int out_x = x + entry.bounds.position.x;
            int out_y = y + entry.bounds.position.y;

            pixels[channels * (out_x + out_y * atlas_size.x)] = meta_glyph->pixels[x + y * meta_glyph->pitch];
        }
    }
}

const char* get_ft_error_message(FT_Error err) {
    #undef FTERRORS_H_
    #define FT_ERRORDEF( e, v, s )  case e: return s;
    #define FT_ERROR_START_LIST     switch (err) {
    #define FT_ERROR_END_LIST       }
    #include FT_ERRORS_H
    return "(Unknown error)";
}

FontEngine new_font_engine(void) {
    FontEngine engine;

    log_debug("Loading Freetype font engine");
    engine.manager.font_map = GHASHMAP(Font, cstrhash);
    FT_Error error;
    error = FT_Init_FreeType(&engine._library);
    if (error) {
        log_error("Cannot initialise Freetype: %s", get_ft_error_message(error));
        engine_crash(SHUTDOWN_CANT_INIT_FREETYPE);
    }

    return engine;
}

void destroy_font_engine(FontEngine engine) {
    log_debug("Destroying Freetype font engine");
    FT_Done_FreeType(engine._library);
}

void load_font_from_disk(FontEngine engine, Vector2i atlas_size, const char* id, const char* path, int point_count, int* points) {
    log_debug("Loading font [id: %s]", id);
    FT_Error error;
    Font font;
    error = FT_New_Face(engine._library, path, 0, &font._face);
    if (error) {
        log_warning("Cannot load font [id: %s]: %s", id, get_ft_error_message(error));
    }

    Vector entries = VECTOR(AtlasEntry);
    font.glyphs = VECTOR(Glyph);
    font.char_glyph_map = GHASHMAP(size_t, inthash);
    for (int i = 0; i < point_count; i++) {
        error = FT_Set_Char_Size(
            font._face,
            0,
            points[i] * 64,
            ENGINE->window.dpi,
            0
        );
        if (error) {
            log_warning("Error setting character size [id: %s]: %s", id, get_ft_error_message(error));
            continue;
        }

        for (char c = '!'; c <= '~'; c++) {
            log_debug_verbose(1, "Generating glyph %c for [id: %s]", c, id);
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
            glyph.uv_coordinates = (Vector2f) { { 0.f, 0.f } };
            log_debug_verbose(3, "Glyph size: [%f, %f]", glyph.bounds.size.x, glyph.bounds.size.y);
            log_debug_verbose(2, "Assigning key '%c' to value '%d'", c, font.glyphs.length);
            hashmap_set(&font.char_glyph_map, &c, &font.glyphs.length);
            log_debug_verbose(2, "Pushing glyph to vector");
            VECTOR_PUSH(Glyph, &font.glyphs, glyph);

            _MetaGlyph* meta_glyph = malloc(sizeof(_MetaGlyph));
            meta_glyph->glyph_index = font.glyphs.length - 1;
            meta_glyph->c = c;
            meta_glyph->pitch = font._face->glyph->bitmap.pitch;
            size_t pixel_length = font._face->glyph->bitmap.rows * font._face->glyph->bitmap.pitch;
            meta_glyph->pixels = malloc(pixel_length);
            memcpy(meta_glyph->pixels, font._face->glyph->bitmap.buffer, pixel_length);
            log_debug_verbose(3, "Pixel buffer [width, rows]: [%d, %d]", font._face->glyph->bitmap.width, font._face->glyph->bitmap.rows);
            log_debug_verbose(3, "Pixel buffer pitch: %d", font._face->glyph->bitmap.pitch);
            log_debug_verbose(3, "Pixel buffer size: %d bytes", pixel_length);

            AtlasEntry glyph_entry;
            glyph_entry.bounds = glyph.bounds;
            glyph_entry.user_data = meta_glyph;
            VECTOR_PUSH(AtlasEntry, &entries, glyph_entry);
        }
    }

    if (!create_atlas(&font.glyph_atlas, entries, atlas_size)) {
        log_warning("Could not pack all glyphs into atlas");
    }

    // Set glyph texture info
    for (int i = 0; i < font.glyph_atlas.packed_entries.length; i++) {
        const AtlasEntry entry = _VECTOR_GET(AtlasEntry, &font.glyph_atlas.packed_entries, i);
        const _MetaGlyph* meta_glyph = entry.user_data;
        Glyph glyph = _VECTOR_GET(Glyph, &font.glyphs, meta_glyph->glyph_index);
        glyph.uv_coordinates = (Vector2f) {
            .x = entry.bounds.position.x / font.glyph_atlas.size.x,
            .y = entry.bounds.position.y / font.glyph_atlas.size.y
        };
        glyph.uv_size = (Vector2f) {
            .x = entry.bounds.size.x / font.glyph_atlas.size.x,
            .y = entry.bounds.size.y / font.glyph_atlas.size.y
        };
        _VECTOR_SET(Glyph, &font.glyphs, meta_glyph->glyph_index, glyph);
    }

    hashmap_set(&engine.manager.font_map, id, &font);
}

void draw_font_atlas(FontEngine engine, const char* id, const char* out_path) {
    const int channels = 3;
    Font* font = (Font*)hashmap_get(&engine.manager.font_map, id);
    uint8_t* pixels = draw_atlas(font->glyph_atlas, channels, draw_meta_glyph);
    stbi_write_bmp(out_path, font->glyph_atlas.size.x, font->glyph_atlas.size.y, channels, pixels);
    free(pixels);
}
