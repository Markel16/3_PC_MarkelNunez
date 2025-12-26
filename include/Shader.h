#pragma once
#include <string>
#include <glad/glad.h>

class Shader
{
public:
    bool LoadFromFiles(const std::string& vsPath, const std::string& fsPath);
    void Use() const { glUseProgram(id); }
    GLuint ID() const { return id; }
    void Cleanup();

private:
    GLuint id = 0;

    static std::string LoadTextFile(const std::string& path);
    static GLuint Compile(GLenum type, const std::string& src);
};
