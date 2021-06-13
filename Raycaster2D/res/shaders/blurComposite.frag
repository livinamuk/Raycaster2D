// fragment shader
#version 440 core
out vec4 FragColor;
in vec2 TexCoords;

layout (binding = 0) uniform sampler2D blur0;
layout (binding = 1) uniform sampler2D blur1;
layout (binding = 2) uniform sampler2D blur2;
layout (binding = 3) uniform sampler2D blur3;
uniform vec3 colorTint;
uniform int levels;
uniform bool u_editorOpen;
  
void main()
{
    vec4 blur0 = texture(blur0, TexCoords);
    vec4 blur1 = texture(blur1, TexCoords);
    vec4 blur2 = texture(blur2, TexCoords);
    vec4 blur3 = texture(blur3, TexCoords);

    vec4 color;
    
    if (levels == 1)
        color = blur0;
    else if (levels == 2)
        color = (blur0 + blur1) / 2;
    else if (levels == 3)
        color = (blur0 + blur1 + blur2) / 3;
    else if (levels == 4)
        color = (blur0 + blur1 + blur2 + blur3) / 4;
    
    FragColor = color;
} 