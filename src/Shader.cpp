#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::string Shader::LoadTextFile(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[Shader] No se pudo abrir: " << path << "\n";
        return "";
    }
    std::stringstream ss;
    ss << file.rdbuf();
    std::string s = ss.str();

    // quitar BOM UTF-8
    if (s.size() >= 3 &&
        (unsigned char)s[0] == 0xEF &&
        (unsigned char)s[1] == 0xBB &&
        (unsigned char)s[2] == 0xBF) {
        s.erase(0, 3);
    }
    return s;
}

GLuint Shader::Compile(GLenum type, const std::string& src)
{
    GLuint sh = glCreateShader(type);
    const char* c = src.c_str();
    glShaderSource(sh, 1, &c, nullptr);
    glCompileShader(sh);

    GLint ok = 0;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(sh, 1024, nullptr, log);
        std::cerr << "[Shader] Error compilando: " << log << "\n";
    }
    return sh;
}

bool Shader::LoadFromFiles(const std::string& vsPath, const std::string& fsPath)
{
    std::string vsSrc = LoadTextFile(vsPath);
    std::string fsSrc = LoadTextFile(fsPath);
    if (vsSrc.empty() || fsSrc.empty()) return false;

    GLuint vs = Compile(GL_VERTEX_SHADER, vsSrc);
    GLuint fs = Compile(GL_FRAGMENT_SHADER, fsSrc);

    id = glCreateProgram();
    glAttachShader(id, vs);
    glAttachShader(id, fs);
    glLinkProgram(id);

    GLint ok = 0;
    glGetProgramiv(id, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(id, 1024, nullptr, log);
        std::cerr << "[Shader] Error linkando: " << log << "\n";
        glDeleteShader(vs);
        glDeleteShader(fs);
        return false;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return true;
}

void Shader::Cleanup()
{
    if (id) glDeleteProgram(id);
    id = 0;
}
