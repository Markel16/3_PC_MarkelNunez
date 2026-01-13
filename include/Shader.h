#pragma once
#include <string>
#include <glad/glad.h>

class Shader
{
public:
    GLuint id = 0;

    Shader() = default;
    bool Load(const std::string& vsPath, const std::string& fsPath);
    void Use() const;
    void Destroy();
};
