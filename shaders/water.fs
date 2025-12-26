#version 330 core

in vec2 vUV;
uniform sampler2D uWaterTex;

out vec4 FragColor;

void main()
{
    vec4 c = texture(uWaterTex, vUV);
    FragColor = c;
}
