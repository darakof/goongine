#include "engine/engine_main.hpp"
#include "engine/engine_variable_definitions.hpp"
#include "engine/renderer/shader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "image/stb_image.h"

#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <cstring>

// callback function to make the window's darawable size be the size of the actual window
void Engine::window_size_callback(GLFWwindow* window, int width, int height) {
    if (Engine::DEBUG_MODE) {
        printf("window resized, new size: width %d, height %d\n", width, height);
    }
    glViewport(0,0,width,height);
}

// get command line arguments where argc is the ammount and argv is the strings that are the argument (argv[1] is always the executable)
int Engine::engine_main(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++) {
        if (argc < 2) {
            break;
        }
        if (strcmp(argv[i], "-debug") == 0) {
            Engine::DEBUG_MODE = true;
        }
    }

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1154, 630, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback(window, window_size_callback);

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    fprintf(stdout, "using GLEW version %s\n", glewGetString(GLEW_VERSION));
    int backend = glfwGetPlatform();
    printf("GLFW backend: %x\n", backend);

    float vertices[28] = {
        // Positions   // Colors (all white)   // Texture Coords
         1.0f,  1.0f,  1.0f,1.0f,1.0f,  1.0f, 1.0f,
         1.0f, -1.0f,  1.0f,1.0f,1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,1.0f,1.0f,  0.0f, 0.0f,
        -1.0f,  1.0f,  1.0f,1.0f,1.0f,  0.0f, 1.0f
    };

    unsigned int indices[6] = {
        0, 1, 3,
        1, 2, 3
    };

    unsigned int texture;
    stbi_set_flip_vertically_on_load(true);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load("../assets/textures/theodore.png", &width, &height, &nrChannels, STBI_rgb_alpha);

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,7*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5*sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    Engine::Renderer::ShaderProgram shaderProgram("../assets/shaders/basic.vert", "../assets/shaders/basic.frag");

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {        
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        shaderProgram.Use();
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

bool Engine::getDebugMode() {
    return Engine::DEBUG_MODE;
}