#ifndef VESTIGE_TEXT_H
#define VESTIGE_TEXT_H

#include <stdbool.h>

#include "lib/string.h"
#include "ui/font.h"

typedef struct Text {
    struct {
        unsigned int vbo_handle;
        unsigned int vao_handle;
        unsigned int vertex_count;
        unsigned int point;
    } _render_info;
    String string;
    Font font;
    bool _updated;
} Text;

Text create_text(Font font);
void text_set_string(Text* text, const char* str);
void draw_text(Text* text);
void destroy_text(Text text);

#endif
