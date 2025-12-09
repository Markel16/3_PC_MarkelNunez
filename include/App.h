#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Figuras 2D
#include "CuadradoMix.h"
#include "CuadradoRotado.h"
#include "Circulo.h"
#include "TrianguloRGB.h"

// Cubo 3D que rota
#include "CuboRotado.h"

class App
{
public:
    App();
    ~App();

    void run();    // arranca la app (llama a mainLoop)

private:
    // --- Estado general de la aplicación ---
    GLFWwindow* window = nullptr;
    GLuint shaderProgram = 0;   // shader compartido por las figuras

    // --- Figuras en pantalla ---
    CuadradoMix    cuadradoMix;     // cuadrado mezcla de 2 texturas + color
    CuadradoRotado cuadradoRotado;  // cuadrado con textura rotada
    Circulo        circulo;         // círculo sólido
    TrianguloRGB   trianguloRGB;    // triángulo con 3 colores
    CuboRotado    cubo;            // cubo que rota

    // --- Ciclo de vida ---
    void init();        // inicializa GLFW, GLAD, shaders y figuras
    void mainLoop();    // bucle principal (draw de todas las figuras)
    void cleanup();     // libera recursos

    // --- Callbacks ---
    // Debe ser static porque se pasa directamente a glfwSetKeyCallback
    static void KeyCallback(GLFWwindow* window,
        int key,
        int scancode,
        int action,
        int mods);

    // --- Shaders / utilidades ---
    void initShaders();                            // compila y linka shaderProgram
    std::string loadShaderSource(const std::string& path); // lee el código de los .vs/.fs
};
