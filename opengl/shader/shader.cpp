#include "shader.hpp"

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
    std::string vertexCode{fileToString(vertexPath)};
    std::string fragmentCode{fileToString(fragmentPath)};

    this->setVertexShader(vertexCode);
    this->setFragmentShader(fragmentCode);

    this->setShaderProgram();

    glDeleteShader(this->vertexShader());
    glDeleteShader(this->fragmentShader());
}

unsigned int Shader::createShader(std::string &shaderCode, GLenum type) {
    const char *codePtr = shaderCode.c_str();

    unsigned int shaderID{glCreateShader(type)};

    glShaderSource(shaderID, 1, &codePtr, nullptr);
    glCompileShader(shaderID);

    Shader::shaderCompileSuccess(shaderID, type);

    return shaderID;
}

unsigned int Shader::createProgram() {
    unsigned int programID{glCreateProgram()};

    glAttachShader(programID, this->vertexShader());
    glAttachShader(programID, this->fragmentShader());

    glLinkProgram(programID);

    Shader::shaderLinkSuccess(programID);

    return programID;
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(this->shaderProgram(), name.c_str()),
                (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(this->shaderProgram(), name.c_str()),
                value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(this->shaderProgram(), name.c_str()),
                value);
}

GLfloat Shader::getFloat(const std::string &name) const {
    GLfloat val{};

    GLint location{glGetUniformLocation(this->shaderProgram(), name.c_str())};
    glGetUniformfv(this->shaderProgram(), location, &val);

    return val;
}

void Shader::shaderCompileSuccess(unsigned int &shader, GLenum type) {
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::" << type << "::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }
}

void Shader::shaderLinkSuccess(unsigned int &shaderProgram) {
    int success;
    char infoLog[512];

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER_PROGRAM::LINK_FAILED\n"
                  << infoLog << std::endl;
    }
}
