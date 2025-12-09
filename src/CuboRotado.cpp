//CuboRotado.cpp
#include "CuboRotado.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

// ---- utilidades internas --------------------------------

std::string CuboRotado::loadTextFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[Cubo] No se pudo abrir: " << path << "\n";
        return "";
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

GLuint CuboRotado::compileShader(GLenum type, const std::string& src)
{
    GLuint sh = glCreateShader(type);
    const char* csrc = src.c_str();
    glShaderSource(sh, 1, &csrc, nullptr);
    glCompileShader(sh);
    GLint ok;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(sh, 1024, nullptr, log);
        std::cerr << "[Cubo] Error compilando shader: " << log << "\n";
    }
    return sh;
}

GLuint CuboRotado::createProgram(const std::string& vsSrc, const std::string& fsSrc)
{
    GLuint vs = compileShader(GL_VERTEX_SHADER, vsSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fsSrc);

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    GLint ok;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(prog, 1024, nullptr, log);
        std::cerr << "[Cubo] Error linkando programa: " << log << "\n";
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

// ---- Init ------------------------------------------------

void CuboRotado::Init()
{
    // 1) Crear shader
    std::string vs = loadTextFile("../shaders/cube.vs");
    std::string fs = loadTextFile("../shaders/cube.fs");
    shaderProgram = createProgram(vs, fs);

    uMvpLoc = glGetUniformLocation(shaderProgram, "uMVP");

    // 2) Geometría del cubo (36 vértices, 12 triángulos)
    //    pos (x,y,z) + color (r,g,b)
    float vertices[] = {
        //     posiciones          // colores
		// Cara delantera
        -0.5f, -0.5f,  0.5f,       1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,       0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,       0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,       0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,       1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,       1.0f, 0.0f, 0.0f,

        // Cara atras
        -0.5f, -0.5f, -0.5f,       1.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,       0.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,       0.5f, 0.5f, 1.0f,
         0.5f,  0.5f, -0.5f,       0.5f, 0.5f, 1.0f,
        -0.5f,  0.5f, -0.5f,       1.0f, 0.5f, 0.5f,
        -0.5f, -0.5f, -0.5f,       1.0f, 0.0f, 1.0f,

        // Cara izquierda
        -0.5f,  0.5f,  0.5f,       0.2f, 1.0f, 0.2f,
        -0.5f,  0.5f, -0.5f,       0.2f, 0.2f, 1.0f,
        -0.5f, -0.5f, -0.5f,       1.0f, 0.2f, 0.2f,
        -0.5f, -0.5f, -0.5f,       1.0f, 0.2f, 0.2f,
        -0.5f, -0.5f,  0.5f,       0.2f, 1.0f, 0.2f,
        -0.5f,  0.5f,  0.5f,       0.2f, 1.0f, 0.2f,

        // Cara derecha
         0.5f,  0.5f,  0.5f,       0.8f, 0.4f, 0.1f,
         0.5f,  0.5f, -0.5f,       0.4f, 0.8f, 0.1f,
         0.5f, -0.5f, -0.5f,       0.1f, 0.4f, 0.8f,
         0.5f, -0.5f, -0.5f,       0.1f, 0.4f, 0.8f,
         0.5f, -0.5f,  0.5f,       0.8f, 0.4f, 0.1f,
         0.5f,  0.5f,  0.5f,       0.8f, 0.4f, 0.1f,

         // Cara abajo
         -0.5f, -0.5f, -0.5f,       0.3f, 0.3f, 0.3f,
          0.5f, -0.5f, -0.5f,       0.6f, 0.6f, 0.6f,
          0.5f, -0.5f,  0.5f,       0.9f, 0.9f, 0.9f,
          0.5f, -0.5f,  0.5f,       0.9f, 0.9f, 0.9f,
         -0.5f, -0.5f,  0.5f,       0.3f, 0.3f, 0.3f,
         -0.5f, -0.5f, -0.5f,       0.3f, 0.3f, 0.3f,

         // Cara arriba
         -0.5f,  0.5f, -0.5f,       0.7f, 0.0f, 0.7f,
          0.5f,  0.5f, -0.5f,       0.0f, 0.7f, 0.7f,
          0.5f,  0.5f,  0.5f,       0.7f, 0.7f, 0.0f,
          0.5f,  0.5f,  0.5f,       0.7f, 0.7f, 0.0f,
         -0.5f,  0.5f,  0.5f,       0.7f, 0.0f, 0.7f,
         -0.5f,  0.5f, -0.5f,       0.7f, 0.0f, 0.7f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// posisción
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // colores
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

//para dibujar el cubo rotado

void CuboRotado::Draw(float timeSeconds)
{
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    // Matrices: model (rotación), view (cámara), projection (perspectiva)
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, timeSeconds, glm::vec3(0.5f, 1.0f, 0.0f));

    glm::mat4 view = glm::translate(glm::mat4(1.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)); // cámara atrás

    glm::mat4 proj = glm::perspective(glm::radians(45.0f),
        1.0f, // aspecto 800x800
        0.1f, 100.0f);

    glm::mat4 mvp = proj * view * model;

    glUniformMatrix4fv(uMvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
}

// Limpieza

void CuboRotado::Cleanup()
{
    if (VBO) glDeleteBuffers(1, &VBO);
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (shaderProgram) glDeleteProgram(shaderProgram);
    VBO = VAO = shaderProgram = 0;
}
