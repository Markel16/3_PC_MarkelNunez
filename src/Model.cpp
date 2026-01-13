#include "Model.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#include <glm/gtc/type_ptr.hpp>


#include "stb_image.h"

// helpers

struct ObjIndexKey
{
    int v = -1;   
    int vt = -1;  
    int vn = -1; 

    bool operator==(const ObjIndexKey& o) const
    {
        return v == o.v && vt == o.vt && vn == o.vn;
    }
};


struct ObjIndexKeyHash
{
    size_t operator()(const ObjIndexKey& k) const noexcept
    {
        size_t h1 = std::hash<int>{}(k.v);
        size_t h2 = std::hash<int>{}(k.vt);
        size_t h3 = std::hash<int>{}(k.vn);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};


static bool ParseFaceToken(const std::string& tok, int& v, int& vt, int& vn)
{
    v = vt = vn = -1;

    size_t a = tok.find('/');
    if (a == std::string::npos)
    {
        // Caso "v"
        v = std::stoi(tok);
        return true;
    }

    size_t b = tok.find('/', a + 1);

    std::string sV = tok.substr(0, a);
    std::string sVT = (b == std::string::npos) ? tok.substr(a + 1) : tok.substr(a + 1, b - a - 1);
    std::string sVN = (b == std::string::npos) ? "" : tok.substr(b + 1);

    if (!sV.empty())  v = std::stoi(sV);
    if (!sVT.empty()) vt = std::stoi(sVT);
    if (!sVN.empty()) vn = std::stoi(sVN);

    return true;
}


void Model::SetMat4(GLuint prog, const char* name, const glm::mat4& m)
{
    GLint loc = glGetUniformLocation(prog, name);
    if (loc != -1)
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}


GLuint Model::LoadTextureGL(const std::string& path)
{
    stbi_set_flip_vertically_on_load(true); 

    int w, h, ch;
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, 0);
    if (!data) {
        std::cerr << "[Model] No se pudo cargar textura: " << path << "\n";
        return 0;
    }

    GLenum format = (ch == 4) ? GL_RGBA : GL_RGB;

    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    // Repetición UV
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Filtros + mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Subir al GPU + mipmaps
    glTexImage2D(GL_TEXTURE_2D, 0, (GLint)format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return tex;
}

// Textura global fallback para todo el modelo (si no hay por material)
bool Model::LoadTexture(const std::string& path)
{
    if (diffuseTex) glDeleteTextures(1, &diffuseTex);
    diffuseTex = LoadTextureGL(path);
    return diffuseTex != 0;
}


bool Model::SetTextureForMaterial(const std::string& materialName,
    const std::string& texturePath,
    bool alphaCutout)
{
    GLuint t = LoadTextureGL(texturePath);
    if (!t) return false;


    auto it = matTable.find(materialName);
    if (it != matTable.end() && it->second.tex)
        glDeleteTextures(1, &it->second.tex);

    matTable[materialName] = MatInfo{ t, alphaCutout };
    return true;
}


bool Model::Load(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "[Model] No se pudo abrir OBJ: " << path << "\n";
        return false;
    }

    // Arrays OBJ base
    std::vector<glm::vec3> positions; 
    std::vector<glm::vec2> texcoords; 
    std::vector<glm::vec3> normals; 

  
    struct BuildData
    {
        std::vector<Vertex> verts; 
        std::vector<unsigned> idx;
        std::unordered_map<ObjIndexKey, unsigned, ObjIndexKeyHash> map; 
        bool hasTex = false;  
        bool hasNorm = false;
    };

    // materialName -> BuildData
    std::unordered_map<std::string, BuildData> perMat;
    std::string currentMat = "default"; 

   
    auto FixIndex = [](int idx, int size) -> int
        {
            if (idx > 0) return idx - 1;   
            if (idx < 0) return size + idx; 
            return -1;
        };

   
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string tag;
        ss >> tag;

