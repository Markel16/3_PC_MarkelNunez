#pragma once
#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

class Camara;

class Terreno
{
public:
    bool Init(const std::string& heightmapPath,
        const std::string& grassTexPath,
        const std::string& rockTexPath);

    void Draw(const Camara& camara, float aspectRatio);
    void Cleanup();

    // Para colocar modelos sobre el terreno
    float GetHeightWorld(float worldX, float worldZ) const;

    // Para que App.cpp pueda calcular el tamaño del terreno
    int   GetWidthHM()  const { return widthHM; }
    int   GetHeightHM() const { return heightHM; }
    float GetScaleXZ()  const { return scaleXZ; }

private:
    bool loadHeightmap(const std::string& path, std::vector<float>& heights);
    bool createShader();
    bool loadTextures(const std::string& grassPath, const std::string& rockPath);
    void generateMesh(const std::vector<float>& heights);

private:
    // OpenGL
    GLuint VAO = 0, VBO = 0, EBO = 0;
    GLsizei numIndices = 0;
    GLuint shaderProgram = 0;
    GLuint texGrass = 0, texRock = 0;

    // Heightmap
    int widthHM = 0;
    int heightHM = 0;
    std::vector<float> heightsHM;

    // Escalas 
    float scaleXZ = 0.6f;  // tamaño en XZ
    float scaleY = 50.0f; // altura máxima
};
