#include "shader/shader.hpp"
#include "window.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static float mixPercent{0.5f};

unsigned int getTriangleVertexArray(std::array<float, 9> &vertices);
unsigned int getTraiangleWithColorVertexArray();
unsigned int getRectangleVertexArray();
void generateTexture(unsigned char *data, int &width, int &height,
                     int textureSelector, GLint format, GLint clamping,
                     GLint filtering);
void processShaderInput(GLFWwindow *window);

int main() {
    GLFWwindow *window = createWindow();

    // unsigned int VAO = getTraiangleWithColorVertexArray();
    unsigned int VAO = getRectangleVertexArray();

    const char *vertexShaderPath = "./src/shader/base.vert";
    const char *fragmentShaderPath = "./src/shader/base.frag";

    Shader shader{vertexShaderPath, fragmentShaderPath};

    // Texture
    int width, height, nrChannels;

    unsigned char *container =
        stbi_load("./assets/container.jpg", &width, &height, &nrChannels, 0);

    stbi_set_flip_vertically_on_load(true);
    unsigned char *face =
        stbi_load("./assets/awesomeface.png", &width, &height, &nrChannels, 0);

    if (container && face) {
        generateTexture(container, width, height, 0, GL_RGB, GL_CLAMP_TO_EDGE,
                        GL_NEAREST);

        generateTexture(face, width, height, 1, GL_RGBA, GL_CLAMP_TO_EDGE,
                        GL_NEAREST);
    } else {
        std::cerr << "Error loading texture from image" << std::endl;
    }

    stbi_image_free(container);
    stbi_image_free(face);

    shader.use();
    shader.setInt("texture2", 1);

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Use draw elements when an EBO is involved i.e. for a rectangle
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // Process shader related inputs
        processShaderInput(window);
        shader.setFloat("mixPercent", mixPercent);

        // GLFW: swap buffers and poll IO events (keys pressed/released, mouse
        // moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // De-allocate all resources once they've outlived their purpose:
    // --------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);

    glfwTerminate();

    return 0;
}

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    // Enable vertex attribute. 0 is the position of our vertex attribute in the
    // VBO
    glEnableVertexAttribArray(0);

    // We can safely unbind buffer now since glVertexAttribPointer registered
    // VBO as the vertex attribute's bound vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return VAO;
}

unsigned int getTraiangleWithColorVertexArray() {
    // Setup vertex data and attributes
    // --------------------------------
    std::array<float, 18> verticesWithColor{
        // positions         // colors
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
        0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f  // top
    };

    unsigned int VBO, VAO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesWithColor), &verticesWithColor,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
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
    // Positions | Colours | Texture coords
    std::array<float, 32> vertices = {
        // 0 -> 2 positions, 3 -> 5 colours, 6 -> 7 texture coords (s,t)
        0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
        0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
    };

    std::array<unsigned int, 6> indices = {
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    // The target is GL_ELEMENT_ARRAY_BUFFER instead of GL_ARRAY_BUFFER
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    return VAO;
}

void generateTexture(unsigned char *data, int &width, int &height,
                     int textureSelector, GLint format, GLint clamping,
                     GLint filtering) {
    unsigned int texture;
    glGenTextures(1, &texture);

    // GL_TEXTURE0 is activated by default. If we wanted to apply multiple
    // texture units we would need to activate them manually here
    glActiveTexture((GL_TEXTURE0 + textureSelector));
    glBindTexture(GL_TEXTURE_2D, texture);

    // Generate texture image on currently bound texture object 'texture'
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Clamping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamping);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamping);

    // Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
}

void processShaderInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        mixPercent += 0.01;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        mixPercent -= 0.01;
    }
}
