#include "window.h"
#define VESTIGE_LOG_CHANNEL LOG_CHANNEL_CORE
#include "logger.h"

void event_window_resize(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    Window *w = glfwGetWindowUserPointer(window);
    w->size.x = width;
    w->size.y = height;
}

void event_cursor_move(GLFWwindow* window, double xPos, double yPos) {
    Window* w = glfwGetWindowUserPointer(window);
    w->cursor.last_position = w->cursor.current_position;
    w->cursor.current_position.x = xPos;
    w->cursor.current_position.y = yPos;
}

void window_clear(Window *window) {
    glClear(GL_COLOR_BUFFER_BIT);
}

void window_display(Window *window) {
    glfwSwapBuffers(window->window);
}

void window_set_clear_color(Window *window, ColorRGB color) {
    FColorRGB f_color = color_rgb_to_float(color);
    glClearColor(f_color.r, f_color.g, f_color.b, 1.f);
}

bool has_window_requested_close(Window *window) {
    return glfwWindowShouldClose(window->window);
}

bool create_window(Window *window) {
    window->window = glfwCreateWindow(1280, 720, "Hello, Window!", NULL, NULL);

    if (!window->window) {
        return false;
    }

    window->cursor.current_position = (Vector2f) { .x = 0.f, .y = 0.f };
    window->cursor.last_position = (Vector2f) { .x = 0.f, .y = 0.f };
    window->size.x = 1280;
    window->size.y = 720;
    glfwSetWindowUserPointer(window->window, window);
    glfwSetWindowSizeCallback(window->window, event_window_resize);
    glfwSetCursorPosCallback(window->window, event_cursor_move);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    int widthMM, heightMM;
    glfwGetMonitorPhysicalSize(monitor, &widthMM, &heightMM);

    window->dpi = mode->width / (widthMM / 25.4);

    log_info("Created window with size [%d, %d] and DPI %d", window->size.x, window->size.y, window->dpi);

    return true;
}

void destroy_window(Window *window) {
    glfwDestroyWindow(window->window);
}
