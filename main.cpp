#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

// GLSL vertex shaders
const char *vertexShaderSource{"#version 330 core\n"
                               "layout (location = 0) in vec3 aPos;\n"
                               "void main()\n"
                               "{\n"
                               "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                               "}\0"};

const char *fragmentShaderSource{"#version 330 core\n"
                                 "out vec4 FragColor;\n"
                                 "void main()\n"
                                 "{\n"
                                 "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                 "}\n"};

void shader_compile_success(unsigned int *shader) {
    int success;
    char infoLog[512];
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(*shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

void shader_link_success(unsigned int *shaderProgram) {
    int success;
    char infoLog[512];

    glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(*shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER_PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
    }
}

unsigned int get_shader_program() {
    // In order for OpenGL to use the shader it has to dynamically compile it at run-time from its
    // source code. The first thing we need to do is create a shader object, again referenced by an
    // ID. GL_VERTEX_SHADER specifies we want to create a... vertex shader
    unsigned int vertexShader{glCreateShader(GL_VERTEX_SHADER)};
    // Attach shader souce to shader object. We're only passing 1 GLSL string
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    shader_compile_success(&vertexShader);

    // Fragment shader to colour the triangle
    unsigned int fragmentShader{glCreateShader(GL_FRAGMENT_SHADER)};
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shader_compile_success(&fragmentShader);

    // Add our shaders to the program and link them
    unsigned int shaderProgram{glCreateProgram()};
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    shader_link_success(&shaderProgram);

    // We no longer need to store shaders after they've been compiled and linked
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

unsigned int get_vertex_array() {
    // Setup vertex data and attributes
    // --------------------------------
    float vertices[]{0.0f, 0.5f, 0.0f, 0.5f, -0.5f, 0.0f, -0.5f, -0.5f, 0.0f};

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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

void draw_triangle() {}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // Called by GLFW whenever the window is manually resized
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    // Checks for esc key press, closes window if pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
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

    // Load GLFW function pointers
    // ---------------------------
    // GLAD manages function pointers for OpenGL. Initalise this before calling any OpenGl function
    // These function pointers are OS specific, glfwGetProcAddress defines the correct function
    // based on which OS we're compiling for
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    unsigned int shaderProgram = get_shader_program();
    unsigned int VAO = get_vertex_array();

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        // 0 = Starting index of vertex array
        // 3 = How many vertices we want to draw. We only want to draw 1 triangle with 3 vertices
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // De-allocate all resources once they've outlived their purpose:
    // --------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();

    return 0;
}
