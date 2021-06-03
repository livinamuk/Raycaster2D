#pragma once
#include "Header.h"

enum LightType {
	ROOM_LIGHT,
	SPOT_LIGHT_E
};

class Light
{
public: // fields
	glm::vec2 m_position;
	glm::vec3 m_color; 
	LightType m_type;
	float m_strength;
	float m_scale;


public: // functions
	//Light();
	Light(glm::vec2 position, glm::vec3 lightColor, float scale, LightType type, float strength);
	void DrawSolidPolygon(Shader* shader);
	void DrawOutline(Shader* shader);
	void DrawSpriteIntoLightingBuffer(Shader* shader, int gBufferID);
	void NextType();
	//void RemoveGLData();

	// configures framebuffer
	static unsigned int s_fboID;
	static unsigned int s_polygonTextureID;
	static unsigned int s_outlinedPolygonTextureID;
	static void Init(int screenWidth, int screenHeight); 
};