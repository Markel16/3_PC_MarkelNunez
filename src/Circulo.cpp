#include "Circulo.h"

// Reutilizamos un quad
static const float quadVertsCircle[] = {
    -0.5f,  0.5f, 0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
     0.5f, -0.5f, 0.0f,   1.0f, 0.0f,
     0.5f,  0.5f, 0.0f,   1.0f, 1.0f
};

static const unsigned int quadIndicesCircle[] = { 0,1,2, 0,2,3 };

void Circulo::Init(GLuint shaderProgram)
{
    shader = shaderProgram;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertsCircle), quadVertsCircle, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        sizeof(quadIndicesCircle), quadIndicesCircle, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
        5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Circulo::Draw()
{
    glUseProgram(shader);
    glBindVertexArray(VAO);

    GLint uModeLoc = glGetUniformLocation(shader, "uMode");
    GLint uOffsetLoc = glGetUniformLocation(shader, "uOffset");
    GLint uTintColorLoc = glGetUniformLocation(shader, "uTintColor");

    if (uModeLoc != -1)   glUniform1i(uModeLoc, 2);            // modo círculo
    if (uOffsetLoc != -1) glUniform2f(uOffsetLoc, -0.5f, -0.5f); // abajo izquierda
    if (uTintColorLoc != -1) glUniform4f(uTintColorLoc,
        0.0f, 0.6f, 1.0f, 1.0f); // azulito

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Circulo::Cleanup()
{
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
    VAO = VBO = EBO = 0;
}
