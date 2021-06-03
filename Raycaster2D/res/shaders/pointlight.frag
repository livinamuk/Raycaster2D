// fragment shader
#version 440 core
out vec4 FragColor;
in vec2 TexCoords;

layout (binding = 0) uniform sampler2D inputTexture;
layout (binding = 1) uniform sampler2D lineOfSightMaskTexture;
layout (binding = 2) uniform sampler2D lineOfSightMaskTextureWithoutOutlLine;

uniform int xMouse;
uniform int yMouse;
uniform int camX;
uniform int camY;
uniform int screenWidth;
uniform int screenHeight;
uniform vec3 color;
uniform float strength;
  
float Map(float value, float min1, float max1, float min2, float max2)
{
     float perc = (value - min1) / (max1 - min1);
     float val = perc * (max2 - min2) + min2;
     return val;
}

void main()
{
    vec4 spriteColor = texture(inputTexture, TexCoords);

    
    float maskTexCoordX = Map(gl_FragCoord.x, 0, screenWidth, 0, 1);
    float maskTexCoordY= Map(gl_FragCoord.y, 0, screenHeight, 0, 1);
    vec4 lineOfSightMask = texture(lineOfSightMaskTexture, vec2(maskTexCoordX, maskTexCoordY));
    
    //FragColor = inputColor;
   // FragColor.rgb *= color;
   
    lineOfSightMask = texture(lineOfSightMaskTexture, vec2(maskTexCoordX, maskTexCoordY));
    
    FragColor = spriteColor * vec4(color * strength, 1) * lineOfSightMask;
    
  //  FragColor = lineOfSightMask;
 
} 