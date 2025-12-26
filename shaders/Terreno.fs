#version 330 core

in vec3 vNormal;
in vec2 vTexCoord;
in vec3 vWorldPos;

uniform sampler2D uTexGrass;
uniform sampler2D uTexRock;

//para la niebla estoe s para la posicion de la camara cuanndo se aleja y se acerca
uniform vec3 uCameraPos;

out vec4 FragColor;

void main()
{
    vec3 N = normalize(vNormal);

    // Mezcla del cespez y piedra
    float slope = 1.0 - N.y;
    slope = smoothstep(0.2, 0.6, slope);

    vec2 uv = vTexCoord * 8.0;
    vec4 grass = texture(uTexGrass, uv);
    vec4 rock  = texture(uTexRock,  uv);

    vec4 color = mix(grass, rock, slope);

    //niebla
    float dist = distance(uCameraPos, vWorldPos);

    float fogStart = 40.0;
    float fogEnd   = 120.0;

    float fogFactor = clamp((fogEnd - dist) / (fogEnd - fogStart), 0.0, 1.0);

    vec3 fogColor = vec3(0.6, 0.7, 0.8); // colores

    vec3 finalColor = mix(fogColor, color.rgb, fogFactor);

    FragColor = vec4(finalColor, 1.0);
}
