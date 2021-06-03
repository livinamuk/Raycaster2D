#include "Light.h"
#include "Camera2D.h"
#include "WorldMap.h"
#include "CoreGL.h"

unsigned int Light::s_fboID;
unsigned int Light::s_polygonTextureID;
unsigned int Light::s_outlinedPolygonTextureID;

Light::Light(glm::vec2 position, glm::vec3 lightColor, float scale, LightType type, float strength)
{
	m_position = position;
    m_color = lightColor;
    m_scale = scale;
    m_type = type;
    m_strength = strength;


}

void Light::Init(int screenWidth, int screenHeight)
{
    glGenFramebuffers(1, &s_fboID);
    glGenTextures(1, &s_polygonTextureID);
    glGenTextures(1, &s_outlinedPolygonTextureID);

	glBindFramebuffer(GL_FRAMEBUFFER, s_fboID);

    glBindTexture(GL_TEXTURE_2D, s_polygonTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, s_polygonTextureID, 0);

    glBindTexture(GL_TEXTURE_2D, s_outlinedPolygonTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, s_outlinedPolygonTextureID, 0);
}

/*
void Light::RemoveGLData()
{
    glDeleteFramebuffers(1, &s_fboID);
    glDeleteBuffers(1, &s_polygonTextureID);
    glDeleteBuffers(1, &s_outlinedPolygonTextureID);
}*/

void Light::DrawSolidPolygon(Shader* shader)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s_fboID);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glClearTexImage(s_polygonTextureID, 0, GL_RGBA, GL_FLOAT, 0);

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    glActiveTexture(GL_TEXTURE0);

    // Light
    float srcX = m_position.x;
    float srcY = m_position.y;

    float testX = srcX + Camera2D::s_scrollX - SCR_WIDTH / 2;
    float testY = srcY + Camera2D::s_scrollY - SCR_HEIGHT / 2;

    std::vector<std::tuple<float, float, float>> lightVisibilityPolygonPoints = WorldMap::CalculateVisibilityPolygon(srcX, srcY, 1.0f);;

    std::vector<glm::vec2> vertices;
    glDisable(GL_CULL_FACE);

    shader->use();
    shader->setVec3("color", glm::vec3(1));

    float ndc_srcx = srcX / SCR_WIDTH * 2 - 1;
    float ndc_srcy = (SCR_HEIGHT - srcY) / SCR_HEIGHT * 2 - 1;
    vertices.push_back(glm::vec2(ndc_srcx, ndc_srcy));

    for (int i = 0; i < lightVisibilityPolygonPoints.size(); i++) {
        float x1 = std::get<1>(lightVisibilityPolygonPoints[i]);
        float y1 = std::get<2>(lightVisibilityPolygonPoints[i]);
        float ndc_x = x1 / SCR_WIDTH * 2 - 1;
        float ndc_y = (SCR_HEIGHT - y1) / SCR_HEIGHT * 2 - 1;
        vertices.push_back(glm::vec2(ndc_x, ndc_y));
    }
    float x1 = std::get<1>(lightVisibilityPolygonPoints[0]);
    float y1 = std::get<2>(lightVisibilityPolygonPoints[0]);
    float ndc_x = x1 / SCR_WIDTH * 2 - 1;
    float ndc_y = (SCR_HEIGHT - y1) / SCR_HEIGHT * 2 - 1;
    vertices.push_back(glm::vec2(ndc_x, ndc_y));

    unsigned int VAO;
    unsigned int VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

    glPointSize(5);
    glBindVertexArray(VAO);
    glEnable(GL_BLEND);
    glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size());
    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VAO);
}

void Light::DrawOutline(Shader* shader)
{
    int sourceFBO = s_fboID;
    int sourceTextureID = s_polygonTextureID;
    int destinationFBO = s_fboID;
    int sourceColorAttachment = GL_COLOR_ATTACHMENT0;
    int destinationColorAttachment = GL_COLOR_ATTACHMENT1;
    int outlineSize = -1;

    shader->use();
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, sourceFBO);
    glReadBuffer(sourceColorAttachment);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sourceTextureID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, destinationColorAttachment);
    glDrawBuffer(destinationColorAttachment);

    Quad2D::RenderFullScreenQuad(shader);    
}

void Light::DrawSpriteIntoLightingBuffer(Shader* shader, int gBufferID)
{
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

   // glBindFramebuffer(GL_READ_FRAMEBUFFER, s_fboID);
    //glReadBuffer(GL_COLOR_ATTACHMENT1);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBufferID);
    glDrawBuffer(GL_COLOR_ATTACHMENT2);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE); // additive blending for multiple lights. you always forget how to do this.

    shader->use();
    shader->setVec3("color", m_color);
    shader->setFloat("strength", m_strength);
    shader->setMat4("view", Camera2D::s_viewMatrix);
    shader->setInt("xMouse", Input::s_mouseWorldX);
    shader->setInt("yMouse", Input::s_mouseWorldY);
    shader->setInt("camX", Camera2D::s_scrollX);
    shader->setInt("camY", Camera2D::s_scrollY);
    shader->setInt("screenWidth", SCR_WIDTH);
    shader->setInt("screenHeight", SCR_HEIGHT);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, s_outlinedPolygonTextureID);
    glActiveTexture(GL_TEXTURE2); // for testing
    glBindTexture(GL_TEXTURE_2D, s_polygonTextureID);

    Texture* texture;

    if (m_type == LightType::ROOM_LIGHT)
        texture = Texture::GetTexByName("Light_Room.png");
    else if (m_type == LightType::SPOT_LIGHT_E)
        texture = Texture::GetTexByName("Light_Spot_E.png");

    Quad2D::RenderQuad(shader, texture, m_position.x - (texture->width * m_scale / 2), m_position.y - (texture->height * m_scale / 2), m_scale);
}

void Light::NextType()
{

    if (m_type == LightType::ROOM_LIGHT) {
        m_type = LightType::SPOT_LIGHT_E;
        return;
    }
    if (m_type == LightType::SPOT_LIGHT_E) {
        m_type = LightType::ROOM_LIGHT;
        return;
    }
}