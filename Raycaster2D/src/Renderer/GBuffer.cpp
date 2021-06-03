#include "GBuffer.h"

GBuffer::GBuffer(int width, int height)
{
	glGenFramebuffers(1, &ID);
	glGenTextures(1, &gWorld);
	glGenTextures(1, &gLighting);
	glGenTextures(1, &gLineOfSight);
	Configure(width, height); 
}

GBuffer::GBuffer()
{
}

GBuffer::~GBuffer()
{
}

void GBuffer::Configure(int width, int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	glBindTexture(GL_TEXTURE_2D, gWorld);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gWorld, 0);

	glBindTexture(GL_TEXTURE_2D, gLineOfSight);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gLineOfSight, 0);

	glBindTexture(GL_TEXTURE_2D, gLighting);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gLighting, 0);
	
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (fboStatus == 36053)
		std::cout << "G Buffer: " << "COMPLETE\n";
	if (fboStatus == 36054)
		std::cout << "G Buffer: " << "INCOMPLETE ATTACHMENT\n";
	if (fboStatus == 36057)
		std::cout << "G Buffer: " << "INCOMPLETE DIMENSIONS\n";
	if (fboStatus == 36055)
		std::cout << "G Buffer: " << "MISSING ATTACHMENT\n";
	if (fboStatus == 36061)
		std::cout << "G Buffer: " << "UNSPORTED\n";

	auto glstatus = glGetError();
	if (glstatus != GL_NO_ERROR)
		std::cout << "Error in GL call: " << glstatus << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}