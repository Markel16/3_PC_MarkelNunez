#include "App.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "Camara.h"
#include <Terreno.h>
#include <Agua.h>


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


void App::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}





void App::init()
{
    // --- GLFW ---
    if (!glfwInit()) {
        std::cerr << "[ERROR] glfwInit\n";
        std::exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1920, 1080, "Markel Núñez", nullptr, nullptr);
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

    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, App::KeyCallback);

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    glViewport(0, 0, w, h);

    glEnable(GL_DEPTH_TEST);

    int maj = 0, min = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &maj);
    glGetIntegerv(GL_MINOR_VERSION, &min);
    std::cout << "[INFO] OpenGL " << maj << "." << min << "\n";

    // Shaders base 
    initShaders();

    cuadradoMix.Init(shaderProgram);
    cuadradoRotado.Init(shaderProgram);
    circulo.Init(shaderProgram);
    trianguloRGB.Init(shaderProgram);
    cubo.Init();

    agua.SetWaterY(15.0f);          // la altura base que te gustaba
    agua.Init("../Assets/agua.jpg"); // o si ya lo tienes inicializado, ok
    agua.SetTide(true, 0.02f, 2.0f);  // activa (usa la waterY actual como base)


    //      TERRENO

    if (!terreno.Init(
        "../Assets/heightmap.jpeg",
        "../Assets/cesped.jpg",
        "../Assets/roca.jpg"))
    {
        std::cerr << "[ERROR] Terreno.Init\n";
    }


//        AGUA

    agua.SetTide(true, 0.4f, 0.6f); // amplitud 0.4, velocidad 0.6

// Ttamaño del la isla
    float terrenoW = (terreno.GetWidthHM() - 1) * terreno.GetScaleXZ();
    float terrenoH = (terreno.GetHeightHM() - 1) * terreno.GetScaleXZ();

    // Centro real del terreno
    float cx = terrenoW * 0.5f;
    float cz = terrenoH * 0.5f;

    // para la altura del agua
    float waterSize = std::max(terrenoW, terrenoH) * 2.0f;

    agua.SetSize(waterSize);
    agua.SetCenter(cx, cz);

    // Altura del agua
    agua.SetWaterY(0.2f);

    agua.Init("../Assets/agua.jpg");


    
    //     MODELOS OBJ
   
    bool okTree = treeModel.Load("../Assets/tree.obj");
    bool okRock = rockModel.Load("../Assets/rock.obj");

    if (!okTree) std::cerr << "[WARN] No se pudo cargar tree.obj\n";
    if (!okRock) std::cerr << "[WARN] No se pudo cargar rock.obj\n";

    
    // POSICIONAR OBJETOS
   
    float maxX = (terreno.GetWidthHM() - 1) * terreno.GetScaleXZ();
    float maxZ = (terreno.GetHeightHM() - 1) * terreno.GetScaleXZ();

    auto Rand01 = []() {
        return (float)rand() / (float)RAND_MAX;
        };

    srand(1234);

    arboles.clear();
    rocas.clear();

    //Árboles
    for (int i = 50; i < 60; i++)
    {
        float x = Rand01() * maxX;
        float z = Rand01() * maxZ;

        float nx = x / maxX;
        float nz = z / maxZ;
        float dx = nx - 0.5f;
        float dz = nz - 0.5f;
        float dist = sqrtf(dx * dx + dz * dz);

        if (dist > 0.45f) { i--; continue; }

        float y = terreno.GetHeightWorld(x, z);
        arboles.push_back(glm::vec3(x, y, z));
    }

    //Rocas 
    for (int i = 50; i < 60; i++)
    {
        float x = Rand01() * maxX;
        float z = Rand01() * maxZ;

        float nx = x / maxX;
        float nz = z / maxZ;
        float dx = nx - 0.5f;
        float dz = nz - 0.5f;
        float dist = sqrtf(dx * dx + dz * dz);

        if (dist > 0.48f) { i--; continue; }

        float y = terreno.GetHeightWorld(x, z);
        rocas.push_back(glm::vec3(x, y, z));
    }
}


