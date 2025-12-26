#pragma once

#include <GLFW/glfw3.h>   
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camara
{
public:
    Camara() = default;

    
    void Update(GLFWwindow* window, float dt);

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(float aspectRatio) const;

    glm::vec3 GetPosition() const { return posicion; }

private:
    void updateVectors();

    glm::vec3 posicion{ 0.0f, 5.0f, 15.0f };  
    glm::vec3 frente{ 0.0f, 0.0f, -1.0f };
    glm::vec3 arriba{ 0.0f, 1.0f,  0.0f };
    glm::vec3 derecha{ 1.0f, 0.0f, 0.0f };

    float yaw = -90.0f;
    float pitch = -15.0f;

    float velocidad = 20.0f;  
    float sensibilidad = 60.0f;  
};
