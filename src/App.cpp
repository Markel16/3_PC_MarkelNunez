#include "App.h"
#define STB_IMAGE_IMPLEMENTATION 1
#include "stb_image.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <array>


static float offsetX = -0.7f;   // posición inicial
static float offsetY = 0.7f;

//cuadrado
static std::array<float, 12> baseVerts = {
    -0.5f,  0.5f, 0.0f,  // 0: top-left
    -0.5f, -0.5f, 0.0f,  // 1: bottom-left
     0.5f, -0.5f, 0.0f,  // 2: bottom-right
     0.5f,  0.5f, 0.0f   // 3: top-right
};

App::App() { init(); }
App::~App() { cleanup(); }

// LECTOR DE SHADERS
std::string App::loadShaderSource(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[ERROR] No se pudo abrir el shader: " << path << "\n";
        return "";
    }
    std::stringstream buffer; buffer << file.rdbuf();
    return buffer.str();
}

// CALLBACK DE TECLADO 
void App::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

//INICIALIZACIÓN GENERAL 
void App::init() {
    if (!glfwInit()) { std::cerr << "[ERROR] glfwInit\n"; std::exit(-1); }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(800, 800, "Markel Núñez", nullptr, nullptr);
    if (!window) { std::cerr << "[ERROR] creando ventana\n"; glfwTerminate(); std::exit(-1); }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[ERROR] gladLoadGLLoader\n"; std::exit(-1);
    }

    // Registrar callback teclado -> método de clase
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
}

//CREACIÓN DE SHADERS 
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
}
void App::initTexture() 
{
	int width, height, nrChannels;

	stbi_set_flip_vertically_on_load (true);

	unsigned char* data = stbi_load("../Assets/Texture1", &width, &height, &nrChannels, 0);
    
	GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB; // Determina el formato basado en los canales

    glGenTextures(1, &textureID); // Genera la texture en GPU
    glBindTexture(GL_TEXTURE_2D, textureID); // Setea la textura como textura activa
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //Seteamos los parametros de la texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
    GL_UNSIGNED_BYTE, data); // Seteamos la data de la textura
    glGenerateMipmap(GL_TEXTURE_2D); //Generamos el mimap de la textura
    glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
    glBindTexture(GL_TEXTURE_2D, textureID); // Set texture to sampler 0
    
}


void App::initTriangle() {
    // Dos triangulos
    unsigned int indices[] = { 0,1,2,  0,2,3 };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * baseVerts.size(), baseVerts.data(), GL_DYNAMIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}


void App::run() { mainLoop(); }

void App::mainLoop() {
    const float halfW = 0.5f; 
    const float halfH = 0.5f;

    // velocidad 
    static float velX = 0.4f;
    static float velY = 0.4f;
    static float Speed = 0.6f;

    const GLint uOffset = glGetUniformLocation(shaderProgram, "uOffset");
    if (uOffset == -1) std::cerr << "[AVISO] uOffset NO encontrado. (Se podría hacer fallback moviendo VBO)\n";

    double last = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // dt
        double now = glfwGetTime();
        float dt = static_cast<float>(now - last);
        last = now;

        // Movimiento automático
        offsetX += velX * Speed * dt;
        offsetY += velY * Speed * dt;

        // Rebotes 
        if (offsetX + halfW >= 1.0f) { offsetX = 1.0f - halfW; velX = -velX; }
        else if (offsetX - halfW <= -1.0f) { offsetX = -1.0f + halfW; velX = -velX; }

        if (offsetY + halfH >= 1.0f) { offsetY = 1.0f - halfH; velY = -velY; }
        else if (offsetY - halfH <= -1.0f) { offsetY = -1.0f + halfH; velY = -velY; }

        // Render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        if (uOffset != -1) {
            glUniform2f(uOffset, offsetX, offsetY);
        }
        else {
           
            float v[12];
            v[0] = baseVerts[0] + offsetX;  v[1] = baseVerts[1] + offsetY;  v[2] = baseVerts[2];
            v[3] = baseVerts[3] + offsetX;  v[4] = baseVerts[4] + offsetY;  v[5] = baseVerts[5];
            v[6] = baseVerts[6] + offsetX;  v[7] = baseVerts[7] + offsetY;  v[8] = baseVerts[8];
            v[9] = baseVerts[9] + offsetX; v[10] = baseVerts[10] + offsetY; v[11] = baseVerts[11];
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(v), v);
        }

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
    }
}


void App::cleanup() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
    if (shaderProgram) glDeleteProgram(shaderProgram);
    if (window) glfwDestroyWindow(window);
    glfwTerminate();
}
