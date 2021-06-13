// vertex shader
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;

uniform mat4 view;
uniform float lightX;
uniform float lightY;

//out vec2 TexCoords;

out vec3 colorr;

void main()
{
    vec2 pos = vec2(position.x, position.y);

    if (position.z > 0.5)
    {
        vec2 v =  pos - vec2(lightX, lightY);
        vec2 vNormalized = normalize(v);
        pos.x += vNormalized.x * 10000;
        pos.y += vNormalized.y * 10000;        
    }
    gl_Position = view * vec4(pos.x, pos.y, 0, 1.0f);
}
 