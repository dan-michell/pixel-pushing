#ifndef UTIL_HPP
#define UTIL_HPP

#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

inline std::string fileToString(const char *path) {
    std::string fileAsString{};
    std::ifstream file{};

    // ensure ifstream objects can throw exceptions:
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        // open files
        file.open(path);

        // read file's buffer contents into stream
        std::stringstream stream{};
        stream << file.rdbuf();

        // close file handler
        file.close();

        // convert stream into string
        fileAsString = stream.str();
    } catch (std::ifstream::failure &e) {
        std::cout << "ERROR:: " << path << " ::NOT_SUCCESFULLY_READ"
                  << std::endl;
    }

    return fileAsString;
}

#endif
