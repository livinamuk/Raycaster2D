#version 420 core

in vec2 TexCoords;
layout (location = 0) out vec4 FragColor;
  
uniform vec3 color;
uniform float brightness;
uniform int screenWidth;
uniform int screenHeight;

layout (binding = 0) uniform sampler2D lightSpriteTexture;
layout (binding = 1) uniform sampler2D shadowMaskTexture;
 
 float Map(float value, float min1, float max1, float min2, float max2)
{
     float perc = (value - min1) / (max1 - min1);
     float val = perc * (max2 - min2) + min2;
     return val;
}

void main()
{
    vec4 lightSprite = texture(lightSpriteTexture, TexCoords);
    
    float sampleX = Map(gl_FragCoord.x, 0, screenWidth, 0, 1);
    float sampleY = Map(gl_FragCoord.y, 0, screenHeight, 0, 1);

    float shadowMask = texture(shadowMaskTexture, vec2(sampleX, sampleY)).r;

    FragColor = lightSprite * shadowMask;
    FragColor.rgb *= color * brightness;
    //FragColor = lightSprite * 1;

   // FragColor = vec4(0, 0, 1, 1);
} 