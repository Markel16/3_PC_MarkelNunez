#include "Terreno.h"
#include "Camara.h"
#include "stb_image.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// ------------------------------------------------------------
// Helper: leer archivo de texto (shaders) a string
// ------------------------------------------------------------
static std::string LoadTextFile(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "[Terreno] No se pudo abrir: " << path << "\n";
        return "";
    }
    std::stringstream ss;
    ss << file.rdbuf();
    std::string s = ss.str();

    // quitar BOM UTF-8 si existiera
    if (s.size() >= 3 &&
        (unsigned char)s[0] == 0xEF &&
        (unsigned char)s[1] == 0xBB &&
        (unsigned char)s[2] == 0xBF)
    {
        s.erase(0, 3);
    }
    return s;
}

// ------------------------------------------------------------
// Estructura de vértice (VBO)
// ------------------------------------------------------------
struct TerrenoVertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
};

// ------------------------------------------------------------
// Init
// ------------------------------------------------------------
bool Terreno::Init(const std::string& heightmapPath,
    const std::string& grassTexPath,
    const std::string& rockTexPath)
{
    std::vector<float> heights;
    if (!loadHeightmap(heightmapPath, heights))
        return false;

    // Guardamos alturas para poder consultar altura/normal
    heightsHM = heights;

    // Generar malla (esto además aplica isla+gamma y actualiza heightsHM)
    generateMesh(heightsHM);

    if (!createShader())
        return false;

    if (!loadTextures(grassTexPath, rockTexPath))
        return false;

    return true;
}

// ------------------------------------------------------------
// Cargar heightmap (grises forzado a 1 canal) -> heights [0..1]
// ------------------------------------------------------------
bool Terreno::loadHeightmap(const std::string& path, std::vector<float>& heights)
{
    stbi_set_flip_vertically_on_load(true);

    int nrChannels = 0;
    stbi_uc* data = stbi_load(path.c_str(), &widthHM, &heightHM, &nrChannels, 1);
    if (!data)
    {
        std::cerr << "[Terreno] Error cargando heightmap: " << path << "\n";
        return false;
    }

    heights.resize(widthHM * heightHM);

    for (int z = 0; z < heightHM; ++z)
    {
        for (int x = 0; x < widthHM; ++x)
        {
            int idx = z * widthHM + x;
            unsigned char pixel = data[idx];
            heights[idx] = (float)pixel / 255.0f; // 0..1
        }
    }

    stbi_image_free(data);
    return true;
}

// ------------------------------------------------------------
// Generar malla (VAO/VBO/EBO) + normales + aplicar isla+gamma
// ------------------------------------------------------------
void Terreno::generateMesh(const std::vector<float>& heightsIn)
{
    std::vector<TerrenoVertex> vertices;
    std::vector<unsigned int> indices;

    vertices.resize(widthHM * heightHM);

    // Copia local para aplicar isla+gamma y guardarla en heightsHM
    heightsHM = heightsIn;

    for (int z = 0; z < heightHM; ++z)
    {
        for (int x = 0; x < widthHM; ++x)
        {
            int idx = z * widthHM + x;

            float h = heightsHM[idx];  // 0..1

            // gamma / contraste
            h = powf(h, 2.2f);

            // isla (más alto al centro, baja a bordes)
            float nx = (float)x / (float)(widthHM - 1);
            float nz = (float)z / (float)(heightHM - 1);
            float dx = nx - 0.5f;
            float dz = nz - 0.5f;

            float dist = sqrtf(dx * dx + dz * dz);
            float island = 1.0f - (dist / 0.7071f);
            island = std::clamp(island, 0.0f, 1.0f);
            island = powf(island, 2.0f);

            h *= island;

            // IMPORTANTE: guardar la altura final (0..1) que usa GetHeightWorld()
            heightsHM[idx] = h;

            float posX = x * scaleXZ;
            float posZ = z * scaleXZ;
            float posY = h * scaleY;

            TerrenoVertex v{};
            v.pos = glm::vec3(posX, posY, posZ);
            v.uv = glm::vec2(nx, nz);
            v.normal = glm::vec3(0, 1, 0);

            vertices[idx] = v;
        }
    }

    // Índices
    for (int z = 0; z < heightHM - 1; ++z)
    {
        for (int x = 0; x < widthHM - 1; ++x)
        {
            int i0 = z * widthHM + x;
            int i1 = z * widthHM + (x + 1);
            int i2 = (z + 1) * widthHM + x;
            int i3 = (z + 1) * widthHM + (x + 1);

            indices.push_back(i0); indices.push_back(i2); indices.push_back(i1);
            indices.push_back(i1); indices.push_back(i2); indices.push_back(i3);
        }
    }

    // Normales
    for (auto& v : vertices) v.normal = glm::vec3(0.0f);

    for (size_t i = 0; i < indices.size(); i += 3)
    {
        unsigned int ia = indices[i];
        unsigned int ib = indices[i + 1];
        unsigned int ic = indices[i + 2];

        const glm::vec3& a = vertices[ia].pos;
        const glm::vec3& b = vertices[ib].pos;
        const glm::vec3& c = vertices[ic].pos;

        glm::vec3 n = glm::normalize(glm::cross(b - a, c - a));

        vertices[ia].normal += n;
        vertices[ib].normal += n;
        vertices[ic].normal += n;
    }

    for (auto& v : vertices) v.normal = glm::normalize(v.normal);

    // OpenGL buffers
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TerrenoVertex), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TerrenoVertex), (void*)offsetof(TerrenoVertex, pos));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TerrenoVertex), (void*)offsetof(TerrenoVertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TerrenoVertex), (void*)offsetof(TerrenoVertex, uv));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    numIndices = (GLsizei)indices.size();
}

