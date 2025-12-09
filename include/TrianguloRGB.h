#pragma once
#include <glad/glad.h>

class TrianguloRGB
{
public:
    TrianguloRGB() = default;

    void Init(GLuint shaderProgram);
    void Draw();
    void Cleanup();

private:
    GLuint shader = 0;
    GLuint VAO = 0, VBO = 0;
};
