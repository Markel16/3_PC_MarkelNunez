#pragma once

#include <string>
#include <glad/glad.h>

class Camara;

class Agua
{
public:
    
    bool Init(const std::string& waterTexPath);

    // Parámetros
    void SetSize(float s);              
    void SetCenter(float x, float z);   
    void SetWaterY(float y);            

    // Olas simples 
    void SetWave(float amplitude, float speed);

    // Marea 
    void SetTide(bool enabled, float amplitude, float speed);

    // Update y Draw
    void Update(float timeSeconds);
    void Draw(const Camara& camara, float aspect);

    void Cleanup();

private:
    
    std::string loadTextFile(const std::string& path);
    GLuint compileShader(GLenum type, const std::string& src);
    GLuint createProgram(const std::string& vsSrc, const std::string& fsSrc);
    bool createShader();

    
    GLuint loadTexture(const std::string& path);

private:
  
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

    GLuint texWater = 0;
    GLuint shaderProgram = 0;

    // Uniforms
    GLint uViewLoc = -1;
    GLint uProjLoc = -1;
    GLint uModelLoc = -1;
    GLint uTexLoc = -1;

    // Transform parameters
    float centerX = 0.0f;
    float centerZ = 0.0f;

    float sizeX = 100.0f;
    float sizeZ = 25.0f;

    float waterY = 0.0f;
    float baseWaterY = 0.0f;

    // ola 
    float waveAmp = 5.0f;
    float waveSpeed = 2.0f;

    // Tide 
    bool  tideEnabled = false;
    float tideAmplitude = 0.0f;
    float tideSpeed = 3.0f;
    float tideBaseY = 2.0f;
};
