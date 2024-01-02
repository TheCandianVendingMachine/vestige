#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "render/color.h"
#include "lib/math.h"

typedef struct Window {
    GLFWwindow* window;
    unsigned int dpi;
    Vector2i size;
    struct {
        Vector2f last_position;
        Vector2f current_position;
    } cursor;
} Window;

void window_clear(Window *window);
void window_display(Window *window);
void window_set_clear_color(Window *window, ColorRGB color);
bool has_window_requested_close(Window *window);

bool create_window(Window *window);
void destroy_window(Window *window);

#endif
