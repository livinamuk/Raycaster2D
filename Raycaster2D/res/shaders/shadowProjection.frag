// fragment shader
#version 330 core

//layout (location = 0) out vec4 FragColor;
layout (location = 0) out float FragColor;

uniform int xMouse;
uniform int yMouse;
uniform int screenWidth;
uniform int screenHeight;
uniform int camX;
uniform int camY;

in vec3 colorr;

void main()
{
	//FragColor = vec4(0,0,0,1);
	FragColor = 0;
} 