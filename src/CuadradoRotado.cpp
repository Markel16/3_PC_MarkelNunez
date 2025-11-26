//CuadradoRotado.cpp
#include "CuadradoRotado.h"
#include <glad/glad.h>

void DrawCuadradoRotado(GLuint shaderProgram,
    GLuint quadVAO,
    GLuint texture,
    float angleSeconds)
{
    glUseProgram(shaderProgram);
    glBindVertexArray(quadVAO);

    GLint uModeLoc = glGetUniformLocation(shaderProgram, "uMode");
    GLint uOffsetLoc = glGetUniformLocation(shaderProgram, "uOffset");
    GLint uTintColorLoc = glGetUniformLocation(shaderProgram, "uTintColor");
    GLint uAngleLoc = glGetUniformLocation(shaderProgram, "uAngle");

    if (uModeLoc != -1) glUniform1i(uModeLoc, 1);             // modo 1 = rotado
    if (uOffsetLoc != -1) glUniform2f(uOffsetLoc, 0.5f, 0.5f);  // arriba derecha
    if (uAngleLoc != -1) glUniform1f(uAngleLoc, angleSeconds); // rotacion con el tiempo
    if (uTintColorLoc != -1) glUniform4f(uTintColorLoc, 1, 1, 1, 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
