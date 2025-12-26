#include "Model.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

static bool StartsWith(const std::string& s, const char* p)
{
    return s.rfind(p, 0) == 0;
}

struct ObjIndexKey
{
    int v = -1;
    int vt = -1;
    int vn = -1;

    bool operator==(const ObjIndexKey& o) const { return v == o.v && vt == o.vt && vn == o.vn; }
};

struct ObjIndexKeyHash
{
    size_t operator()(const ObjIndexKey& k) const noexcept
    {
        // hash simple
        size_t h1 = std::hash<int>{}(k.v);
        size_t h2 = std::hash<int>{}(k.vt);
        size_t h3 = std::hash<int>{}(k.vn);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

static bool ParseFaceToken(const std::string& tok, int& v, int& vt, int& vn)
{
    v = vt = vn = -1;

    // formatos: v / v/vt / v//vn / v/vt/vn
    // OBJ es 1-based
    size_t a = tok.find('/');
    if (a == std::string::npos)
    {
        v = std::stoi(tok);
        return true;
    }

    size_t b = tok.find('/', a + 1);
    std::string sV = tok.substr(0, a);
    std::string sVT = (b == std::string::npos) ? tok.substr(a + 1) : tok.substr(a + 1, b - a - 1);
    std::string sVN = (b == std::string::npos) ? "" : tok.substr(b + 1);

    if (!sV.empty()) v = std::stoi(sV);
    if (!sVT.empty()) vt = std::stoi(sVT);
    if (!sVN.empty()) vn = std::stoi(sVN);

    return true;
}

void Model::SetMat4(GLuint prog, const char* name, const glm::mat4& m)
{
    GLint loc = glGetUniformLocation(prog, name);
    if (loc != -1) glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}

bool Model::Load(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "[Model] No se pudo abrir OBJ: " << path << "\n";
        return false;
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texcoords;
    std::vector<glm::vec3> normals;

    std::vector<Vertex> outVerts;
    std::vector<unsigned> outIdx;

    std::unordered_map<ObjIndexKey, unsigned, ObjIndexKeyHash> map;
    map.reserve(20000);

    bool hasTex = false;
    bool hasNorm = false;

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string tag;
        ss >> tag;

        if (tag == "v")
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
            // leemos N tokens, triangulamos en fan
            std::vector<std::string> tokens;
            std::string t;
            while (ss >> t) tokens.push_back(t);
            if (tokens.size() < 3) continue;

            auto GetIndex = [&](const std::string& tok) -> unsigned
                {
                    int v, vt, vn;
                    ParseFaceToken(tok, v, vt, vn);

                    // OBJ permite negativos (relativos al final)
                    auto FixIndex = [](int idx, int size) -> int
                        {
                            if (idx > 0) return idx - 1;
                            if (idx < 0) return size + idx;
                            return -1;
                        };

                    int iv = FixIndex(v, (int)positions.size());
                    int iuv = FixIndex(vt, (int)texcoords.size());
                    int in = FixIndex(vn, (int)normals.size());

                    ObjIndexKey key{ iv, iuv, in };
                    auto it = map.find(key);
                    if (it != map.end()) return it->second;

                    Vertex vert{};
                    vert.pos = (iv >= 0) ? positions[iv] : glm::vec3(0);
                    vert.uv = (iuv >= 0) ? texcoords[iuv] : glm::vec2(0);
                    vert.normal = (in >= 0) ? normals[in] : glm::vec3(0, 1, 0);

                    if (iuv >= 0) hasTex = true;
                    if (in >= 0) hasNorm = true;

                    unsigned newIndex = (unsigned)outVerts.size();
                    outVerts.push_back(vert);
                    map[key] = newIndex;
                    return newIndex;
                };

            unsigned i0 = GetIndex(tokens[0]);
            for (size_t k = 1; k + 1 < tokens.size(); k++)
            {
                unsigned i1 = GetIndex(tokens[k]);
                unsigned i2 = GetIndex(tokens[k + 1]);

                outIdx.push_back(i0);
                outIdx.push_back(i1);
                outIdx.push_back(i2);
            }
        }
    }

    // Si no había normales reales, puedes dejarlas como (0,1,0).
    // Para tu práctica vale.

    meshes.clear();
    Mesh mesh;
    mesh.Build(outVerts, outIdx);
    mesh.SetHasTex(hasTex);
    meshes.push_back(std::move(mesh));

    std::cout << "[Model] OBJ cargado: " << path
        << " | verts=" << outVerts.size()
        << " tris=" << (outIdx.size() / 3) << "\n";

    return true;
}

void Model::Draw(GLuint shaderProgram, const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj) const
{
    glUseProgram(shaderProgram);

    SetMat4(shaderProgram, "uModel", model);
    SetMat4(shaderProgram, "uView", view);
    SetMat4(shaderProgram, "uProj", proj);

    // si tu shader tiene uHasTex, lo ponemos a 0 para evitar cosas raras
    GLint locHas = glGetUniformLocation(shaderProgram, "uHasTex");
    if (locHas != -1) glUniform1i(locHas, 0);

    for (const auto& m : meshes)
        m.Draw();
}

void Model::Cleanup()
{
    for (auto& m : meshes) m.Cleanup();
    meshes.clear();
}
