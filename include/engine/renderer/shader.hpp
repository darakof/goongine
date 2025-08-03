#pragma once

#include <string>

namespace Engine {
namespace Renderer {
    
    class ShaderProgram {
    public:
        unsigned int ID;
        ShaderProgram(const char* vertexPath, const char* fragmentPath);
        void Use();
        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;

    private:
        char* readShaderFile(const char* fileName);
    };

} // namespace Renderer
} // namespace Engine