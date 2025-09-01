#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLFWwindow *createWindow();
void processInput(GLFWwindow *window);

#endif
