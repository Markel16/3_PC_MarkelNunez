#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//Figuras 2D
#include "CuadradoMix.h"
#include "CuadradoRotado.h"
#include "Circulo.h"
#include "TrianguloRGB.h"

//Cubo 3D que rota
#include "CuboRotado.h"

//Entrega PDF
#include "Camara.h"
#include "Terreno.h"
#include "Agua.h"
#include "Model.h"
#include "Shader.h"

class App
{
public:
    App();
    ~App();

    void run();   

private:
    
    GLFWwindow* window = nullptr;
    GLuint shaderProgram = 0;   // shader compartido por las figuras

    GLuint modelShader = 0;
    Model treeModel;
    Model rockModel;
    
    CuadradoMix    cuadradoMix;     // cuadrado mezcla de 2 texturas + color
    CuadradoRotado cuadradoRotado;  // cuadrado con textura rotada
    Circulo        circulo;         // círculo sólido
    TrianguloRGB   trianguloRGB;    // triángulo con 3 colores
    CuboRotado    cubo;            // cubo que rota

	//Llamada de la Entrega PDF
	Camara camara;
	Terreno terreno;
	Agua agua;
    //Roca roca;

    std::vector<glm::vec3> arboles;
    std::vector<glm::vec3> rocas;

    //Para el deltaTime
	double lastTime = 0.0;

    //Ciclo de vida
    void init();        // inicializa GLFW, GLAD, shaders y figuras
    void mainLoop();    // bucle principal (draw de todas las figuras)
    void cleanup();     // libera recursos

    //Callbacks
    // Debe ser static porque se pasa directamente a glfwSetKeyCallback
    static void KeyCallback(GLFWwindow* window,int key,int scancode,int action, int mods);

    //Shaders / utilidades
    void initShaders(); // compila y linka shaderProgram
    std::string loadShaderSource(const std::string& path); // lee el codigo de los .vs/.fs
};