        if (tag == "mtllib")
        {
           
            continue;
        }
        else if (tag == "usemtl")
        {
            
            ss >> currentMat;
            if (currentMat.empty()) currentMat = "default";
            continue;
        }
        else if (tag == "v")
        {
            glm::vec3 p;
            ss >> p.x >> p.y >> p.z;
            positions.push_back(p);
        }
        else if (tag == "vt")
        {
            glm::vec2 uv;
            ss >> uv.x >> uv.y;
            texcoords.push_back(uv);
        }
        else if (tag == "vn")
        {
            glm::vec3 n;
            ss >> n.x >> n.y >> n.z;
            normals.push_back(n);
        }
        else if (tag == "f")
        {
            
            std::vector<std::string> tokens;
            std::string t;
            while (ss >> t) tokens.push_back(t);
            if (tokens.size() < 3) continue;

            BuildData& bd = perMat[currentMat];
            if (bd.map.empty()) bd.map.reserve(20000);

            
            auto GetIndex = [&](const std::string& tok) -> unsigned
                {
                    int v, vt, vn;
                    ParseFaceToken(tok, v, vt, vn);

                    int iv = FixIndex(v, (int)positions.size());
                    int iuv = FixIndex(vt, (int)texcoords.size());
                    int in = FixIndex(vn, (int)normals.size());

                    ObjIndexKey key{ iv, iuv, in };
                    auto it = bd.map.find(key);
                    if (it != bd.map.end()) return it->second;

                    Vertex vert{};
                    vert.pos = (iv >= 0) ? positions[iv] : glm::vec3(0);
                    vert.uv = (iuv >= 0) ? texcoords[iuv] : glm::vec2(0);
                    vert.normal = (in >= 0) ? normals[in] : glm::vec3(0, 1, 0);

                    if (iuv >= 0) bd.hasTex = true;
                    if (in >= 0) bd.hasNorm = true;

                    unsigned newIndex = (unsigned)bd.verts.size();
                    bd.verts.push_back(vert);
                    bd.map[key] = newIndex;
                    return newIndex;
                };

            
            unsigned i0 = GetIndex(tokens[0]);
            for (size_t k = 1; k + 1 < tokens.size(); k++)
            {
                unsigned i1 = GetIndex(tokens[k]);
                unsigned i2 = GetIndex(tokens[k + 1]);
                bd.idx.push_back(i0);
                bd.idx.push_back(i1);
                bd.idx.push_back(i2);
            }
        }
    }

    
    bool first = true;
    for (auto& kv : perMat)
    {
        for (auto& v : kv.second.verts)
        {
            if (first)
            {
                minY = maxY = v.pos.y;
                first = false;
            }
            else
            {
                minY = std::min(minY, v.pos.y);
                maxY = std::max(maxY, v.pos.y);
            }
        }
    }
    if (first) { minY = maxY = 0.0f; }

    // Construir Mesh por material
    meshes.clear();
    meshes.reserve(perMat.size());

    for (auto& kv : perMat)
    {
        const std::string& matName = kv.first;
        BuildData& bd = kv.second;

        if (bd.idx.empty() || bd.verts.empty())
            continue;

        Mesh m;
        m.Build(bd.verts, bd.idx);    
        m.SetHasTex(bd.hasTex);        
        m.SetMaterialName(matName);    
        meshes.push_back(std::move(m));
    }

    std::cout << "[Model] OBJ cargado: " << path
        << " | submeshes=" << meshes.size() << "\n";

    return !meshes.empty();
}


void Model::Draw(GLuint shaderProgram,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& proj) const
{
    glUseProgram(shaderProgram);

    // Subir matrices al shader del modelo
    SetMat4(shaderProgram, "uModel", model);
    SetMat4(shaderProgram, "uView", view);
    SetMat4(shaderProgram, "uProj", proj);

    // Cutout: NO blending global, se recorta con discard en el fragment shader
    glDisable(GL_BLEND);

    for (const auto& m : meshes)
    {
        const std::string& mat = m.GetMaterialName();

        GLuint tex = diffuseTex; // fallback
        bool alphaCutout = false;

        // Si hay textura por material, se usa
        auto it = matTable.find(mat);
        if (it != matTable.end())
        {
            tex = it->second.tex;
            alphaCutout = it->second.alphaCutout;
        }

        // Bind de textura en slot 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);

        // Uniforms 
        GLint locTex = glGetUniformLocation(shaderProgram, "uTex");
        if (locTex != -1) glUniform1i(locTex, 0);

        GLint locHas = glGetUniformLocation(shaderProgram, "uHasTex");
        if (locHas != -1) glUniform1i(locHas, tex != 0 ? 1 : 0);

        GLint locCut = glGetUniformLocation(shaderProgram, "uAlphaCutout");
        if (locCut != -1) glUniform1i(locCut, alphaCutout ? 1 : 0);

        GLint locThr = glGetUniformLocation(shaderProgram, "uAlphaThreshold");
        if (locThr != -1) glUniform1f(locThr, 0.5f);

        // Dibuja el submesh
        m.Draw();
    }
}

// Libera todos los recursos OpenGL del modelo
void Model::Cleanup()
{
    for (auto& m : meshes) m.Cleanup();
    meshes.clear();

    if (diffuseTex) glDeleteTextures(1, &diffuseTex);
    diffuseTex = 0;

    for (auto& kv : matTable)
    {
        if (kv.second.tex) glDeleteTextures(1, &kv.second.tex);
    }
    matTable.clear();
}
