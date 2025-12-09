//CuadradoRotado.h
#pragma once
#include <glad/glad.h>

class CuadradoRotado
{
	public:
		CuadradoRotado() = default;

		void Init(GLuint shaderProgram);
		void Draw(float anlge);
		void Cleanup();

private:
	GLuint shader = 0;
	GLuint VAO = 0, VBO = 0, EBO = 0;
	GLuint tex = 0;

};
