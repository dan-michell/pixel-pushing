#include "hello_triangle.h"
#include "../context.h"

#include <cmath>
#include <iostream>

unsigned int getTriangleVertexArray(std::array<float, 9> &vertices) {
    // Generate VBO ID -> Bind buffer to ID -> Add buffer vertex data -> Create
    // pointers to vertex attributes -> Activate specific vertex attribute in
    // array

    // Generate (v)ertex (b)uffer (o)bject and (v)ertex (a)rray (o)bject IDs
    unsigned int VBO, VAO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind vertex array first, then bind vertex buffers, then configure vertex
    // attributes
    glBindVertexArray(VAO);

    // Bind vertex buffer to GL_ARRAY_BUFFER object
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Send our vertex data into the buffer. Passes GL_ARRAY_BUFFER object which
    // is now bound to our VBO buffer ID. Needs to also take the size of the
    // vertex data along with the data itself. The fourth parameter can be one
    // of:
    //   GL_STREAM_DRAW: the data is set only once and used by the GPU at most a
    //   few times. GL_STATIC_DRAW: the data is set only once and used many
    //   times. GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    // 0 = Location of position vertex, defined in shader
    // 3 = Size of vertex attribute. We're using vec3.
    // GL_FLOAT = Type of vertex data.
    // GL_FALSE = If the data needs to be normalised. We're already in the range
    // -1 <= x <= 1. 3 * sizeof(float) = Spacing between each vertex attribute.
    // In our array 'vertices' the new vertex attribute begins 3 * sizeof(float)
    // away from the start of the first. (void *)0 = Offset to where the vertex
    // position data starts in the array. Ours begins immediately.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    // Enable vertex attribute. 0 is the position of our vertex attribute in the
    // VBO
    glEnableVertexAttribArray(0);

    // We can safely unbind buffer now since glVertexAttribPointer registered
    // VBO as the vertex attribute's bound vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return VAO;
}

unsigned int getTraiangleWithColorVertexArray(std::array<float, 18> &vertices) {
    unsigned int VBO, VAO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    // Enable pointer to color vertex attributes. Stride is 6 * sizeof(float) as
    // we need to jump over the color attributes to reach the next 'x' value.
    // The color attributes start 3 * sizeof(float) after the position data.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return VAO;
}

unsigned int getRectangleVertexArray() {
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

    // We can use an (E)lement (B)uffer (O)bject to store only the unique
    // vertices instead, and specify the order with indices
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    return VAO;
}

int helloTriangle() {
    GLFWwindow *window = createWindow();

    unsigned int shaderProgram = getShaderProgram(vertexShaderSourceWithColor,
                                                  fragmentShaderSourceDynamic);

    // Setup vertex data and attributes
    // --------------------------------
    std::array<float, 9> verticesOne{-0.5f, 0.25f,  0.0f,   -0.75f, -0.25f,
                                     0.0f,  -0.25f, -0.25f, 0.0f};
    std::array<float, 18> verticesWithColor{
        // positions         // colors
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
        0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f  // top
    };
    // std::array<float, 9> verticesTwo{0.5f, 0.25f, 0.0f, 0.75f, -0.25f, 0.0f,
    // 0.25f, -0.25f, 0.0f};

    unsigned int VAO1 = getTraiangleWithColorVertexArray(verticesWithColor);
    // unsigned int VAO1 = getTriangleVertexArray(verticesOne);
    // unsigned int VAO2 = getTriangleVertexArray(verticesTwo);
    // unsigned int VAO = getRectangleVertexArray();

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // colorOverTime(shaderProgram);
        glUseProgram(shaderProgram);

        glBindVertexArray(VAO1);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // glBindVertexArray(VAO2);
        // glDrawArrays(GL_TRIANGLES, 0, 3);

        //   0 = Starting index of vertex array
        //   3 = How many vertices we want to draw. We only want to draw 1
        //   triangle with 3 vertices
        // Use draw elements when an EBO is involved i.e. for a rectangle
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // De-allocate all resources once they've outlived their purpose:
    // --------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO1);
    glDeleteProgram(shaderProgram);

    // glDeleteVertexArrays(1, &VAO2);
    // glDeleteProgram(shaderProgramYellow);

    glfwTerminate();

    return 0;
}
