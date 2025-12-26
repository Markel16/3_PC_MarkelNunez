#pragma once
#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Mesh.h"

class Model
{
public:
    bool Load(const std::string& path);
    void Draw(GLuint shaderProgram, const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj) const;
    void Cleanup();

private:
    std::vector<Mesh> meshes;

    static void SetMat4(GLuint prog, const char* name, const glm::mat4& m);
};
