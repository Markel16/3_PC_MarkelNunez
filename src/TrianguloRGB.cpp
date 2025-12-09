#include "TrianguloRGB.h"

// x,y,z + r,g,b
static const float triVertsRGB[] = {
     0.2f, -0.2f, 0.0f,   1.0f, 0.0f, 0.0f,  // rojo
     0.8f, -0.2f, 0.0f,   0.0f, 1.0f, 0.0f,  // verde
     0.5f, -0.8f, 0.0f,   0.0f, 0.0f, 1.0f   // azul
};

void TrianguloRGB::Init(GLuint shaderProgram)
{
    shader = shaderProgram;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triVertsRGB), triVertsRGB, GL_STATIC_DRAW);

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

void TrianguloRGB::Draw()
{
    glUseProgram(shader);
    glBindVertexArray(VAO);

    GLint uModeLoc = glGetUniformLocation(shader, "uMode");
    GLint uOffsetLoc = glGetUniformLocation(shader, "uOffset");

    if (uModeLoc != -1) glUniform1i(uModeLoc, 3);          // modo triángulo RGB
    if (uOffsetLoc != -1) glUniform2f(uOffsetLoc, 0.0f, 0.0f); // lo dejamos en su sitio

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void TrianguloRGB::Cleanup()
{
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    VAO = VBO = 0;
}
