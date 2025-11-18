#include "App.h"
#define STB_IMAGE_IMPLEMENTATION 1
#include "stb_image.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <array>

//ESTADO GLOBAL

// Texturas
static GLuint texture1 = 0;
static GLuint texture2 = 0;

// Quad: POS (x,y,z) + UV (u,v)
static std::array<float, 20> baseVerts = {
    //   x,      y,    z,    u,    v
    -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,  // 0: top-left
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,  // 1: bottom-left
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f,  // 2: bottom-right
     0.5f,  0.5f, 0.0f,  1.0f, 1.0f   // 3: top-right
};

App::App() { init(); }
App::~App() { cleanup(); }

//LECTOR DE SHADERS (quita BOM UTF-8)
std::string App::loadShaderSource(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[ERROR] No se pudo abrir el shader: " << path << "\n";
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    // Eliminar BOM UTF-8 si existe
    if (source.size() >= 3 &&
        (unsigned char)source[0] == 0xEF &&
        (unsigned char)source[1] == 0xBB &&
        (unsigned char)source[2] == 0xBF) {
        source.erase(0, 3);
    }

    return source;
}

//CALLBACK TECLADO
void App::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}


void App::init() {
    if (!glfwInit()) { std::cerr << "[ERROR] glfwInit\n"; std::exit(-1); }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 800, "Markel Núñez", nullptr, nullptr);
    if (!window) { std::cerr << "[ERROR] creando ventana\n"; glfwTerminate(); std::exit(-1); }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[ERROR] gladLoadGLLoader\n"; std::exit(-1);
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, [](GLFWwindow* win, int key, int sc, int act, int mods) {
        if (auto* self = static_cast<App*>(glfwGetWindowUserPointer(win))) {
            self->KeyCallback(win, key, sc, act, mods);
        }
        });

    int maj = 0, min = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &maj);
    glGetIntegerv(GL_MINOR_VERSION, &min);
    std::cout << "[INFO] OpenGL " << maj << "." << min << "\n";

    glViewport(0, 0, 800, 800);

    initShaders();
    initTriangle();
    initTexture();
}


void App::initShaders() {
    std::string vertexCode = loadShaderSource("../shaders/basic.vs");
    std::string fragmentCode = loadShaderSource("../shaders/basic.fs");

    if (vertexCode.empty() || fragmentCode.empty()) {
        std::cerr << "[ERROR] Shader vacío. Revisa rutas ../shaders/basic.vs y .fs\n";
    }

    const char* vsrc = vertexCode.c_str();
    const char* fsrc = fragmentCode.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsrc, nullptr);
    glCompileShader(vs);
    GLint ok = GL_FALSE; char log[1024];
    glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
    if (!ok) { glGetShaderInfoLog(vs, 1024, nullptr, log); std::cerr << "[ERROR] Vertex Shader:\n" << log << "\n"; }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsrc, nullptr);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
    if (!ok) { glGetShaderInfoLog(fs, 1024, nullptr, log); std::cerr << "[ERROR] Fragment Shader:\n" << log << "\n"; }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &ok);
    if (!ok) { glGetProgramInfoLog(shaderProgram, 1024, nullptr, log); std::cerr << "[ERROR] Link Program:\n" << log << "\n"; }

    glDeleteShader(vs);
    glDeleteShader(fs);

    if (ok) std::cout << "[INFO] Shaders OK.\n";

    // Asignar samplers a las unidades de textura 0 y 1
    glUseProgram(shaderProgram);
    GLint loc1 = glGetUniformLocation(shaderProgram, "uTex1");
    GLint loc2 = glGetUniformLocation(shaderProgram, "uTex2");
    if (loc1 != -1) glUniform1i(loc1, 0);
    if (loc2 != -1) glUniform1i(loc2, 1);
}

// ▬▬▬ TEXTURAS ▬▬▬
void App::initTexture()
{
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    // TEXTURA 1
    unsigned char* data = stbi_load("../Assets/Texture1.jpg", &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "[ERROR] No se pudo cargar Texture1.jpg\n";
        return;
    }

    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,
        format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    // TEXTURA 2
    data = stbi_load("../Assets/Texture 2.jpg", &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "[ERROR] No se pudo cargar Texture2.jpg\n";
        return;
    }

    format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,
        format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
}


void App::initTriangle() {
    unsigned int indices[] = { 0,1,2,  0,2,3 };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(float) * baseVerts.size(),
        baseVerts.data(),
        GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indices), indices, GL_STATIC_DRAW);

    // POSICIÓN -> location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        5 * sizeof(float),
        (void*)0);
    glEnableVertexAttribArray(0);

    // UV -> location 2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
        5 * sizeof(float),
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void App::run() { mainLoop(); }


void App::mainLoop() {

    const GLint uOffsetLoc = glGetUniformLocation(shaderProgram, "uOffset");
    const GLint uTintColorLoc = glGetUniformLocation(shaderProgram, "uTintColor");
    const GLint uModeLoc = glGetUniformLocation(shaderProgram, "uMode");
    const GLint uAngleLoc = glGetUniformLocation(shaderProgram, "uAngle");

    double startTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        double now = glfwGetTime();
        float t = static_cast<float>(now - startTime);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        //Cuadrado 2 texturas + color ARRIBA IZQUIERDA
        if (uModeLoc != -1) glUniform1i(uModeLoc, 0);
        if (uOffsetLoc != -1) glUniform2f(uOffsetLoc, -0.5f, 0.5f);
        if (uTintColorLoc != -1) glUniform4f(uTintColorLoc, 1, 1, 1, 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Cuadrado textura rotada ARRIBA DERECHA
        if (uModeLoc != -1) glUniform1i(uModeLoc, 1);
        if (uOffsetLoc != -1) glUniform2f(uOffsetLoc, 0.5f, 0.5f);
        if (uAngleLoc != -1) glUniform1f(uAngleLoc, t);
        if (uTintColorLoc != -1) glUniform4f(uTintColorLoc, 1, 1, 1, 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //Círculo azul ABAJO IZQUIERDA
        if (uModeLoc != -1) glUniform1i(uModeLoc, 2);
        if (uOffsetLoc != -1) glUniform2f(uOffsetLoc, -0.5f, -0.5f);
        if (uTintColorLoc != -1) glUniform4f(uTintColorLoc, 0.0f, 0.6f, 1.0f, 1.0f);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
    }
}


void App::cleanup() {
    if (VAO)       glDeleteVertexArrays(1, &VAO);
    if (VBO)       glDeleteBuffers(1, &VBO);
    if (EBO)       glDeleteBuffers(1, &EBO);
    if (texture1)  glDeleteTextures(1, &texture1);
    if (texture2)  glDeleteTextures(1, &texture2);
    if (shaderProgram) glDeleteProgram(shaderProgram);
    if (window)    glfwDestroyWindow(window);
    glfwTerminate();
}
