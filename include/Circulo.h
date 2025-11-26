//Circulo.h
#pragma once
#include <glad/glad.h>
class Circulo 
{
    public:
    Circulo() = default;
	~Circulo() = default;
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
    void initCircle();//circulo abajo derecha
}
void DrawCirculo(GLuint shaderProgram,
    GLuint quadVAO);
