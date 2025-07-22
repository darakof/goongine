#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

// callback function to make the window's darawable size be the size of the actual window
void window_size_callback(GLFWwindow* window, int width, int height) {
    printf("window resized, new size: width %d, height %d\n", width, height);
    glViewport(0,0,width,height);
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr,"GLEW failed to initialize, error: %a\n", glewGetErrorString(err));
    }
    fprintf(stdout, "using GLEW version %a\n", glewGetString(GLEW_VERSION));

    glfwSetWindowSizeCallback(window, window_size_callback);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {        
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glBegin(GL_TRIANGLES);

        glColor3f(1.0f,0.0f,0.0f);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(0.5f, -0.5f);

        glEnd();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}