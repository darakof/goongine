#pragma once

#include "GL/glew.h"
#include "GLFW/glfw3.h"

namespace Engine {
    void window_size_callback(GLFWwindow* window, int width, int height);
    int engine_main(int argc, char* argv[]);
    bool getDebugMode();
};