// fragment shader
#version 440 core
out vec4 FragColor;
in vec2 TexCoords;

layout (binding = 0) uniform sampler2D worldTexture;
layout (binding = 1) uniform sampler2D lineOfSightTexture;
layout (binding = 2) uniform sampler2D lightingTexture;
layout (binding = 3) uniform sampler2D editorTexture;
layout (binding = 4) uniform sampler2D temp;
layout (binding = 4) uniform sampler2D lineOfSightSpriteTexture;

uniform vec3 colorTint;
uniform int renderMode;
uniform int u_viewPosX;
uniform int u_viewPosY;
uniform int u_camX;
uniform int u_camY;

uniform bool u_editorOpen;
  
uniform int screenWidth;
uniform int screenHeight;
uniform int windowWidth;
uniform int windowHeight;

 float Map(float value, float min1, float max1, float min2, float max2)
{
     float perc = (value - min1) / (max1 - min1);
     float val = perc * (max2 - min2) + min2;
     return val;
}

void main()
{
    vec4 worldColor = texture(worldTexture, TexCoords);
    vec4 lineOfSightColor = texture(lineOfSightTexture, TexCoords);
    vec4 lightingColor = texture(lightingTexture, TexCoords);
    vec4 editorColor = texture(editorTexture, TexCoords);
    vec4 tempColor = texture(temp, TexCoords);
    vec4 lineOfSightSpriteColor = texture(lineOfSightSpriteTexture, TexCoords);


    // Darken LOS over distance

    vec2 uv = gl_FragCoord.xy  / vec2(windowWidth/windowHeight);
	float dist = length(uv - vec2(u_viewPosX - u_camX + screenWidth / 2, screenHeight-u_viewPosY + (u_camY - screenHeight/2 )));
	vec3 brightness = vec3(1 - (dist * 0.0025));
	//lineOfSightColor *= vec4(brightness, 1);




    worldColor *= 1.1;

    if (renderMode == 0)
        FragColor = worldColor * 0.85;

    if (renderMode == 1)
        FragColor = (lightingColor + vec4(0.1))  * (worldColor);
        
    if (renderMode == 2) 
        FragColor = lightingColor * worldColor * lineOfSightColor;


    const float gamma = 2.2;
    vec3 hdrColor = FragColor.rgb;  
    // reinhard tone mapping
    vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
    FragColor.rgb = mix(mapped, FragColor.rgb, 0.85);    
    FragColor.rgb =  clamp(FragColor.rgb, 0.0, 1.0);
    
    FragColor = worldColor  *lightingColor *  lineOfSightColor;
  
 // FragColor =   lightingColor;

  
  //FragColor = lightingColor;
//  FragColor = tempColor;

    if (u_editorOpen)
        FragColor += editorColor;
} 