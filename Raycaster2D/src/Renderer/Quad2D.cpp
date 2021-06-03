#include "Quad2D.h"
#include <algorithm>

void Quad2D::RenderFullScreenQuad(Shader* shader)
{
	static unsigned int VAO = 0;
	if (VAO == 0)
	{
		float quadVertices[] = {
			// positions         texcoords
			-1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
				1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
				1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
		};
		unsigned int VBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}

	shader->setMat4("model", glm::mat4(1));
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Quad2D::RenderQuad(Shader* shader, Texture* texture, int xScreenCoord, int yScreenCoord, float scale)
{
	static unsigned int VAO2 = 0;
	if (VAO2 == 0)
	{
		float quadVertices[] = {
			// positions         texcoords
			0.0f,  1.0f, 0.0f,  0.0f, 0.0f,
			0.0f,  0.0f, 0.0f,  0.0f, 1.0f,
			1.0f,  1.0f, 0.0f,  1.0f, 0.0f,
			1.0f,  0.0f, 0.0f,  1.0f, 1.0f,
		};
		unsigned int VBO2;
		glGenVertexArrays(1, &VAO2);
		glGenBuffers(1, &VBO2);
		glBindVertexArray(VAO2);
		glBindBuffer(GL_ARRAY_BUFFER, VBO2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}

	if (texture == nullptr)
		return;

	float width = (texture->width / SCR_WIDTH) * 2;
	float height = (texture->height / SCR_HEIGHT) * 2;
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	float x = (xScreenCoord / SCR_WIDTH) * 2 - 1;;
	float y = ((SCR_HEIGHT - yScreenCoord - texture->height * scale) / SCR_HEIGHT) * 2 - 1;

	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(width * scale, height * scale, 1));
	shader->setMat4("model", modelMatrix);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->ID);

	glBindVertexArray(VAO2); 
	glEnable(GL_BLEND);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	shader->setVec3("color", glm::vec3(1));
}

