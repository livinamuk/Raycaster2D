#pragma once
#include "Header.h"
#include "Renderer/Texture.h"


class Light
{
public: // fields
	glm::vec2 m_position;
	glm::vec3 m_color; 
	int m_type;
	int m_rotate;
	float m_strength;
	float m_scale;
	std::vector<std::tuple<float, float, float>> m_lightVisibilityPolygonPoints;
	Texture* m_texture;

public: // functions
	//Light();
	Light(glm::vec2 position, glm::vec3 lightColor, float scale, int type, float strength, int rotate);
	void DrawSolidPolygon(Shader* shader);
	void DrawOutline(Shader* shader);
	void DrawShadowCastingLight(Shader* shader, int gBufferID);
	void NextType();
	void RotateLight();
	void SetTextureFromType();
	bool IsInScreenBounds();
	bool IsShadowCasting();
	//void RemoveGLData();

	// configures framebuffer
	static unsigned int s_fboID;
	static unsigned int s_accumulateLightingTexture;
	static void Init(int screenWidth, int screenHeight); 
};