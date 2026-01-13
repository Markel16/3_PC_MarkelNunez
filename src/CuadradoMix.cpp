#include "CuadradoMix.h"


#include "stb_image.h"

#include <iostream>

// Quad: POS (x,y,z) + UV (u,v)
static const float quadVertsMix[] = {
    //   x,      y,    z,    u,    v
    -0.5f,  0.5f, 0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, 0.0f,  1.0f, 1.0f
};

static const unsigned int quadIndices[] = { 0,1,2, 0,2,3 };

void CuadradoMix::Init(GLuint shaderProgram)
{
    shader = shaderProgram;

    // VAO/VBO/EBO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertsMix), quadVertsMix, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    // posición -> location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // UV -> location 2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
        5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // --- Cargar TEXTURA 1 ---
    int w, h, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("../Assets/Texture1.jpg", &w, &h, &channels, 0);
    if (!data) {
        std::cerr << "[ERROR] No se pudo cargar Texture1.jpg\n";
        return;
    }

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    glGenTextures(1, &tex1);
    glBindTexture(GL_TEXTURE_2D, tex1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format,
        GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    // --- Cargar TEXTURA 2 ---
    data = stbi_load("../Assets/Texture 2.jpg", &w, &h, &channels, 0);
    if (!data) {
        std::cerr << "[ERROR] No se pudo cargar Texture 2.jpg\n";
        return;
    }

    format = (channels == 4) ? GL_RGBA : GL_RGB;

    glGenTextures(1, &tex2);
    glBindTexture(GL_TEXTURE_2D, tex2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format,
        GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
}

void CuadradoMix::Draw()
{
    glUseProgram(shader);
    glBindVertexArray(VAO);

    GLint uModeLoc = glGetUniformLocation(shader, "uMode");
    GLint uOffsetLoc = glGetUniformLocation(shader, "uOffset");
    GLint uTintColorLoc = glGetUniformLocation(shader, "uTintColor");

    if (uModeLoc != -1) glUniform1i(uModeLoc, 0);              // modo mezclado
    if (uOffsetLoc != -1) glUniform2f(uOffsetLoc, -0.5f, 0.5f);  // arriba izquierda
    if (uTintColorLoc != -1) glUniform4f(uTintColorLoc, 1, 1, 1, 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex2);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void CuadradoMix::Cleanup()
{
    if (VAO)  glDeleteVertexArrays(1, &VAO);
    if (VBO)  glDeleteBuffers(1, &VBO);
    if (EBO)  glDeleteBuffers(1, &EBO);
    if (tex1) glDeleteTextures(1, &tex1);
    if (tex2) glDeleteTextures(1, &tex2);
    VAO = VBO = EBO = tex1 = tex2 = 0;
}
