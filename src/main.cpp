#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <cstring>

// we will read the command line and set this accordingly
bool DEBUG_MODE = 0;

// callback function to make the window's darawable size be the size of the actual window
void window_size_callback(GLFWwindow* window, int width, int height) {
    if (DEBUG_MODE) {
        printf("window resized, new size: width %d, height %d\n", width, height);
    }
    glViewport(0,0,width,height);
}

// get command line arguments where argc is the ammount and argv is the strings that are the argument (argv[1] is always the executable)
int main(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++) {
        if (argc < 2) {
            break;
        }
        if (strcmp(argv[i], "-debug") == 0) {
            DEBUG_MODE = true;
        }
    }
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit()) {
        return -1;
    }

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
    if (err != GLEW_OK) {
        fprintf(stderr, "GLEW intilization return error code %d\n", err);
        fprintf(stderr,"GLEW failed to initialize, error: %s\n", glewGetErrorString(err));
    }
    if (DEBUG_MODE) {
        printf("using GLEW version %s\n", glewGetString(GLEW_VERSION));
    }
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

    float positions[6] = {
        -0.5f, -0.5f,
        0.0f, 0.5f,
        0.5f, -0.5f
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6*sizeof(float), positions, GL_STATIC_DRAW);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {        
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}