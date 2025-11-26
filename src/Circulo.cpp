//circuolo.cpp
#include "Circulo.h"
#include <glad/glad.h>

void DrawCirculo(GLuint shaderProgram,
    GLuint quadVAO)
{
    glUseProgram(shaderProgram);
    glBindVertexArray(quadVAO);

    GLint uModeLoc = glGetUniformLocation(shaderProgram, "uMode");
    GLint uOffsetLoc = glGetUniformLocation(shaderProgram, "uOffset");
    GLint uTintColorLoc = glGetUniformLocation(shaderProgram, "uTintColor");

    if (uModeLoc != -1) glUniform1i(uModeLoc, 2);              //círculo
    if (uOffsetLoc != -1) glUniform2f(uOffsetLoc, -0.5f, -0.5f); //abajo izquierda
    if (uTintColorLoc != -1) glUniform4f(uTintColorLoc, 0.0f, 0.6f, 1.0f, 1.0f);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
