#include <GLFW/glfw3.h>

int main(int argc, char* argv[]) {
    if (!glfwInit()) {
        return 1;
    }

    GLFWwindow *app = glfwCreateWindow(640, 480, "Hello, Window!", NULL, NULL);
    if (!app) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(app);

    while (!glfwWindowShouldClose(app)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(app);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
