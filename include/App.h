#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class App {
public:
    App();
    ~App();

    void run();

private:
    //Estado general
    GLFWwindow* window = nullptr;

    GLuint VBO = 0;
    GLuint EBO = 0;
    GLuint VAO = 0;
    GLuint shaderProgram = 0;

    GLuint triVAO = 0;
    GLuint triVBO = 0;

    //Ciclo de vida
    void init();
    void mainLoop();
    void cleanup();

    // Callbacks
    void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    //Inicializaciones
    void initTriangle();//quad para texturas + círculo
    void initShaders();//carga shaders
    void initTexture();//texturas cuadrado
    void initTrianguloRGB();//triángulo abajo derecha

    //Funciones de dibujado
    void DrawCuadradoMix();
    void DrawCuadradoRotado(float angle);
    void DrawCirculo();
    void DrawTrianguloRGB();

    //Utilidad
    std::string loadShaderSource(const std::string& path);
};
