#include "Agua.h"
#include "Camara.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>


#include "stb_image.h"

//  utilidades shader 
std::string Agua::loadTextFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[Agua] No se pudo abrir: " << path << "\n";
        return "";
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

GLuint Agua::compileShader(GLenum type, const std::string& src)
{
    GLuint sh = glCreateShader(type);
    const char* csrc = src.c_str();
    glShaderSource(sh, 1, &csrc, nullptr);
    glCompileShader(sh);

    GLint ok = 0;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(sh, 1024, nullptr, log);
        std::cerr << "[Agua] Error compilando shader: " << log << "\n";
    }
    return sh;
}

GLuint Agua::createProgram(const std::string& vsSrc, const std::string& fsSrc)
{
    GLuint vs = compileShader(GL_VERTEX_SHADER, vsSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fsSrc);

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    GLint ok = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(prog, 1024, nullptr, log);
        std::cerr << "[Agua] Error linkando programa: " << log << "\n";
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

bool Agua::createShader()
{
    // las rutas
    std::string vs = loadTextFile("../shaders/water.vs");
    std::string fs = loadTextFile("../shaders/water.fs");
    if (vs.empty() || fs.empty()) {
        std::cerr << "[Agua] Shader vacío, revisa rutas water.vs/fs\n";
        return false;
    }

    shaderProgram = createProgram(vs, fs);

    uViewLoc = glGetUniformLocation(shaderProgram, "uView");
    uProjLoc = glGetUniformLocation(shaderProgram, "uProj");
    uModelLoc = glGetUniformLocation(shaderProgram, "uModel");
    uTexLoc = glGetUniformLocation(shaderProgram, "uWaterTex");

    return true;
}

//textura 
GLuint Agua::loadTexture(const std::string& path)
{
    int w, h, n;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &n, 0);
    if (!data) {
        std::cerr << "[Agua] No se pudo cargar textura: " << path << "\n";
        return 0;
    }

    GLenum format = (n == 4) ? GL_RGBA : GL_RGB;

    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return tex;
}

// API 
bool Agua::Init(const std::string& waterTexPath)
{
    // 1) Shader
    if (!createShader())
        return false;

    // 2) Textura
    texWater = loadTexture(waterTexPath);
    if (!texWater)
        return false;

    // 3) Malla del plano 
    //   las posiciones  Pos (x,y,z) + UV (u,v)
    float half = size * 0.5f;

    float vertices[] = {
        -half, waterY, -half,   0.0f, 0.0f,
         half, waterY, -half,   1.0f, 0.0f,
         half, waterY,  half,   1.0f, 1.0f,
        -half, waterY,  half,   0.0f, 1.0f
    };

    unsigned int indices[] = { 0, 1, 2,  2, 3, 0 };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // layout location=0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // layout location=1  -> vec2 uv
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    return true;
}

void Agua::Draw(const Camara& camara, float aspect)
{
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    glm::mat4 view = camara.GetViewMatrix();
    glm::mat4 proj = camara.GetProjectionMatrix(aspect);

    glm::mat4 model = glm::mat4(1.0f);

    // mover el agua al centro del terreno
    model = glm::translate(model, glm::vec3(centerX, 0.0f, centerZ));


    glUniformMatrix4fv(uViewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(uProjLoc, 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(uModelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texWater);
    glUniform1i(uTexLoc, 0);

    // para que el agua tape cosas que no quiero ver
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

void Agua::Cleanup()
{
    if (texWater) glDeleteTextures(1, &texWater);
    if (EBO) glDeleteBuffers(1, &EBO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (shaderProgram) glDeleteProgram(shaderProgram);

    texWater = 0;
    EBO = VBO = VAO = 0;
    shaderProgram = 0;
}
