#pragma once
#include <vector>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>


struct Vertex
{
    glm::vec3 pos{};    
    glm::vec3 normal{};  
    glm::vec2 uv{};     
};


class Mesh
{
public:
   
    void Build(const std::vector<Vertex>& vertices, const std::vector<unsigned>& indices);

    
    void Draw() const;

    
    void Cleanup();

    void SetHasTex(bool v) { hasTex = v; }
    bool HasTex() const { return hasTex; }

    
    void SetMaterialName(const std::string& n) { materialName = n; }
    const std::string& GetMaterialName() const { return materialName; }

private:
    GLuint vao = 0, vbo = 0, ebo = 0; 
    GLsizei indexCount = 0;           

    bool hasTex = false;             
    std::string materialName = "default"; 
};
