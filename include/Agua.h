#pragma once
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Camara; 

class Agua
{
public:

    bool Init(const std::string& waterTexPath);
    void Draw(const Camara& camara, float aspect);
    void Cleanup();

    void SetHeight(float y) { waterY = y; }
    void SetSize(float s) { size = s; }
    void SetCenter(float x, float z) { centerX = x; centerZ = z; }
    void SetWaterY(float y) { waterY = y; } 

private:

    float centerX = 0.0f;
    float centerZ = 0.0f;
    GLuint VAO = 0, VBO = 0, EBO = 0;
    GLuint shaderProgram = 0;
    GLuint texWater = 0;

    GLint uViewLoc = -1;
    GLint uProjLoc = -1;
    GLint uModelLoc = -1;
    GLint uTexLoc = -1;

    float waterY = 0.0f;
    float size = 400.0f; // tamaño del plano

private:
    bool createShader();
    GLuint loadTexture(const std::string& path);
    std::string loadTextFile(const std::string& path);
    GLuint compileShader(GLenum type, const std::string& src);
    GLuint createProgram(const std::string& vsSrc, const std::string& fsSrc);
};
