in vec3 vN;
in vec2 vUV;

out vec4 FragColor;

uniform int uHasTex;        
uniform sampler2D uTex;     

uniform int uAlphaCutout;      
uniform float uAlphaThreshold;

void main()
{

    vec3 N = normalize(vN);
    float l = max(dot(N, normalize(vec3(0.3, 1.0, 0.2))), 0.2);

   
    vec4 texC = vec4(0.7, 0.7, 0.7, 1.0);

    if (uHasTex == 1)
        texC = texture(uTex, vUV);

   
    if (uAlphaCutout == 1 && texC.a < uAlphaThreshold)
        discard;

   
    FragColor = vec4(texC.rgb * l, 1.0);
}
