#include "window.h"

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
