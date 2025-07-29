#include "engine/engine_main.hpp"
#include "engine/engine_variable_definitions.hpp"

#include <iostream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <cstring>

char* Engine::readShaderFile(const char* fileName) {
    FILE* file = fopen(fileName, "rb");  // Open in binary mode
    if (!file) {
        perror("Failed to open shader file");
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory (+1 for null terminator)
    char* buffer = (char*)malloc(length + 1);
    if (!buffer) {
        fclose(file);
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    // Read file content
    size_t bytes_read = fread(buffer, 1, length, file);
    if (bytes_read != (size_t)length) {
        fclose(file);
        free(buffer);
        fprintf(stderr, "Read error: expected %ld bytes, got %zu\n", length, bytes_read);
        return NULL;
    }

    // Null-terminate the string
    buffer[length] = '\0';
    fclose(file);
    return buffer;
}



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

    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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

    float vertices[8] = {
         0.5f,  0.5f,
         0.5f, -0.5f,
        -0.5f, -0.5f,
        -0.5f,  0.5f
    };

    unsigned int indices[6] = {
        0, 1, 3,
        1, 2, 3
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

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    const char *vertexShaderSource = readShaderFile("../assets/shaders/basic.vert");
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Error compiling vertex shader" << infoLog << std::endl;
    }

    const char *fragmentShaderSource = readShaderFile("../assets/shaders/basic.frag");
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "Error compiling fragment shader: " << infoLog << std::endl;
    }

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glValidateProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {        
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);  // Add this
    glDeleteBuffers(1, &VBO);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glfwTerminate();
    return 0;
}

bool Engine::getDebugMode() {
    return Engine::DEBUG_MODE;
}