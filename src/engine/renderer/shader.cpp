#ifdef VSCODE
#   include "include/engine/renderer/shader.hpp"
#else
#   include "engine/renderer/shader.hpp"
#endif

#include "GL/glew.h"

#include <stdio.h>

namespace Engine {
namespace Renderer {

    ShaderProgram::ShaderProgram(const char* vertexPath, const char* fragmentPath) {
        const char* vShaderSource = readShaderFile(vertexPath);
        const char* fShaderSource = readShaderFile(fragmentPath);

        unsigned int vertex, fragment;
        int success;
        char infoLog[512];

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderSource, NULL);
        glCompileShader(vertex);

        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            fprintf(stderr, "Error compiling vertex shader: %s\n", infoLog); 
        }

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderSource, NULL);
        glCompileShader(fragment);

        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            fprintf(stderr, "Error compiling fragment shader: %s\n", infoLog); 
        }

        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);

        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            fprintf(stderr, "Error linking program: %s\n", infoLog);
        }

        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void ShaderProgram::Use() {
        glUseProgram(ID);
    }

    void ShaderProgram::setBool(const std::string &name, bool value) const {         
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
    }

    void ShaderProgram::setInt(const std::string &name, int value) const { 
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
    }

    void ShaderProgram::setFloat(const std::string &name, float value) const { 
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    }


    char* ShaderProgram::readShaderFile(const char* fileName) {
        FILE* file = fopen(fileName, "rb");
        if (!file) {
            perror("Failed to open shader file");
            return NULL;
        }

        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        fseek(file, 0, SEEK_SET);

        char* buffer = (char*)malloc(length + 1);
        if (!buffer) {
            fclose(file);
            fprintf(stderr, "Memory allocation failed\n");
            return NULL;
        }

        size_t bytes_read = fread(buffer, 1, length, file);
        if (bytes_read != (size_t)length) {
            fclose(file);
            free(buffer);
            fprintf(stderr, "Read error: expected %ld bytes, got %zu\n", length, bytes_read);
            return NULL;
        }

        buffer[length] = '\0';
        fclose(file);
        return buffer;
    }

} // namespace Renderer
} // namespace Engine