#include "App.h"
#include <iostream>
#include <fstream>
#include <sstream>


//   Constructor / Destructor


App::App()
{
    init();
}

App::~App()
{
    cleanup();
}


//   Utilidad: cargar shader desde archivo (sin BOM)


std::string App::loadShaderSource(const std::string& path)
{
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

//   Callback teclado 


void App::KeyCallback(GLFWwindow* window,int key, int /*scancode*/,int action, int /*mods*/)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) 
    {
        glfwSetWindowShouldClose(window, true);
    }
}


//   init() – se ejecuta una vez


void App::init()
{
    if (!glfwInit()) {
        std::cerr << "[ERROR] glfwInit\n";
        std::exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 800, "Markel Núñez", nullptr, nullptr);
    if (!window) {
        std::cerr << "[ERROR] creando ventana\n";
        glfwTerminate();
        std::exit(-1);
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[ERROR] gladLoadGLLoader\n";
        std::exit(-1);
    }

    glfwSetKeyCallback(window, App::KeyCallback);

    int maj = 0, min = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &maj);
    glGetIntegerv(GL_MINOR_VERSION, &min);
    std::cout << "[INFO] OpenGL " << maj << "." << min << "\n";

    glViewport(0, 0, 800, 800);

    //ACTIVAR DEPTH TEST PARA QUE EL CUBO SE VEA EN 3D
    glEnable(GL_DEPTH_TEST);

    initShaders();

    cuadradoMix.Init(shaderProgram);
    cuadradoRotado.Init(shaderProgram);
    circulo.Init(shaderProgram);
    trianguloRGB.Init(shaderProgram);

    cubo.Init();   // el cubo rotando 3D

	glEnable(GL_DEPTH_TEST);   // para el cubo 3D
}



//   initShaders() – crea shaderProgram


void App::initShaders()
{
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
    if (!ok) {
        glGetShaderInfoLog(vs, 1024, nullptr, log);
        std::cerr << "[ERROR] Vertex Shader:\n" << log << "\n";
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsrc, nullptr);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        glGetShaderInfoLog(fs, 1024, nullptr, log);
        std::cerr << "[ERROR] Fragment Shader:\n" << log << "\n";
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &ok);
    if (!ok) 
    {
        glGetProgramInfoLog(shaderProgram, 1024, nullptr, log);
        std::cerr << "[ERROR] Link Program:\n" << log << "\n";
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    if (ok) std::cout << "[INFO] Shaders OK.\n";


    glUseProgram(shaderProgram);
    GLint loc1 = glGetUniformLocation(shaderProgram, "uTex1");
    GLint loc2 = glGetUniformLocation(shaderProgram, "uTex2");
    if (loc1 != -1) glUniform1i(loc1, 0);
    if (loc2 != -1) glUniform1i(loc2, 1);
}


//   run() / mainLoop()


void App::run()
{
    mainLoop();
}

void App::mainLoop()
{
    double startTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        double now = glfwGetTime();
        float t = static_cast<float>(now - startTime);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_DEPTH_TEST);   // para el cubo 3D

        // --- figuras 2D con tu shader ---
        glUseProgram(shaderProgram);
        cuadradoMix.Draw();
        cuadradoRotado.Draw(t);
        circulo.Draw();
        trianguloRGB.Draw();

        // --- cubo 3D que gira ---
        cubo.Draw(t);

        glfwSwapBuffers(window);
    }
}



//   cleanup()


void App::cleanup()
{
    // limpiar figuras
    cuadradoMix.Cleanup();
    cuadradoRotado.Cleanup();
    circulo.Cleanup();
    trianguloRGB.Cleanup();

	cubo.Cleanup();

    if (shaderProgram) glDeleteProgram(shaderProgram);
    if (window)        glfwDestroyWindow(window);
    glfwTerminate();
}
