#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

// Shader class.

class Shader {
  public:
    Shader(const char *vertexPath, const char *fragmentPath) {}
    void use(); // TODO: Call glUseProgram()
    void setBool(const std::string &name);
    void setInt();
    void setFloat();

  private:
    void shaderCompileSuccess(const unsigned int &shader, std::string &type);
    void shaderLinkSuccess(const unsigned int &shaderProgram);
};

#endif

// GLSL vertex shaders

void colorOverTime(const unsigned int shaderProgram) {
    float time = glfwGetTime();

    float green = (tan(time) / 2.0f) + 0.5f;

    int vertexColorUniform = glGetUniformLocation(shaderProgram, "vertexColor");

    glUseProgram(shaderProgram);

    // Choose the 4f variant as we are updating a vec4
    glUniform4f(vertexColorUniform, 0.0f, green, 0.0f, 1.0f);
}

unsigned int getShaderProgram(const char *vertexShaderSource,
                              const char *fragmentShaderSource) {
    // In order for OpenGL to use the shader it has to dynamically compile it at
    // run-time from its source code. The first thing we need to do is create a
    // shader object, again referenced by an ID. GL_VERTEX_SHADER specifies we
    // want to create a... vertex shader
    unsigned int vertexShader{glCreateShader(GL_VERTEX_SHADER)};
    // Attach shader source to shader object. We're only passing 1 GLSL string
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    shaderCompileSuccess(&vertexShader, "vertex");

    // Fragment shader to color the triangle
    const unsigned int fragmentShader{glCreateShader(GL_FRAGMENT_SHADER)};
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shaderCompileSuccess(&fragmentShader, "fragment");

    // Add our shaders to the program and link them
    const unsigned int shaderProgram{glCreateProgram()};
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    shaderLinkSuccess(&shaderProgram);

    // We no longer need to store shaders after they've been compiled and linked
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}
