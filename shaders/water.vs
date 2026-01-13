#version 330 core

layout(location = 0) in vec3 aPos; // posición del quad
layout(location = 1) in vec2 aUV;  // UV del quad

uniform mat4 uModel; // coloca el agua en mundo (center, size, waterY)
uniform mat4 uView;  // cámara
uniform mat4 uProj;  // proyección

out vec2 vUV;

void main()
{
   
    vUV = aUV * 12.0;

   
    gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
}
