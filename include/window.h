#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>

#include <GLFW/glfw3.h>

typedef struct Window {
    GLFWwindow* window;
} Window;

void window_display(Window *window);
bool has_window_requested_close(Window *window);

bool create_window(Window *window);
void destroy_window(Window *window);

#endif
