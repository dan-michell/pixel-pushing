#include "hello_triangle.h"

#include <array>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

bool WIREFRAME{false};

// GLSL vertex shaders
const char *vertexShaderSource{"#version 330 core\n"
                               "layout (location = 0) in vec3 aPos;\n"
                               "void main()\n"
                               "{\n"
                               "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                               "}\0"};

const char *fragmentShaderSourceOrange{"#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "void main()\n"
                                       "{\n"
                                       "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                       "}\n"};

const char *fragmentShaderSourceYellow{"#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "void main()\n"
                                       "{\n"
                                       "FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
                                       "}\n"};

// std::array<const char *, 2> fragmentShaderSources{fragmentShaderSource,
// fragmentShaderSourceYellow};

void shader_compile_success(const unsigned int *shader) {
    int success;
    char infoLog[512];
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(*shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

void shader_link_success(const unsigned int *shaderProgram) {
    int success;
    char infoLog[512];

    glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(*shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER_PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
    }
}

unsigned int get_shader_program(const char *fragmentShaderSource) {
    // In order for OpenGL to use the shader it has to dynamically compile it at run-time from its
    // source code. The first thing we need to do is create a shader object, again referenced by an
    // ID. GL_VERTEX_SHADER specifies we want to create a... vertex shader
    unsigned int vertexShader{glCreateShader(GL_VERTEX_SHADER)};
    // Attach shader source to shader object. We're only passing 1 GLSL string
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    shader_compile_success(&vertexShader);

    // Fragment shader to colour the triangle
    const unsigned int fragmentShader{glCreateShader(GL_FRAGMENT_SHADER)};
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shader_compile_success(&fragmentShader);

    // Add our shaders to the program and link them
    const unsigned int shaderProgram{glCreateProgram()};
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    shader_link_success(&shaderProgram);

    // We no longer need to store shaders after they've been compiled and linked
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

unsigned int get_triangle_vertex_array(std::array<float, 9> &vertices) {
    // Generate (v)ertex (b)uffer (o)bject and (v)ertex (a)rray (o)bject IDs
    unsigned int VBO, VAO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind vertex array first, then bind vertex buffers, then configure vertex attributes
    glBindVertexArray(VAO);

    // Bind vertex buffer to GL_ARRAY_BUFFER object
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Send our vertex data into the buffer. Passes GL_ARRAY_BUFFER object which is now bound to our
    // VBO buffer ID. Needs to also take the size of the vertex data along with the data itself. The
    // fourth parameter can be one of:
    //   GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
    //   GL_STATIC_DRAW: the data is set only once and used many times.
    //   GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    // 0 = Location of position vertex, defined in shader
    // 3 = Size of vertex attribute. We're using vec3.
    // GL_FLOAT = Type of vertex data.
    // GL_FALSE = If the data needs to be normalised. We're already in the range -1 <= x <= 1.
    // 3 * sizeof(float) = Spacing between each vertex attribute. In our array 'vertices' the new
    // vertex attribute begins 3 * sizeof(float) away from the start of the first.
    // (void *)0 = Offset to where the vertex position data starts in the array. Ours begins
    // immediately.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    // Enable vertex attribute. 0 is the position of our vertex attribute in the VBO
    glEnableVertexAttribArray(0);

    // We can safely unbind buffer now since glVertexAttribPointer registered VBO as the vertex
    // attribute's bound vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return VAO;
}

unsigned int get_rectangle_vertex_array() {
    // float vertices[]{
    //     // first triangle
    //     0.5f, 0.5f, 0.0f,  // top right
    //     0.5f, -0.5f, 0.0f, // bottom right
    //     -0.5f, 0.5f, 0.0f, // top left
    //     // second triangle
    //     0.5f, -0.5f, 0.0f,  // bottom right
    //     -0.5f, -0.5f, 0.0f, // bottom left
    //     -0.5f, 0.5f, 0.0f   // top left
    // };

    // We can use an (E)lement (B)uffer (O)bject to store only the unique vertices instead, and
    // specify the order with indices
    float vertices[] = {
        0.5f,  0.5f,  0.0f, // top right
        0.5f,  -0.5f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f,  0.0f  // top left
    };
    unsigned int indices[] = {
        // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    // The target is GL_ELEMENT_ARRAY_BUFFER instead of GL_ARRAY_BUFFER
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    return VAO;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // Called by GLFW whenever the window is manually resized
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    // Checks for esc key press, closes window if pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        WIREFRAME = !WIREFRAME;

        WIREFRAME ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
                  : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

int hello_triangle() {
    // GLFW initialisation and configuration
    // -------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // GLFW window creation
    // --------------------
    GLFWwindow *window{glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL)};

    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetKeyCallback(window, key_callback);

    // Load GLFW function pointers
    // ---------------------------
    // GLAD manages function pointers for OpenGL. Initalise this before calling any OpenGl function
    // These function pointers are OS specific, glfwGetProcAddress defines the correct function
    // based on which OS we're compiling for
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    unsigned int shaderProgramOrange = get_shader_program(fragmentShaderSourceOrange);
    unsigned int shaderProgramYellow = get_shader_program(fragmentShaderSourceYellow);

    // Setup vertex data and attributes
    // --------------------------------
    std::array<float, 9> verticesOne{-0.5f, 0.25f,  0.0f,   -0.75f, -0.25f,
                                     0.0f,  -0.25f, -0.25f, 0.0f};
    std::array<float, 9> verticesTwo{0.5f, 0.25f, 0.0f, 0.75f, -0.25f, 0.0f, 0.25f, -0.25f, 0.0f};

    unsigned int VAO1 = get_triangle_vertex_array(verticesOne);
    unsigned int VAO2 = get_triangle_vertex_array(verticesTwo);
    // unsigned int VAO = get_rectangle_vertex_array();

    std::cout << VAO2;

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgramOrange);
        glBindVertexArray(VAO1);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glUseProgram(shaderProgramYellow);
        glBindVertexArray(VAO2);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        //   0 = Starting index of vertex array
        //   3 = How many vertices we want to draw. We only want to draw 1 triangle with 3
        //   vertices
        // Use draw elements when an EBO is involved i.e. for a rectangle
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // De-allocate all resources once they've outlived their purpose:
    // --------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO1);
    glDeleteProgram(shaderProgramOrange);

    glDeleteVertexArrays(1, &VAO2);
    glDeleteProgram(shaderProgramYellow);

    glfwTerminate();

    return 0;
}
