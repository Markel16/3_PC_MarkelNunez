#version 330 core

in vec3 vNormal;
in vec2 vTexCoord;
in vec3 vWorldPos;

uniform sampler2D uTexGrass;
uniform sampler2D uTexRock;

// niebla
uniform vec3 uCameraPos;

out vec4 FragColor;

void main()
{
    vec3 N = normalize(vNormal);

    
    vec2 uv = vTexCoord * 8.0;
    vec3 grass = texture(uTexGrass, uv).rgb;
    vec3 rock  = texture(uTexRock,  uv).rgb;



   
    float rockStart = 6.0;   
    float rockEnd   = 10.0;  

  
    float heightMask = smoothstep(rockStart, rockEnd, vWorldPos.y);

  
    float slope = 1.0 - N.y;
    float slopeMask = smoothstep(0.25, 0.65, slope);

    
    float rockMask = clamp(heightMask * (0.85 + 0.4 * slopeMask), 0.0, 1.0);

    vec3 baseColor = mix(grass, rock, rockMask);


    float dist = distance(uCameraPos, vWorldPos);
    float fogStart = 1.0;
    float fogEnd   = 300.0;

    float fogFactor = clamp((fogEnd - dist) / (fogEnd - fogStart), 0.0, 1.0);
    vec3 fogColor = vec3(0.6, 0.7, 0.8);

    vec3 finalColor = mix(fogColor, baseColor, fogFactor);
    FragColor = vec4(finalColor, 1.0);
}
