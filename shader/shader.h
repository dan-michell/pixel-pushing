#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

class Shader {
  public:
    Shader(const char *vertexPath, const char *fragmentPath);

    const unsigned int &shaderProgram() const { return m_shaderProgram; };

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;

    void use() { glUseProgram(shaderProgram()); };

  private:
    unsigned int m_vertexShader;
    unsigned int m_fragmentShader;
    unsigned int m_shaderProgram;

    // Getters
    const unsigned int vertexShader() { return m_vertexShader; };
    const unsigned int fragmentShader() { return m_fragmentShader; };

    // Setters
    void setShaderProgram() { m_shaderProgram = createProgram(); };
    void setVertexShader(std::string &shaderCode) {
        m_vertexShader = createShader(shaderCode, GL_VERTEX_SHADER);
    };
    void setFragmentShader(std::string &shaderCode) {
        m_fragmentShader = createShader(shaderCode, GL_FRAGMENT_SHADER);
    };

    // Shader
    unsigned int createShader(std::string &code, GLenum type);
    unsigned int createProgram();

    // Helper
    std::string fileToString(const char *path);
    void shaderCompileSuccess(unsigned int &shader, GLenum type);
    void shaderLinkSuccess(unsigned int &shaderProgram);
};

#endif
