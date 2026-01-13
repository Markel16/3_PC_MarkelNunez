#version 330 core

in vec2 vUV;
uniform sampler2D uWaterTex;

out vec4 FragColor;

void main()
{
    
    vec4 c = texture(uWaterTex, vUV);

   
    float alpha = 0.75;

    // Tinte azul leve
    vec3 tint = vec3(0.10, 0.35, 0.85);

    // Mezcla la textura con el tinte 
    vec3 finalRGB = mix(c.rgb, tint, 0.15);

    
    FragColor = vec4(finalRGB, alpha);
}
