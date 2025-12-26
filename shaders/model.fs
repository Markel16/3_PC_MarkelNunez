#version 330 core

in vec3 vN;
in vec2 vUV;

out vec4 FragColor;

uniform int uHasTex;
uniform sampler2D uTex0;

void main()
{
    // luz simple para que no sea todo blanco plano
    vec3 N = normalize(vN);
    float l = max(dot(N, normalize(vec3(0.3, 1.0, 0.2))), 0.2);

    vec3 base = vec3(0.7, 0.7, 0.7);

    if (uHasTex == 1)
        base = texture(uTex0, vUV).rgb;

    FragColor = vec4(base * l, 1.0);
}
