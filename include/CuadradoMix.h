#pragma once
#include <glad/glad.h>

class CuadradoMix
{
public:
    CuadradoMix() = default;

    void Init(GLuint shaderProgram);
    void Draw();
    void Cleanup();

private:
    GLuint shader = 0;
    GLuint VAO = 0, VBO = 0, EBO = 0;
    GLuint tex1 = 0, tex2 = 0;
};
