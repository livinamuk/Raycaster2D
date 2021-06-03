// fragment shader
#version 440 core
out vec4 FragColor;
in vec2 TexCoords;

layout (binding = 0) uniform sampler2D worldTexture;
layout (binding = 1) uniform sampler2D lineOfSightTexture;
layout (binding = 2) uniform sampler2D lightingTexture;
layout (binding = 3) uniform sampler2D test;
layout (binding = 4) uniform sampler2D test2;


uniform vec3 colorTint;
uniform int renderMode;
  
void main()
{
    vec4 worldColor = texture(worldTexture, TexCoords);
    vec4 lineOfSightColor = texture(lineOfSightTexture, TexCoords);
    vec4 lightingColor = texture(lightingTexture, TexCoords);

    worldColor *= 1.1;

    if (renderMode == 0)
        FragColor = worldColor;

    if (renderMode == 1)
        FragColor = (lightingColor  * (worldColor));

    if (renderMode == 2)
        FragColor = lightingColor * worldColor * lineOfSightColor;

 //   if (renderMode == 2)
 //       FragColor = lineOfSightColor;

  //  FragColor = texture(lightingTexture, TexCoords) * worldColor;
    //FragColor = lightingColor; 
    if (renderMode == 2) 
    FragColor = (worldColor * lineOfSightColor) + worldColor / 4;
} 