#include "window.h"

#include <iostream>

static bool WIREFRAME{false};

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // Called by GLFW whenever the window is manually resized
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        WIREFRAME = !WIREFRAME;

        WIREFRAME ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
                  : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

GLFWwindow *createWindow() {
    // GLFW initialisation and configuration
    // -------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // GLFW window creation
    // --------------------
    GLFWwindow *window{glfwCreateWindow(800, 600, "Shaders", NULL, NULL)};

    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    // Load GLFW function pointers
    // ---------------------------
    // GLAD manages function pointers for OpenGL. Initalise this before
    // calling any OpenGl function These function pointers are OS specific,
    // glfwGetProcAddress defines the correct function based on which OS
    // we're compiling for
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    return window;
}