// ------------------------------------------------------------
// Altura en mundo (para pegar árboles/rocas al suelo)
// ------------------------------------------------------------
float Terreno::GetHeightWorld(float worldX, float worldZ) const
{
    if (heightsHM.empty() || widthHM <= 1 || heightHM <= 1)
        return 0.0f;

    float gx = worldX / scaleXZ;
    float gz = worldZ / scaleXZ;

    gx = std::clamp(gx, 0.0f, (float)(widthHM - 1));
    gz = std::clamp(gz, 0.0f, (float)(heightHM - 1));

    int x0 = (int)gx;
    int z0 = (int)gz;
    int x1 = std::min(x0 + 1, widthHM - 1);
    int z1 = std::min(z0 + 1, heightHM - 1);

    float tx = gx - x0;
    float tz = gz - z0;

    auto H01 = [&](int x, int z) {
        return heightsHM[z * widthHM + x]; // 0..1 (ya con isla+gamma)
        };

    float h00 = H01(x0, z0);
    float h10 = H01(x1, z0);
    float h01 = H01(x0, z1);
    float h11 = H01(x1, z1);

    float h0 = h00 * (1 - tx) + h10 * tx;
    float h1 = h01 * (1 - tx) + h11 * tx;
    float h = h0 * (1 - tz) + h1 * tz;

    return h * scaleY;
}

// ------------------------------------------------------------
// Normal en mundo (para inclinar árboles/rocas)
// ------------------------------------------------------------
glm::vec3 Terreno::GetNormalWorld(float worldX, float worldZ) const
{
    // paso en mundo
    float e = scaleXZ;
    if (e <= 0.0f) e = 1.0f;

    float hL = GetHeightWorld(worldX - e, worldZ);
    float hR = GetHeightWorld(worldX + e, worldZ);
    float hD = GetHeightWorld(worldX, worldZ - e);
    float hU = GetHeightWorld(worldX, worldZ + e);

    glm::vec3 dx(2.0f * e, hR - hL, 0.0f);
    glm::vec3 dz(0.0f, hU - hD, 2.0f * e);

    glm::vec3 n = glm::normalize(glm::cross(dz, dx));
    if (n.y < 0.0f) n = -n;

    return n;
}

// ------------------------------------------------------------
// Shader terreno
// ------------------------------------------------------------
bool Terreno::createShader()
{
    std::string vsCode = LoadTextFile("../shaders/terrain.vs");
    std::string fsCode = LoadTextFile("../shaders/terreno.fs");

    if (vsCode.empty() || fsCode.empty())
    {
        std::cerr << "[Terreno] Shader vacío, revisa rutas terrain.vs / terreno.fs\n";
        return false;
    }

    const char* vsrc = vsCode.c_str();
    const char* fsrc = fsCode.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsrc, nullptr);
    glCompileShader(vs);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsrc, nullptr);
    glCompileShader(fs);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);

    glDeleteShader(vs);
    glDeleteShader(fs);

    // samplers
    glUseProgram(shaderProgram);
    GLint locGrass = glGetUniformLocation(shaderProgram, "uTexGrass");
    GLint locRock = glGetUniformLocation(shaderProgram, "uTexRock");
    if (locGrass != -1) glUniform1i(locGrass, 0);
    if (locRock != -1) glUniform1i(locRock, 1);

    return true;
}

