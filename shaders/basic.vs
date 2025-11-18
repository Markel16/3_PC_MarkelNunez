#version 330 core

// Atributos
layout (location = 0) in vec3 aPos;       // posición
layout (location = 1) in vec3 aColor;     // color (para el triángulo)
layout (location = 2) in vec2 aTexCoord;  // UV (para los cuadrados)

// Salidas hacia el fragment shader
out vec3 ourColor;
out vec2 TexCoord;
out vec2 vUV;            // [0..1] dentro del quad

uniform vec2 uOffset;    // desplazar la figura en pantalla

void main()
{
    // Posición final (mismo modelo para los 3 cuadrados)
    vec3 p = aPos;
    p.xy += uOffset;
    gl_Position = vec4(p, 1.0);

    // Posición local normalizada [0..1] para el círculo
    vUV = aPos.xy + vec2(0.5);

    // Pasamos color y UV al fragment shader
    ourColor = aColor;
    TexCoord = aTexCoord;
}
