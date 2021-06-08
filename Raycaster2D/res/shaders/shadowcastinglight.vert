// vertex shader
#version 330 core
layout (location = 0) in vec2 position;

uniform mat4 view;

uniform vec2 lightScreenSpacePixelPosition;
uniform float spriteWidth;
uniform float spriteHeight;
uniform float lightScale;
uniform int screenWidth;
uniform int screenHeight;
uniform float angle;

out vec2 TexCoords;

#define PI 3.1415926538

float Map(float value, float min1, float max1, float min2, float max2)
{
     float perc = (value - min1) / (max1 - min1);
     float val = perc * (max2 - min2) + min2;
     return val;
}

void main()
{
    gl_Position = view * vec4(position, 0.0f, 1.0f);

    // get pixel coordinates
    vec2 viewportSize = vec2(960, 540);
    vec3 ndc = gl_Position.xyz / gl_Position.w; //perspective divide/normalize
    vec2 viewportCoord = ndc.xy * 0.5 + 0.5; //ndc is -1 to 1 in GL. scale for 0 to 1
    vec2 viewportPixelCoord = viewportCoord * viewportSize;

    // calculate tex coords for visibility polygon using light position
    float worldRangeLowerX = lightScreenSpacePixelPosition.x - spriteWidth*lightScale/2;
    float worldRangeLowerY = lightScreenSpacePixelPosition.y - spriteHeight*lightScale/2;
    float worldRangeUpperX = worldRangeLowerX + spriteWidth * lightScale;
    float worldRangeUpperY = worldRangeLowerY + spriteHeight * lightScale;
    float maskTexCoordX = Map(viewportPixelCoord.x, worldRangeLowerX, worldRangeUpperX, 0, 1);
    float maskTexCoordY = Map(screenHeight-viewportPixelCoord.y, worldRangeLowerY, worldRangeUpperY, 0, 1);
    TexCoords = vec2(maskTexCoordX, maskTexCoordY);

    // rotation matrix
    mat3 rotMatrix;
    rotMatrix[0][0] = cos(angle);
    rotMatrix[0][1] = -sin(angle);
    rotMatrix[0][2] = 0;
    rotMatrix[1][0] = sin(angle);
    rotMatrix[1][1] = cos(angle);
    rotMatrix[1][2] = 0;
    rotMatrix[2][0] = 0;
    rotMatrix[2][1] = 0;
    rotMatrix[2][2] = 1;



    vec2 UV = vec2(maskTexCoordX, maskTexCoordY) + vec2(-0.5, -0.5);
    vec3 newCoords = rotMatrix * vec3(UV.x, UV.y, 0);
    TexCoords = vec2(newCoords.x, newCoords.y) - vec2(-0.5,-0.5);
}
 