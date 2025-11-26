#version 330 core

in vec2 vUV;
in vec3 ourColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D uTex1;
uniform sampler2D uTex2;
uniform vec4 uTintColor;

uniform int   uMode;   // 0=2 texturas, 1=rotado, 2=círculo, 3=triángulo
uniform float uAngle;  // para la textura rotada

void main()
{
    if (uMode == 0) {
        //Cuadrado con 2 texturas mezcladas + color
        vec4 c1 = texture(uTex1, TexCoord);
        vec4 c2 = texture(uTex2, TexCoord);

        // mezcla 50% de cada una 
        vec4 base = mix(c1, c2, 0.1);

        FragColor = base * uTintColor;
    }
    else if (uMode == 1) {
        //Cuadrado con textura rotada (usamos solo uTex1)
        vec2 centered = TexCoord - vec2(0.5);
        float c = cos(uAngle);
        float s = sin(uAngle);
        vec2 rot = vec2(
            centered.x * c - centered.y * s,
            centered.x * s + centered.y * c
        ) + vec2(0.5);

        vec4 tex = texture(uTex1, rot);
        FragColor = tex * uTintColor;
    }
    else if (uMode == 2) {
        //Círculo de color plano (en base a vUV)
        vec2 p = vUV - vec2(0.5);   // centro (0.5, 0.5)
        float r = length(p);
        if (r > 0.5)
            discard; // fuera del círculo = transparente
        FragColor = uTintColor;// color sólido
    }
    else if (uMode == 3) {
        //Triángulo con 3 colores
        FragColor = vec4(ourColor, 1.0);
    }
    else {
        
        FragColor = vec4(1.0, 0.0, 1.0, 1.0); 
    }
}
