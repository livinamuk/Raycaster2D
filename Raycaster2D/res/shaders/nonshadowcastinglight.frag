// fragment shader
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
  
uniform vec3 color;
uniform float strength;
uniform sampler2D fboAttachment;
  
//uniform int rotate;

void main()
{
    vec2 rotatedTex = TexCoords;
  
  int rotate = 2;

    if (rotate == 1) 
        rotatedTex = vec2(TexCoords.y, TexCoords.x);
    if (rotate == 2) 
        rotatedTex = vec2(-TexCoords.x, TexCoords.y);
    if (rotate == 3) 
        rotatedTex = vec2(-TexCoords.y, -TexCoords.x);
    
    float angle = 180;

    // check if angle is 0 90 180 270 and then skip sin/cos thing below

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

    vec3 newCoords = vec3(TexCoords, 0) * rotMatrix;
  //  rotatedTex.x = newCoords.x;
  //  rotatedTex.y = newCoords.y;
        
    FragColor = texture(fboAttachment, rotatedTex);
    FragColor.rgb *= color * strength;
} 