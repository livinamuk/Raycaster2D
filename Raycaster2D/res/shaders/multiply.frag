// fragment shader
#version 440 core
out vec4 FragColor;
in vec2 TexCoords;

layout (binding = 0) uniform sampler2D inputTexture;

uniform vec3 colorTint;
  
void main()
{
    vec4 inputColor = texture(inputTexture, TexCoords);
    
    FragColor = inputColor;

    if (inputColor.a == 0.0)
    {
        float outLineSizeH = 0.0125;
        float outLineSizeV = 0.0125 * (16.0/9.0);

        if (texture(inputTexture, TexCoords + vec2(0.0,          outLineSizeV)).a  != 0.0 ||
            texture(inputTexture, TexCoords + vec2(0.0,         -outLineSizeV)).a  != 0.0 ||
            texture(inputTexture, TexCoords + vec2(outLineSizeH,  0.0)).a          != 0.0 ||
            texture(inputTexture, TexCoords + vec2(-outLineSizeH, 0.0)).a          != 0.0 ||
            texture(inputTexture, TexCoords + vec2(-outLineSizeH, outLineSizeV)).a  != 0.0 ||
            texture(inputTexture, TexCoords + vec2(-outLineSizeH, -outLineSizeV)).a != 0.0 ||
            texture(inputTexture, TexCoords + vec2(outLineSizeH,  outLineSizeV)).a  != 0.0 ||
            texture(inputTexture, TexCoords + vec2(outLineSizeH,  -outLineSizeV)).a != 0.0) 
       
            FragColor = vec4(1, 1, 1, 1);
    }

    
} 