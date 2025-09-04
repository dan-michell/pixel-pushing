#ifndef WINDOW_H
#define WINDOW_H

#include "shader/shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLFWwindow *createWindow();
void processShaderInput(GLFWwindow *window, Shader &shader);

#endif
