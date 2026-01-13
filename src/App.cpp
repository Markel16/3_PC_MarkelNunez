#include "App.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <vector>   // std::vector (treePositions)

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


static glm::mat4 MakeAlignToNormal(const glm::vec3& normal)
{
    
    glm::vec3 up = glm::normalize(normal);


    glm::vec3 forward(0, 0, 1);
    if (fabs(glm::dot(up, forward)) > 0.95f)
        forward = glm::vec3(1, 0, 0);

    
    forward = glm::normalize(forward - up * glm::dot(forward, up));

    
    glm::vec3 right = glm::normalize(glm::cross(forward, up));

    
    forward = glm::normalize(glm::cross(up, right));

    
    glm::mat4 R(1.0f);
    R[0] = glm::vec4(right, 0.0f);
    R[1] = glm::vec4(up, 0.0f);
    R[2] = glm::vec4(forward, 0.0f);

    return R;
}


App::App()
{
    
    init();
}

App::~App()
{
    
    cleanup();
}


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


void App::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)window; (void)key; (void)scancode; (void)action; (void)mods;
}


static std::string LoadTextFile_NoBOM(const char* path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[ERROR] No se pudo abrir: " << path << "\n";
        return "";
    }

    std::stringstream ss;
    ss << file.rdbuf();
    std::string source = ss.str();

    // quitar BOM UTF-8
    if (source.size() >= 3 &&
        (unsigned char)source[0] == 0xEF &&
        (unsigned char)source[1] == 0xBB &&
        (unsigned char)source[2] == 0xBF)
    {
        source.erase(0, 3);
    }
    return source;
}


static GLuint BuildProgramFromFiles(const char* vsPath, const char* fsPath)
{
    std::string vsCode = LoadTextFile_NoBOM(vsPath);
    std::string fsCode = LoadTextFile_NoBOM(fsPath);

    if (vsCode.empty() || fsCode.empty()) return 0;

    const char* vsrc = vsCode.c_str();
    const char* fsrc = fsCode.c_str();


    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsrc, nullptr);
    glCompileShader(vs);

    GLint ok = GL_FALSE;
    char log[1024];

    glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        glGetShaderInfoLog(vs, 1024, nullptr, log);
        std::cerr << "[ERROR] Vertex Shader (" << vsPath << "):\n" << log << "\n";
        glDeleteShader(vs);
        return 0;
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsrc, nullptr);
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        glGetShaderInfoLog(fs, 1024, nullptr, log);
        std::cerr << "[ERROR] Fragment Shader (" << fsPath << "):\n" << log << "\n";
        glDeleteShader(vs);
        glDeleteShader(fs);
        return 0;
    }

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        glGetProgramInfoLog(prog, 1024, nullptr, log);
        std::cerr << "[ERROR] Link Program:\n" << log << "\n";
        glDeleteProgram(prog);
        prog = 0;
    }


    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

static std::vector<glm::vec3> treePositions = {
  { 2,0, 3 }, { -4,0, 1 }, { 6,0,-2 }, { 1,0,-6 }
};


void App::initShaders()
{
    shaderProgram = BuildProgramFromFiles("../shaders/basic.vs", "../shaders/basic.fs");
    if (shaderProgram) std::cout << "[INFO] Shaders OK.\n";

    modelShader = BuildProgramFromFiles("../shaders/model.vs", "../shaders/model.fs");
    if (!modelShader) std::cerr << "[WARN] modelShader = 0 (no se dibujarán OBJ)\n";
}


