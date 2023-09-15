#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>

#include <GLFW/glfw3.h>

typedef struct Window {
    GLFWwindow* window;
} Window;

bool create_window(Window *window);
void destroy_window(Window *window);

#endif
