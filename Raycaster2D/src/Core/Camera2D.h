#pragma once
#include "Header.h"

class Camera2D
{
public: // functions
	static void AdjustProjection();

public: // variables
	static int s_scrollX;
	static int s_scrollY;
	static glm::mat4 s_viewMatrix;
};