//CuadradoMix.cpp
#include "CuadradoMix.h"
#include <glad/glad.h>

void DrawCuadradoMix(GLuint shaderProgram,
    GLuint quadVAO,
    GLuint texture1,
    GLuint texture2)
{
    glUseProgram(shaderProgram);
    glBindVertexArray(quadVAO);

    GLint uModeLoc = glGetUniformLocation(shaderProgram, "uMode");
    GLint uOffsetLoc = glGetUniformLocation(shaderProgram, "uOffset");
    GLint uTintColorLoc = glGetUniformLocation(shaderProgram, "uTintColor");

    if (uModeLoc != -1) glUniform1i(uModeLoc, 0);//modo 0 = 2 texturas
    if (uOffsetLoc != -1) glUniform2f(uOffsetLoc, -0.5f, 0.5f); //arriba izquierda
    if (uTintColorLoc != -1) glUniform4f(uTintColorLoc, 1, 1, 1, 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
