// CuboRotado.h
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
class CuboRotado
{
public:
    CuboRotado() = default;
    ~CuboRotado() = default;

    void Init();            // SIN parámetros
    void Draw(float time);  // time en segundos
    void Cleanup();
    std::string loadTextFile(const std::string& path);
    GLuint compileShader(GLenum type, const std::string& src);
    GLuint createProgram(const std::string& vsSrc, const std::string& fsSrc);

private:
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint shaderProgram = 0;
    GLint  uMvpLoc = -1;
};
