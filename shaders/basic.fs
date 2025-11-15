#version 330 core
in vec2 vUV;
out vec4 FragColor;


in vec3 ourColor;
in vec2 TexCoord;
uniform sampler2D ourTexture;

void main() {
    float u = clamp(vUV.x, 0.0, 1.0);
    float v = clamp(vUV.y, 0.0, 1.0);
    vec3 color = vec3(u, v, 1.0 - 0.5*(u + v));
    FragColor = vec4(color, 1.0);
    FragColor = texture(ourTexture, TexCoord);

}
