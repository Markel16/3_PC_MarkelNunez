#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;
out vec2 vUV;

uniform vec2 uOffset;

void main()
{
    vec3 p = aPos;
    p.xy += uOffset;          // para mover el quad / círculo
    gl_Position = vec4(p, 1.0);

    ourColor = aColor;        // para el triángulo (modo 3)
    TexCoord = aTexCoord;     // para texturas
    vUV      = aPos.xy + vec2(0.5); // para el círculo (modo 2)
}

