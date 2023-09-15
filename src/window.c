#include "window.h"

void window_display(Window *window) {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window->window);
}

void window_set_clear_color(Window *window, ColorRGB color) {
    FColorRGB f_color = color_rgb_to_float(&color);
    glClearColor(f_color.r, f_color.g, f_color.b, 1.f);
}

bool has_window_requested_close(Window *window) {
    return glfwWindowShouldClose(window->window);
}

bool create_window(Window *window) {
    window->window = glfwCreateWindow(640, 480, "Hello, Window!", NULL, NULL);
    if (!window->window) {
        return false;
    }

    return true;
}

void destroy_window(Window *window) {
    glfwDestroyWindow(window->window);
}
