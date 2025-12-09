#pragma once
#include <glad/glad.h>

class Circulo
{
public:
    Circulo() = default;

    void Init(GLuint shaderProgram);
    void Draw();
    void Cleanup();

private:
    GLuint shader = 0;
    GLuint VAO = 0, VBO = 0, EBO = 0;
};
