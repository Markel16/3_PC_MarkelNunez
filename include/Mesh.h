#pragma once
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
};

class Mesh
{
public:
    void Build(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    void Draw() const;
    void Cleanup();

    void SetHasTex(bool v) { hasTex = v; }
    bool HasTex() const { return hasTex; }

private:
    GLuint VAO = 0, VBO = 0, EBO = 0;
    GLsizei indexCount = 0;
    bool hasTex = false;
};