// ------------------------------------------------------------
// Texturas
// ------------------------------------------------------------
bool Terreno::loadTextures(const std::string& grassPath, const std::string& rockPath)
{
    stbi_set_flip_vertically_on_load(true);

    int w, h, ch;

    // Grass
    unsigned char* data = stbi_load(grassPath.c_str(), &w, &h, &ch, 0);
    if (!data)
    {
        std::cerr << "[Terreno] No se pudo cargar textura cesped: " << grassPath << "\n";
        return false;
    }

    GLenum format = (ch == 4) ? GL_RGBA : GL_RGB;

    glGenTextures(1, &texGrass);
    glBindTexture(GL_TEXTURE_2D, texGrass);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    // Rock
    data = stbi_load(rockPath.c_str(), &w, &h, &ch, 0);
    if (!data)
    {
        std::cerr << "[Terreno] No se pudo cargar textura roca: " << rockPath << "\n";
        return false;
    }

    format = (ch == 4) ? GL_RGBA : GL_RGB;

    glGenTextures(1, &texRock);
    glBindTexture(GL_TEXTURE_2D, texRock);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    return true;
}

// ------------------------------------------------------------
// Draw
// ------------------------------------------------------------
void Terreno::Draw(const Camara& camara, float aspectRatio)
{
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    auto U = [&](const char* n) { return glGetUniformLocation(shaderProgram, n); };

    std::cout
        << "uRockStartHeight=" << U("uRockStartHeight") << " "
        << "uRockEndHeight=" << U("uRockEndHeight") << " "
        << "uSlopeStart=" << U("uSlopeStart") << " "
        << "uSlopeEnd=" << U("uSlopeEnd") << "\n";

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camara.GetViewMatrix();
    glm::mat4 proj = camara.GetProjectionMatrix(aspectRatio);
    glm::mat4 mvp = proj * view * model;

    glm::vec3 camPos = camara.GetPosition();

    GLint locMVP = glGetUniformLocation(shaderProgram, "uMVP");
    GLint locModel = glGetUniformLocation(shaderProgram, "uModel");
    GLint locView = glGetUniformLocation(shaderProgram, "uView");
    GLint locProj = glGetUniformLocation(shaderProgram, "uProj");
    GLint locCam = glGetUniformLocation(shaderProgram, "uCameraPos");

    if (locMVP != -1) glUniformMatrix4fv(locMVP, 1, GL_FALSE, glm::value_ptr(mvp));
    if (locModel != -1) glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
    if (locView != -1) glUniformMatrix4fv(locView, 1, GL_FALSE, glm::value_ptr(view));
    if (locProj != -1) glUniformMatrix4fv(locProj, 1, GL_FALSE, glm::value_ptr(proj));
    if (locCam != -1) glUniform3fv(locCam, 1, glm::value_ptr(camPos));

    // Ajusta estos valores a tu gusto:
    glUniform1f(glGetUniformLocation(shaderProgram, "uRockStartHeight"), 20.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "uRockEndHeight"), 40.0f);

    // Pendiente: esto va en “dot(up)”:
    // 1.0 = plano, 0.0 = vertical.
    // Empieza roca cuando baja de 0.85, y ya es roca sobre 0.60 por ejemplo.
    glUniform1f(glGetUniformLocation(shaderProgram, "uSlopeStart"), 0.5f);
    glUniform1f(glGetUniformLocation(shaderProgram, "uSlopeEnd"), 0.4f);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texGrass);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texRock);

    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}


void Terreno::Cleanup()
{
    if (EBO) glDeleteBuffers(1, &EBO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (texGrass) glDeleteTextures(1, &texGrass);
    if (texRock)  glDeleteTextures(1, &texRock);
    if (shaderProgram) glDeleteProgram(shaderProgram);

    EBO = VBO = VAO = 0;
    texGrass = texRock = 0;
    shaderProgram = 0;
}

