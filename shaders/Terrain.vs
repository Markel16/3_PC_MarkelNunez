#version 330 core

// Entrada desde el VBO
layout (location = 0) in vec3 aPos;    // posición
layout (location = 1) in vec3 aNormal; // normal
layout (location = 2) in vec2 aTex;    // uv

// Matrices
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

// Salidas hacia el fragment shader
out vec3 vNormal;   // normal en espacio mundial
out vec2 vTexCoord; // coordenadas de textura
//niebla
out vec3 vWorldPos;


void main()
{
    // Transformamos la posición a clip space
    gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);

    // Normal transformada por la parte lineal de la matriz de modelo
    mat3 normalMatrix = mat3(transpose(inverse(uModel)));
    vNormal = normalize(normalMatrix * aNormal);

    // Pasamos las UV tal cual
    vTexCoord = aTex;
    //niebla
    vec4 worldPos = uModel * vec4(aPos, 1.0);
vWorldPos = worldPos.xyz;

gl_Position = uProj * uView * worldPos;

}
