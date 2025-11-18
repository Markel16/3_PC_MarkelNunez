#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class App {
public:
    App();
    ~App();

    void run();

private:
    GLFWwindow* window;
    unsigned int textureID;
    unsigned int VBO, EBO, VAO, shaderProgram;
	int currentState = 0;
    void init();
    void mainLoop();
    void cleanup();

	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void initTriangle();
    void initShaders();
	void initTexture();
	void InitColorTriangle();
    std::string loadShaderSource(const std::string& path);
};
