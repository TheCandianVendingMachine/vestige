#include <stdio.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <GLFW/glfw3.h>

#include "window.h"
#include "engine.h"

GLFWwindow* APP = NULL;

static void main_loop(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(APP);
    glfwPollEvents();
}

int main(int argc, char* argv[]) {
    engine_start();

    if (!glfwInit()) {
        printf("Failed to initialize GLFW!\n");
        return 1;
    }

    APP = glfwCreateWindow(640, 480, "Hello, Window!", NULL, NULL);
    if (!APP) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(APP);

    glfwSwapInterval(1);

    glClearColor(255, 0, 0, 255);

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, 1);
#else
    while (!glfwWindowShouldClose(APP)) {
        main_loop();
    }
#endif

    glfwTerminate();

    return engine_stop();
}
