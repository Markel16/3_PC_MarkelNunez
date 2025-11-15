#version 330 core
layout (location = 0) in vec3 aPos;


layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;
out vec3 ourColor;
out vec2 TexCoord;
uniform vec2 uOffset;
out vec2 vUV;                 // coordenadas 0..1 dentro del cuadrado

void main() {
    vUV = aPos.xy + vec2(0.5);   // [-0.5,0.5] -> [0,1]
    vec3 p = aPos; 
    p.xy += uOffset;             // movimiento
    gl_Position = vec4(p, 1.0);
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}
