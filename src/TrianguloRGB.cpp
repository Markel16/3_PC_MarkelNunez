//trianguloRGB.cpp
#include "TrianguloRGB.h"
#include <glad/glad.h>

void InitTrianguloRGB(GLuint& triVAO, GLuint& triVBO)
{
    float triVerts[] = {
        //    x      y     z      r     g     b
         0.2f, -0.2f, 0.0f,   1.0f, 0.0f, 0.0f, // rojo
         0.8f, -0.2f, 0.0f,   0.0f, 1.0f, 0.0f, // verde
         0.5f, -0.8f, 0.0f,   0.0f, 0.0f, 1.0f  // azul
    };

    glGenVertexArrays(1, &triVAO);
    glBindVertexArray(triVAO);

    glGenBuffers(1, &triVBO);
    glBindBuffer(GL_ARRAY_BUFFER, triVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triVerts), triVerts, GL_STATIC_DRAW);

    // posición -> location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color -> location 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void DrawTrianguloRGB(GLuint shaderProgram,
    GLuint triVAO)
{
    glUseProgram(shaderProgram);

    GLint uModeLoc = glGetUniformLocation(shaderProgram, "uMode");
    GLint uOffsetLoc = glGetUniformLocation(shaderProgram, "uOffset");

    if (uModeLoc != -1) glUniform1i(uModeLoc, 3);//triángulo
    if (uOffsetLoc != -1) glUniform2f(uOffsetLoc, 0.0f, 0.0f); //no se desplaza

    glBindVertexArray(triVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
