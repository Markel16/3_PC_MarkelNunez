#pragma once
#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

class Camara;

class Terreno
{
public:
    // Carga heightmap, genera malla, compila shaders y carga texturas
    bool Init(const std::string& heightmapPath,
        const std::string& grassTexPath,
        const std::string& rockTexPath);

    void Draw(const Camara& camara, float aspectRatio);
    void Cleanup();

    // Para colocar objetos
    float GetHeightWorld(float worldX, float worldZ) const;
    glm::vec3 GetNormalWorld(float worldX, float worldZ) const;

    // Para calcular tamaño del terreno
    int   GetWidthHM()  const { return widthHM; }
    int   GetHeightHM() const { return heightHM; }
    float GetScaleXZ()  const { return scaleXZ; }

private:
    bool loadHeightmap(const std::string& path, std::vector<float>& heights);
    void generateMesh(const std::vector<float>& heights);
    bool createShader();
    bool loadTextures(const std::string& grassPath, const std::string& rockPath);

private:
    int widthHM = 0;
    int heightHM = 0;

    float scaleXZ = 1.0f;
    float scaleY = 30.0f;

    std::vector<float> heightsHM; 

    GLuint VAO = 0, VBO = 0, EBO = 0;
    GLsizei numIndices = 0;

    GLuint texGrass = 0;
    GLuint texRock = 0;

    GLuint shaderProgram = 0;
};

