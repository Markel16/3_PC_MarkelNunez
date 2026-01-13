#include "Camara.h"
#include <algorithm> 


void Camara::Update(GLFWwindow* window, float dt)
{
    if (!window) return;
    if (dt < 0.0f) dt = 0.0f;

   
    float velFrame = velocidad * dt;

  
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) posicion += frente * velFrame;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) posicion -= frente * velFrame;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) posicion -= derecha * velFrame;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) posicion += derecha * velFrame;

 
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) posicion.y -= velFrame;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) posicion.y += velFrame;

    float rotFrame = sensibilidad * dt;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  yaw -= rotFrame;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) yaw += rotFrame;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    pitch += rotFrame;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  pitch -= rotFrame;

    
    pitch = std::clamp(pitch, -89.0f, 89.0f);

   
    updateVectors();
}


void Camara::updateVectors()
{
    glm::vec3 f;
    f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    f.y = sin(glm::radians(pitch));
    f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    frente = glm::normalize(f);

 
    derecha = glm::normalize(glm::cross(frente, glm::vec3(0.0f, 1.0f, 0.0f)));

  
    arriba = glm::normalize(glm::cross(derecha, frente));
}


glm::mat4 Camara::GetViewMatrix() const
{
    return glm::lookAt(posicion, posicion + frente, arriba);
}


glm::mat4 Camara::GetProjectionMatrix(float aspectRatio) const
{
    return glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 500.0f);
}
