//TrianguloRGB.h
#pragma once
#include <glad/glad.h>
class TrianguloRGB 
{
    public:
    TrianguloRGB() = default;
    ~TrianguloRGB() = default;
    void run();
 public:
    
    GLuint VBO = 0;
    GLuint EBO = 0;
    GLuint VAO = 0;
    GLuint shaderProgram = 0;
    GLuint triVAO = 0;

    GLuint triVBO = 0;
    void init();
    void mainLoop();
    void cleanup();
    void initCircle();//quad para texturas + círculo
    void initShaders();//carga shaders
    void initTexture();//texturas cuadrado
    void initTrianguloRGB();//triángulo abajo derecha
}
void InitTrianguloRGB(GLuint& triVAO, GLuint& triVBO);
void DrawTrianguloRGB(GLuint shaderProgram,
    GLuint triVAO);