void App::init()
{
    // ---------- GLFW ----------
    if (!glfwInit()) {
        std::cerr << "[ERROR] glfwInit\n";
        std::exit(-1);
    }

    // Pedimos OpenGL 3.3 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Crear ventana
    window = glfwCreateWindow(1920, 1080, "Markel Núñez", nullptr, nullptr);
    if (!window) {
        std::cerr << "[ERROR] creando ventana\n";
        glfwTerminate();
        std::exit(-1);
    }

    // Contexto actual
    glfwMakeContextCurrent(window);

    //GLAD 
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[ERROR] gladLoadGLLoader\n";
        std::exit(-1);
    }

    // Callbacks / user pointer
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, App::KeyCallback);

    // Viewport inicial
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);


    glEnable(GL_DEPTH_TEST);

    //Shaders 
    initShaders();

    //Terreno 

    if (!terreno.Init("../Assets/heightmap.jpeg", "../Assets/cesped.jpg", "../Assets/roca.jpg"))
        std::cerr << "[ERROR] Terreno.Init\n";

 // Agua

    float terrenoW = (terreno.GetWidthHM() - 1) * terreno.GetScaleXZ();
    float terrenoH = (terreno.GetHeightHM() - 1) * terreno.GetScaleXZ();

    // Centro del terreno
    float cx = terrenoW * 0.5f;
    float cz = terrenoH * 0.5f;

    // Agua más grande que el terreno
    float waterSize = std::max(terrenoW, terrenoH) * 2.0f;

    // Parámetros del agua
    agua.SetSize(waterSize);
    agua.SetCenter(cx, cz);
    agua.SetWaterY(4.0f); //altura base del agua 


    if (!agua.Init("../Assets/agua.jpg"))
        std::cerr << "[ERROR] Agua.Init\n";

    // Marea
    agua.SetTide(true, 0.15f, 1.2f); // amplitud, velocidad


    bool okTree = treeModel.Load("../Assets/Tree/Tree.obj");
    bool okRock = rockModel.Load("../Assets/rock.obj");


    treeModel.SetTextureForMaterial("Trank_bark", "../Assets/Tree/bark_0021.jpg", false);     // tronco
    treeModel.SetTextureForMaterial("polySurface1SG1", "../Assets/Tree/DB2X2_L01.png", true); // hojas

    // Roca:
    rockModel.SetTextureForMaterial("default", "../Assets/roca.jpg", false);

    std::cout << "arbol cargado: " << okTree << "\n";
    std::cout << "roca cargada: " << okRock << "\n";

    // Spawn de objetos
    // Límites del terreno en X/Z (mundo)
    float maxX = (terreno.GetWidthHM() - 1) * terreno.GetScaleXZ();
    float maxZ = (terreno.GetHeightHM() - 1) * terreno.GetScaleXZ();

    // Random reproducible (misma isla cada ejecución)
    auto Rand01 = []() { return (float)rand() / (float)RAND_MAX; };
    srand(1234);

    arboles.clear();
    rocas.clear();

    const int NUM_ARBOLES = 30;
    const int NUM_ROCAS = 25;

    // Árboles: se colocan dentro de un radio (isla) para que no salgan en los bordes
    for (int i = 0; i < NUM_ARBOLES; ++i)
    {
        float x = Rand01() * maxX;
        float z = Rand01() * maxZ;

        // Distancia al centro normalizada (0..1) para crear “zona isla”
        float nx = x / maxX;
        float nz = z / maxZ;
        float dx = nx - 0.5f;
        float dz = nz - 0.5f;
        float dist = sqrtf(dx * dx + dz * dz);


        if (dist > 0.45f) { --i; continue; }

        // Altura real del terreno en ese punto
        float y = terreno.GetHeightWorld(x, z);
        arboles.push_back(glm::vec3(x, y, z));
    }

    // Rocas
    for (int i = 0; i < NUM_ROCAS; ++i)
    {
        float x = Rand01() * maxX;
        float z = Rand01() * maxZ;

        float nx = x / maxX;
        float nz = z / maxZ;
        float dx = nx - 0.5f;
        float dz = nz - 0.5f;
        float dist = sqrtf(dx * dx + dz * dz);

        if (dist > 0.48f) { --i; continue; }

        float y = terreno.GetHeightWorld(x, z);
        rocas.push_back(glm::vec3(x, y, z));
    }
}


void App::mainLoop()
{
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        // Delta time (para movimiento cámara constante)
        double now = glfwGetTime();
        float dt = (float)(now - lastTime);
        lastTime = now;

        glfwPollEvents();

        int w = 0, h = 0;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        float aspect = (h > 0) ? (float)w / (float)h : 1.0f;

        // Actualizar cámara 
        camara.Update(window, dt);


        glEnable(GL_DEPTH_TEST);
        glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Matrices cámara
        glm::mat4 view = camara.GetViewMatrix();
        glm::mat4 proj = camara.GetProjectionMatrix(aspect);


        terreno.Draw(camara, aspect);

        agua.Update((float)glfwGetTime());
        agua.Draw(camara, aspect);

        // 3) Modelos (árboles/rocas)
        if (modelShader != 0)
        {
   
            glDisable(GL_CULL_FACE);


            // ÁRBOLES

            float treeScale = 6.0f;

            for (const auto& p : arboles)
            {
                // y del terreno y normal para inclinación
                float y = terreno.GetHeightWorld(p.x, p.z);
                glm::vec3 n = terreno.GetNormalWorld(p.x, p.z);

                // T = mover a posición
                glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, y, p.z));

                // R = alinear con normal
                glm::mat4 R = MakeAlignToNormal(n);



                float baseOffset = -treeModel.GetMinY();
                glm::mat4 Lift = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, baseOffset, 0.0f));

                // S = escala del árbol
                glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(treeScale));


                glm::mat4 M = T * R * S * Lift;

                treeModel.Draw(modelShader, M, view, proj);
            }


            // ROCAS

            float rockScale = 4.0f;

            for (const auto& p : rocas)
            {
   
                //  Esto evita que floten o se hundan.
                float y = terreno.GetHeightWorld(p.x, p.z);


                //La normal apunta hacia arriba según la pendiente.
                glm::vec3 n = terreno.GetNormalWorld(p.x, p.z);


                // Mueve la roca a su posición final: 
                glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, y, p.z));


                //Construimos una matriz de rotación 
                //siga la normal del terreno
                glm::mat4 R = MakeAlignToNormal(n);



                //Si el modelo tiene minY negativo, significa por debajo del 0
                //Al subirlo (-minY), hacem que el punto más bajo toque el suelo local
                float baseOffset = -rockModel.GetMinY();
                glm::mat4 Lift = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, baseOffset, 0.0f));

                //S = SCALE tamaño final
                glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(rockScale));

                //MATRIZ FINAL
                //primero colocas (T) luego inclinas (R)
                //luego escalas (S) y por ultimo ajustas base Lift en espacio local.
                glm::mat4 M = T * R * S * Lift;

                // 8) Dibujar la roca
                rockModel.Draw(modelShader, M, view, proj);
            }

            // Si quieres volver a culling para otras cosas:
            glEnable(GL_CULL_FACE);
        }

        // Presentar frame
        glfwSwapBuffers(window);
    }
}

void App::cleanup()
{
    terreno.Cleanup();
    agua.Cleanup();

    treeModel.Cleanup();
    rockModel.Cleanup();

    if (modelShader)   glDeleteProgram(modelShader);
    if (shaderProgram) glDeleteProgram(shaderProgram);

    if (window) glfwDestroyWindow(window);
    glfwTerminate();
}
