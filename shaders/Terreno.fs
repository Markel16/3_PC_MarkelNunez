#version 330 core

in vec3 vNormal;
in vec2 vTexCoord;

uniform sampler2D uTexGrass;
uniform sampler2D uTexRock;

out vec4 FragColor;

void main()
{
    vec3 N = normalize(vNormal);

    float slope = clamp(1.0 - N.y, 0.0, 1.0);

    vec2 uv = vTexCoord * 8.0;

    vec4 grassColor = texture(uTexGrass, uv);
    vec4 rockColor  = texture(uTexRock,  uv);

    FragColor = mix(grassColor, rockColor, slope);
}
