#pragma once
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Core/Camera.h"
#include "Core/CoreGL.h"
#include "Renderer/GBuffer.h"

class Renderer
{
	
public: // functions
	static void Init();
	static void RenderFrame(Camera* camera);
	static void DrawGrid(Shader* shader);
	static void RenderFinalImage();
	static void TextBlitterPass(Shader* shader);
	static void DrawLine(Shader* shader, Line line, glm::mat4 modelMatrix);
	static void HotLoadShaders();
	static void DrawPoint(Shader* shader, glm::vec3 position, glm::vec3 color);

public: // variables
	static Shader s_test_shader;
	static Shader s_solid_color_shader;
	static Shader s_textued_2D_quad_shader;
	static Shader s_skinned_model_shader;
	static GBuffer s_gBuffer;
};

