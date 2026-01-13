#pragma once
#define GLM_ENABLE_EXPERIMENTAL  /

#include <string>   /
#include <vector>   
#include <glad/glad.h>   
#include <GLFW/glfw3.h>  


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>  
#include <glm/gtc/quaternion.hpp>        
#include <glm/gtx/quaternion.hpp>        


#include "CuadradoMix.h"
#include "CuadradoRotado.h"
#include "Circulo.h"
#include "TrianguloRGB.h"


#include "Camara.h"   
#include "Terreno.h" 
#include "Agua.h"     
#include "Model.h"    

class App
{
public:
    App();    
    ~App();    

    void mainLoop(); 

private:
    void init();       
    void cleanup();     
    void initShaders(); 
    std::string loadShaderSource(const std::string& path);

    
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
    GLFWwindow* window = nullptr;

    
    GLuint shaderProgram = 0; 
    GLuint modelShader = 0;  

    
    Camara  camara;
    Terreno terreno;
    Agua    agua;

   
    Model treeModel;
    Model rockModel;

    
    std::vector<glm::vec3> arboles;
    std::vector<glm::vec3> rocas;
};