void App::initShaders()
{
    std::string vertexCode = loadShaderSource("../shaders/basic.vs");
    std::string fragmentCode = loadShaderSource("../shaders/basic.fs");

    if (vertexCode.empty() || fragmentCode.empty()) {
        std::cerr << "[ERROR] Shader vacío. Revisa rutas ../shaders/basic.vs y basic.fs\n";
        return;
    }

    const char* vsrc = vertexCode.c_str();
    const char* fsrc = fragmentCode.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsrc, nullptr);
    glCompileShader(vs);

    GLint ok = GL_FALSE;
    char log[1024];

    glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        glGetShaderInfoLog(vs, 1024, nullptr, log);
        std::cerr << "[ERROR] Vertex Shader:\n" << log << "\n";
        glDeleteShader(vs);
        return;
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsrc, nullptr);
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        glGetShaderInfoLog(fs, 1024, nullptr, log);
        std::cerr << "[ERROR] Fragment Shader:\n" << log << "\n";
        glDeleteShader(vs);
        glDeleteShader(fs);
        return;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &ok);
    if (!ok) {
        glGetProgramInfoLog(shaderProgram, 1024, nullptr, log);
        std::cerr << "[ERROR] Link Program:\n" << log << "\n";
    }
    else {
        std::cout << "[INFO] Shaders OK.\n";
    }

    bool ok1 = treeModel.Load("../Assets/tree.obj");
    bool ok2 = rockModel.Load("../Assets/rock.obj"); 

    std::cout << "arbol cargado: " << ok1 << "\n";
    std::cout << "roca cargada: " << ok2 << "\n";


    glDeleteShader(vs);
    glDeleteShader(fs);
}


void App::run()
{
    mainLoop();
}
void App::mainLoop()
{
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        double now = glfwGetTime();
        float dt = (float)(now - lastTime);
        lastTime = now;

        glfwPollEvents();

        int w = 0, h = 0;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        float aspect = (h > 0) ? (float)w / (float)h : 1.0f;

        camara.Update(window, dt);

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.08f, 0.08f, 0.10f, 1.0f); // <- NO rosa
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //para que sube y baje el agua
        agua.Update((float)glfwGetTime());
    
        agua.Draw(camara, aspect);

        // 1) Terreno + Agua
        terreno.Draw(camara, aspect);
        



        // 2) Modelos del obj
        if (modelShader != 0)
        {
            glm::mat4 view = camara.GetViewMatrix();
            glm::mat4 proj = camara.GetProjectionMatrix(aspect);
            glm::mat4 M(1.0f);
            M = glm::translate(M, glm::vec3(10.0f, 10.0f, 10.0f)); 
            M = glm::scale(M, glm::vec3(300.0f));                   
            treeModel.Draw(modelShader, M, view, proj);

            for (const auto& p : arboles)
            {
                glm::mat4 M(800.0f);
                M = glm::translate(M, p);
                M = glm::scale(M, glm::vec3(1.0f));
                treeModel.Draw(modelShader, M, view, proj);
            }

            for (const auto& p : rocas)
            {
                glm::mat4 M(800.0f);
                M = glm::translate(M, p);
                M = glm::scale(M, glm::vec3(1.0f));
                rockModel.Draw(modelShader, M, view, proj);
            }
        }

        glfwSwapBuffers(window);
    }
}

void App::cleanup()
{
    
    cuadradoMix.Cleanup();
    cuadradoRotado.Cleanup();
    circulo.Cleanup();
    trianguloRGB.Cleanup();

	cubo.Cleanup();

	terreno.Cleanup();
	

	treeModel.Cleanup();
	rockModel.Cleanup();
	

    if (shaderProgram) glDeleteProgram(shaderProgram);
    if (window)        glfwDestroyWindow(window);
    glfwTerminate();
}
