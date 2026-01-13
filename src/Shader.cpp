#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Lee archivo de texto completo
static std::string ReadTextFile(const std::string& path)
{
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

// Compila un shader de tipo (VS/FS)
static GLuint Compile(GLenum type, const std::string& src)
{
    GLuint s = glCreateShader(type);
    const char* c = src.c_str();
    glShaderSource(s, 1, &c, nullptr);
    glCompileShader(s);

    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        char log[2048];
        glGetShaderInfoLog(s, 2048, nullptr, log);
        std::cerr << "Shader compile error:\n" << log << "\n";
        glDeleteShader(s);
        return 0;
    }
    return s;
}

// Load: compila y linkea un programa
bool Shader::Load(const std::string& vsPath, const std::string& fsPath)
{
    std::string vs = ReadTextFile(vsPath);
    std::string fs = ReadTextFile(fsPath);

    GLuint v = Compile(GL_VERTEX_SHADER, vs);
    GLuint f = Compile(GL_FRAGMENT_SHADER, fs);
    if (!v || !f) return false;

    id = glCreateProgram();
    glAttachShader(id, v);
    glAttachShader(id, f);
    glLinkProgram(id);

    glDeleteShader(v);
    glDeleteShader(f);

    GLint ok = 0;
    glGetProgramiv(id, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        char log[2048];
        glGetProgramInfoLog(id, 2048, nullptr, log);
        std::cerr << "Program link error:\n" << log << "\n";
        glDeleteProgram(id);
        id = 0;
        return false;
    }
    return true;
}


void Shader::Use() const
{
    glUseProgram(id);
}


void Shader::Destroy()
{
    if (id)
    {
        glDeleteProgram(id);
        id = 0;
    }
}
