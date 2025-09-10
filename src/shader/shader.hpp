#ifndef SHADER_HPP
#define SHADER_HPP

#include "../util.hpp"

class Shader {
  public:
    Shader(const char *vertexPath, const char *fragmentPath);

    // const at end of member function ensures that no data members are modified
    const unsigned int &shaderProgram() const { return m_shaderProgram; };

    void setBool(const std::string &name, bool value) const;

    void setInt(const std::string &name, int value) const;

    void setFloat(const std::string &name, float value) const;
    GLfloat getFloat(const std::string &name) const;

    void use() const { glUseProgram(shaderProgram()); };

  private:
    unsigned int m_vertexShader;
    unsigned int m_fragmentShader;
    unsigned int m_shaderProgram;

    // Getters
    unsigned int vertexShader() const { return m_vertexShader; };
    unsigned int fragmentShader() const { return m_fragmentShader; };

    // Setters
    void setShaderProgram() { m_shaderProgram = createProgram(); };
    void setVertexShader(std::string &shaderCode) {
        m_vertexShader = createShader(shaderCode, GL_VERTEX_SHADER);
    };
    void setFragmentShader(std::string &shaderCode) {
        m_fragmentShader = createShader(shaderCode, GL_FRAGMENT_SHADER);
    };

    // Shader
    static unsigned int createShader(std::string &code, GLenum type);
    unsigned int createProgram();

    // Helper
    static void shaderCompileSuccess(unsigned int &shader, GLenum type);
    static void shaderLinkSuccess(unsigned int &shaderProgram);
};

#endif
