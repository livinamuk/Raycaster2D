// fragment shader
#version 440 core
out vec4 FragColor;
in vec2 TexCoords;

layout (binding = 0) uniform sampler2D worldTexture;
layout (binding = 1) uniform sampler2D lineOfSightTexture;
layout (binding = 2) uniform sampler2D lightingTexture;
layout (binding = 3) uniform sampler2D editorTexture;

uniform vec3 colorTint;
uniform int renderMode;
uniform bool u_editorOpen;
  
void main()
{
    vec4 worldColor = texture(worldTexture, TexCoords);
    vec4 lineOfSightColor = texture(lineOfSightTexture, TexCoords);
    vec4 lightingColor = texture(lightingTexture, TexCoords);
    vec4 editorColor = texture(editorTexture, TexCoords);

    worldColor *= 1.1;

    if (renderMode == 0)
        FragColor = worldColor * 0.85;

    if (renderMode == 1)
        FragColor = (lightingColor + vec4(0.1))  * (worldColor);
        
    if (renderMode == 2) 
        FragColor = lightingColor * worldColor * lineOfSightColor;


    const float gamma = 2.2;
    vec3 hdrColor = FragColor.rgb;  
    // reinhard tone mapping
    vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
    FragColor.rgb = mix(mapped, FragColor.rgb, 0.85);    
    FragColor.rgb =  clamp(FragColor.rgb, 0.0, 1.0);
    
    FragColor = worldColor  *lightingColor *  lineOfSightColor;
    //FragColor = worldColor  *  lightingColor;

    if (u_editorOpen)
        FragColor += editorColor;

} 