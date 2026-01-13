#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Mesh.h"


class Model
{
public:
    
    bool Load(const std::string& path);

    
    bool LoadTexture(const std::string& path);


    bool SetTextureForMaterial(const std::string& materialName,
        const std::string& texturePath,
        bool alphaCutout);


    void Draw(GLuint shaderProgram,
        const glm::mat4& model,
        const glm::mat4& view,
        const glm::mat4& proj) const;

   
    float GetMinY() const { return minY; }
    float GetMaxY() const { return maxY; }

    // Libera VAOs/VBOs y texturas.
    void Cleanup();

private:
    
    static void SetMat4(GLuint prog, const char* name, const glm::mat4& m);

    // Info por material
    struct MatInfo
    {
        GLuint tex = 0;            
        bool alphaCutout = false;  
    };

    
    GLuint LoadTextureGL(const std::string& path);

private:
    float minY = 0.0f; 
    float maxY = 0.0f; 

    GLuint diffuseTex = 0; 

    std::vector<Mesh> meshes; 

   
    std::unordered_map<std::string, MatInfo> matTable;
};
