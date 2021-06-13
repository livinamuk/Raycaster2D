#pragma once
#include "Header.h"

class GBuffer
{
public:
	GBuffer(int width, int height);
	GBuffer();
	~GBuffer();

	unsigned int ID;
	unsigned int gWorld, gLineOfSight, gLighting, gEditor, gLightTemp;

	void Configure(int width, int height);
};
