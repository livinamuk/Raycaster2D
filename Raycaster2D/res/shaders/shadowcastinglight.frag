// fragment shader
#version 440 core
out vec4 FragColor;

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D inputTexture;

uniform int screenWidth;
uniform int screenHeight;
uniform vec3 color;
uniform float brightness;

uniform vec2 lightScreenSpacePixelPosition;
uniform float spriteWidth;
uniform float spriteHeight;
uniform float lightScale;
uniform int rotate;
  
void main()
{   
    vec4 spriteColor = texture(inputTexture, vec2(TexCoords));
    FragColor = spriteColor * vec4(color * brightness, 1);
} 