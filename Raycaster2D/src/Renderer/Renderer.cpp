#include "Renderer.h"
#include "Renderer/TextBlitter.h"
#include <random>

Shader Renderer::s_test_shader;
Shader Renderer::s_solid_color_shader;
Shader Renderer::s_textued_2D_quad_shader;
Shader Renderer::s_lighting_shader;
Shader Renderer::s_composite;
GBuffer Renderer::s_gBuffer;
Shader Renderer::s_blurVerticalShader;
Shader Renderer::s_blurHorizontalShader;
Shader Renderer::s_outlineShader;
Shader Renderer::s_pointLightShader;
Shader Renderer::s_multipyShader;
std::vector<BlurBuffer> Renderer::s_BlurBuffers;

int Renderer::s_renderMode = 1;
int Renderer::s_selectedLight = -1;
int Renderer::s_hoveredLight = -1;

void Renderer::Init()
{
    s_test_shader = Shader("Test Shader", "test.vert", "test.frag");
    s_solid_color_shader = Shader("Solid Color Shader", "solidColor.vert", "solidColor.frag");
    s_textued_2D_quad_shader = Shader("Textured 2D Quad Shader", "textured2DquadShader.vert", "textured2DquadShader.frag");
    s_lighting_shader = Shader("Lighting Shader", "lighting.vert", "lighting.frag");
    s_composite = Shader("Composite Shader", "composite.vert", "composite.frag");
    s_blurVerticalShader = Shader("Blur V", "blurVertical.vert", "blur.frag", "NONE");
    s_blurHorizontalShader = Shader("Blur H", "blurHorizontal.vert", "blur.frag", "NONE");
    s_outlineShader = Shader("Outline", "outline.vert", "outline.frag", "NONE");
    s_pointLightShader = Shader("PointLight", "pointlight.vert", "pointlight.frag", "NONE");
    s_multipyShader = Shader("Multiply", "multiply.vert", "multiply.frag", "NONE");

    Texture::LoadTexture("tile_dirt.png");
    Texture::LoadTexture("tile_wall.png");
    Texture::LoadTexture("tile_white.png");
    Texture::LoadTexture("CharSheet.png");
    Texture::LoadTexture("Light_Room.png");
    Texture::LoadTexture("Light_Spot_E.png");
    Texture::LoadTexture("Light_Icon.png");
    Texture::LoadTexture("Light_Icon_Select.png");
      
    s_gBuffer = GBuffer(CoreGL::s_windowWidth, CoreGL::s_windowHeight);

    // Load map
    s_BlurBuffers.push_back(BlurBuffer(SCR_WIDTH / 2, SCR_HEIGHT / 2));
    s_BlurBuffers.push_back(BlurBuffer(SCR_WIDTH / 4, SCR_HEIGHT / 4));
    s_BlurBuffers.push_back(BlurBuffer(SCR_WIDTH / 8, SCR_HEIGHT / 8));
    s_BlurBuffers.push_back(BlurBuffer(SCR_WIDTH / 16, SCR_HEIGHT / 16));
}

void Renderer::RenderFrame(Camera* camera)
{

    CheckForKeyPresses();

    RenderTiledWorld(&s_textued_2D_quad_shader);

    if (s_renderMode != RENDER_MODE_LIT_WITH_LINE_OF_SIGHT)
        RenderEdgeMap(&s_solid_color_shader);

    // player visi poly
    for (auto p : WorldMap::s_visibilityPolygonPoints) {
        float x = std::get<1>(p);
        float y = std::get<2>(p);
        DrawLineByWorldCoords(&s_solid_color_shader, x, y, Input::s_mouseWorldX, Input::s_mouseWorldY, HELL_RED);
    }

    float playerX = Input::s_mouseWorldX;
    float playerY = Input::s_mouseWorldY;

    // player bounds poly
    int lowerX = playerX - 200 * (1.777777);
    int upperX = playerX + 200 * (1.777777);
    int lowerY = playerY - 200;
    int upperY = playerY + 200;

    DrawLineByWorldCoords(&s_solid_color_shader, lowerX, lowerY, lowerX, upperY, HELL_YELLOW);
    DrawLineByWorldCoords(&s_solid_color_shader, lowerX, upperY, upperX, upperY, HELL_YELLOW);
    DrawLineByWorldCoords(&s_solid_color_shader, upperX, upperY, upperX, lowerY, HELL_YELLOW);
    DrawLineByWorldCoords(&s_solid_color_shader, upperX, lowerY, lowerX, lowerY, HELL_YELLOW);



    if (s_renderMode != RENDER_MODE_UNLIT) {
     //   LightingPass();
    }

    if (s_renderMode == RENDER_MODE_LIT_WITH_LINE_OF_SIGHT)
        RenderPlayerLineOfSight(&s_lighting_shader);

    RenderFinalImage(&s_composite);

    // UI
    if (s_renderMode == RENDER_MODE_LIT) {
        HandleEditorInput();
        RenderLightIcons(&s_textued_2D_quad_shader);
    }

    // player visi poly
    s_solid_color_shader.use();
    for (auto p : WorldMap::s_visibilityPolygonPoints) {
        float x = std::get<1>(p);
        float y = std::get<2>(p);
        DrawLineByWorldCoords(&s_solid_color_shader, x, y, Input::s_mouseWorldX, Input::s_mouseWorldY, HELL_RED);
    }
    DrawLineByWorldCoords(&s_solid_color_shader, lowerX, lowerY, lowerX, upperY, HELL_YELLOW);
    DrawLineByWorldCoords(&s_solid_color_shader, lowerX, upperY, upperX, upperY, HELL_YELLOW);
    DrawLineByWorldCoords(&s_solid_color_shader, upperX, upperY, upperX, lowerY, HELL_YELLOW);
    DrawLineByWorldCoords(&s_solid_color_shader, upperX, lowerY, lowerX, lowerY, HELL_YELLOW);


    for (glm::vec2 point : WorldMap::s_newPoints)
    {
        DrawPoint(&s_solid_color_shader, point.x, point.y, glm::vec3(0, 0, 1));

    }


    TextBlitterPass(&s_textued_2D_quad_shader);
}

void Renderer::CheckForKeyPresses()
{
    if (Input::s_keyPressed[HELL_KEY_TAB])
        s_renderMode++;
    if (s_renderMode == 3)
        s_renderMode = 0;

    if (Input::s_keyDown[HELL_KEY_1])
        s_selectedLight = 0;
    if (Input::s_keyDown[HELL_KEY_2])
        s_selectedLight = 1;
    if (Input::s_keyDown[HELL_KEY_3])
        s_selectedLight = 2;
    if (Input::s_keyDown[HELL_KEY_4])
        s_selectedLight = 3;
    if (Input::s_keyDown[HELL_KEY_5])
        s_selectedLight = 4;

    if (Input::s_keyDown[HELL_KEY_Q])
        Scene::s_lights[s_selectedLight].m_position = glm::vec2(Input::s_mouseWorldX, Input::s_mouseWorldY);

    if (Input::s_keyPressed[HELL_KEY_T])
        Scene::s_lights[s_selectedLight].NextType();

    if (Input::s_keyPressed[HELL_KEY_EQUAL])
        Scene::s_lights[s_selectedLight].m_scale++;
    if (Input::s_keyPressed[HELL_KEY_MINUS])
        Scene::s_lights[s_selectedLight].m_scale--;

    if (Input::s_keyPressed[HELL_KEY_RIGHT_BRACKET])
        Scene::s_lights[s_selectedLight].m_strength += 0.1f;
    if (Input::s_keyPressed[HELL_KEY_LEFT_BRACKET])
        Scene::s_lights[s_selectedLight].m_strength -= 0.1f;
}

void Renderer::RenderTiledWorld(Shader* shader)
{
    glBindFramebuffer(GL_FRAMEBUFFER, s_gBuffer.ID);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    glActiveTexture(GL_TEXTURE0);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s_gBuffer.ID);
    glDrawBuffer(GL_COLOR_ATTACHMENT0); 

    shader->use();
    shader->setMat4("view", Camera2D::s_viewMatrix);

    for (int x = 0; x < MAP_WIDTH; x++)
        for (int y = 0; y < MAP_HEIGHT; y++)
            DrawTile(&s_textued_2D_quad_shader, WorldMap::s_map[x][y].tile, x, y);
}

void Renderer::RenderEdgeMap(Shader* shader)
{
    shader->use();
    shader->setMat4("view", Camera2D::s_viewMatrix);
    shader->setVec3("color", glm::vec3(1));

    for (int i = 0; i < WorldMap::s_edges.size(); i++)
        DrawEdge(&WorldMap::s_edges[i]);
}

void Renderer::RenderPlayerLineOfSight(Shader* shader)
{
    // u don't know why this is required
    s_solid_color_shader.use();
    s_solid_color_shader.setMat4("view", Camera2D::s_viewMatrix);
    // u don't know why this is required

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, s_gBuffer.ID);
    glDrawBuffer(GL_COLOR_ATTACHMENT1); // Draw to gLineOfSight
    glClearTexImage(s_gBuffer.gLineOfSight, 0, GL_RGBA, GL_FLOAT, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    shader->use();
    shader->setInt("xMouse", Input::s_mouseWorldX);
    shader->setInt("yMouse", Input::s_mouseWorldY);
    shader->setInt("camX", Camera2D::s_scrollX);
    shader->setInt("camY", Camera2D::s_scrollY);
    shader->setInt("screenWidth", SCR_WIDTH);
    shader->setInt("screenHeight", SCR_HEIGHT);
    shader->setMat4("view", Camera2D::s_viewMatrix);
    shader->setVec3("color", glm::vec3(1, 1, 1));

    if (WorldMap::s_visibilityPolygonPoints.size() > 0)
    {
        // player sight
        std::vector<glm::vec2> vertices;
        glDisable(GL_CULL_FACE);
        float srcX = Input::s_mouseWorldX;
        float srcY = Input::s_mouseWorldY;
        float ndc_srcx = srcX / SCR_WIDTH * 2 - 1;
        float ndc_srcy = (SCR_HEIGHT - srcY) / SCR_HEIGHT * 2 - 1;
        vertices.push_back(glm::vec2(ndc_srcx, ndc_srcy));

        for (int i = 0; i < WorldMap::s_visibilityPolygonPoints.size(); i++) {
            float x1 = std::get<1>(WorldMap::s_visibilityPolygonPoints[i]);
            float y1 = std::get<2>(WorldMap::s_visibilityPolygonPoints[i]);
            float ndc_x = x1 / SCR_WIDTH * 2 - 1;
            float ndc_y = (SCR_HEIGHT - y1) / SCR_HEIGHT * 2 - 1;
            vertices.push_back(glm::vec2(ndc_x, ndc_y));
        }
        float x1 = std::get<1>(WorldMap::s_visibilityPolygonPoints[0]);
        float y1 = std::get<2>(WorldMap::s_visibilityPolygonPoints[0]);
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

    // Blur player visiblity
   // DownScale(s_gBuffer.ID, GL_COLOR_ATTACHMENT1, 3);
}

void Renderer::DrawTile(Shader* shader, Tile tile, int gridX, int gridY)
{
    Texture* texture; 

    if (tile == Tile::DIRT)
        texture = Texture::GetTexByName("tile_dirt.png");
    else if (tile == Tile::WALL)
        texture = Texture::GetTexByName("tile_wall.png");

    Quad2D::RenderQuad(&s_textued_2D_quad_shader, texture, gridX * GRID_SIZE, gridY * GRID_SIZE);
}

void Renderer::LightingPass()
{
    glBindFramebuffer(GL_FRAMEBUFFER, s_gBuffer.ID);
    static const float white[4] = { 1, 1, 1, 1 };
    glClearTexImage(s_gBuffer.gLineOfSight, 0, GL_RGBA, GL_FLOAT, white);
    glClearTexImage(s_gBuffer.gLighting, 0, GL_RGBA, GL_FLOAT, 0);

    for (int i = 0; i < Scene::s_lights.size(); i++)
     //   for (int i = 0; i < 2; i++)
//    for (Light& light : Scene::s_lights)
    {
        Light& light = Scene::s_lights[i];
        glBindFramebuffer(GL_FRAMEBUFFER, Light::s_fboID);
        glClearTexImage(Light::s_polygonTextureID, 0, GL_RGBA, GL_FLOAT, white);

        light.DrawSolidPolygon(&s_solid_color_shader);
        light.DrawOutline(&s_outlineShader);
        DownScale(light.s_fboID, GL_COLOR_ATTACHMENT1, 2);

        light.DrawSpriteIntoLightingBuffer(&s_pointLightShader, s_gBuffer.ID);
    }
}

void Renderer::HandleEditorInput()
{
    static int dragOffsetX = 0;
    static int dragOffsetY = 0;
    static bool dragging = false;

    // Lights
    s_hoveredLight = -1;
    for (int i = 0; i < Scene::s_lights.size(); i++) {
        int threshold = 20;
        Light* light = &Scene::s_lights[i];
        if (Input::s_mouseWorldX > light->m_position.x - threshold 
            && Input::s_mouseWorldX < light->m_position.x + threshold
            && Input::s_mouseWorldY > light->m_position.y - threshold
            && Input::s_mouseWorldY < light->m_position.y + threshold)
            s_hoveredLight = i;
    }

    // Mouse stuff
    if (Input::s_leftMousePressed)
    {
        // select new light
        if (s_hoveredLight != -1)
        {
            s_selectedLight = s_hoveredLight;
            dragOffsetX = Scene::s_lights[s_selectedLight].m_position.x - Input::s_mouseWorldX;
            dragOffsetY = Scene::s_lights[s_selectedLight].m_position.y - Input::s_mouseWorldY;
            dragging = true;
        }
        // select no light
        else
            s_selectedLight = -1;
    }
    if (!Input::s_leftMouseDown)
        dragging = false;

    // drag light
    if (dragging && s_selectedLight != -1)  {
        Scene::s_lights[s_selectedLight].m_position.x = Input::s_mouseWorldX + dragOffsetX;
        Scene::s_lights[s_selectedLight].m_position.y = Input::s_mouseWorldY + dragOffsetY;
    }

    // delete light
    if (Input::s_keyPressed[HELL_KEY_BACKSPACE] && s_selectedLight != -1) {
       // Scene::s_lights[s_selectedLight].RemoveGLData();
        Scene::s_lights.erase(Scene::s_lights.begin() + s_selectedLight);
        s_selectedLight = -1;
    }

    // new light
    if (Input::s_keyPressed[HELL_KEY_E]) {
        Scene::AddLight(Input::s_mouseWorldX, Input::s_mouseWorldY, glm::vec3(1), 2, LightType::ROOM_LIGHT, 1);
        //s_selectedLight = -1;
    }

  //  if (Input::s_leftMouseDown && Renderer::s_renderMode == RENDER_MODE_UNLIT) {
}

void Renderer::DrawEdge(Edge* edge)
{
    unsigned int VAO;
    unsigned int VBO;

    float ndc_x = edge->sX / SCR_WIDTH * 2 - 1;
    float ndc_y = (SCR_HEIGHT - edge->sY) / SCR_HEIGHT * 2 - 1;

    float ndc_x2 = edge->eX / SCR_WIDTH * 2 - 1;
    float ndc_y2 = (SCR_HEIGHT - edge->eY) / SCR_HEIGHT * 2 - 1;

    float quadVertices[] = {
        ndc_x,  ndc_y, 
        ndc_x2, ndc_y2,
    };
        
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
   
    glPointSize(5);
    glBindVertexArray(VAO);
    glEnable(GL_BLEND);
    glDrawArrays(GL_POINTS, 0, 2);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VAO);
}

void Renderer::TextBlitterPass(Shader* shader)
{
    shader->use();
    s_textued_2D_quad_shader.setMat4("view", glm::mat4(1));
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture::GetTexIDByTexName("CharSheet.png"));
    TextBlitter::BlitText(TextBlitter::s_textToBlit, false); // this congfigures internal blitter variables
    TextBlitter::DrawTextBlit(shader);                       // this draws the thing
    TextBlitter::ResetBlitter();                             // resets internal variables
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void Renderer::DrawLine(Shader* shader, Line line, glm::mat4 modelMatrix = glm::mat4(1))
{
    static unsigned int VAO = 0;
    static unsigned int VBO;
    if (VAO == 0) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
    }
    float vertices[] = {
        line.start_pos.r,  line.start_pos.g,  line.start_pos.b,  line.start_color.r,  line.start_color.g,  line.start_color.b,
        line.end_pos.r,  line.end_pos.g,  line.end_pos.b,  line.end_color.r,  line.end_color.g,  line.end_color.b,
    };
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    shader->setMat4("model", modelMatrix);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 6);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VAO);
}

void Renderer::HotLoadShaders()
{
    s_test_shader.ReloadShader();
    s_lighting_shader.ReloadShader();
    s_solid_color_shader.ReloadShader();
    s_composite.ReloadShader();
    s_textued_2D_quad_shader.ReloadShader();
    s_outlineShader.ReloadShader(); 
    s_pointLightShader.ReloadShader(); 
    s_multipyShader.ReloadShader();
}

void Renderer::DrawPoint(Shader* shader, int x, int y, glm::vec3 color)
{
    shader->setVec3("color", color);

    float ndc_x = x / SCR_WIDTH * 2 - 1;
    float ndc_y = (SCR_HEIGHT - y) / SCR_HEIGHT * 2 - 1;
    float vertices[] = {ndc_x,  ndc_y};

    unsigned int VAO;
    unsigned int VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glPointSize(5);
    glDrawArrays(GL_POINTS, 0, 1);
    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &VAO);
}

void Renderer::RenderFinalImage(Shader* shader)
{
    shader->use();
    shader->setInt("renderMode", s_renderMode);

    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(shader->ID);
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, s_gBuffer.gWorld);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, s_gBuffer.gLineOfSight);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, s_gBuffer.gLighting);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, Light::s_polygonTextureID);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, Light::s_outlinedPolygonTextureID);
    glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);

    static GLuint VAO = 0;
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

void Renderer::RenderLightIcons(Shader* shader)
{
    shader->use();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (int i = 0; i < Scene::s_lights.size(); i++)
    {
        Light* light = &Scene::s_lights[i];
        shader->setVec3("color", light->m_color);
        float x = light->m_position.x - 32;
        float y = light->m_position.y - 32;
        Texture* texture = Texture::GetTexByName("Light_Icon.png");
        Quad2D::RenderQuad(shader, texture, x, y);

        if (i == s_selectedLight) {
            Texture* texture = Texture::GetTexByName("Light_Icon_Select.png");
            Quad2D::RenderQuad(shader, texture, x, y);
        }
    }   
}

void Renderer::DownScale(int sourceFBO, int sourceColorAttachment, int levels)
{
    Shader* blurHorizontalShader = &s_blurHorizontalShader;
    Shader* blurVerticalShader = &s_blurVerticalShader;

    glDisable(GL_DEPTH_TEST);

    // Source
    glBindFramebuffer(GL_READ_FRAMEBUFFER, sourceFBO);
    glReadBuffer(sourceColorAttachment);
    int factor = 2;

    // Destination
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s_BlurBuffers[0].ID);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    // Blit
    glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH / factor, SCR_HEIGHT / factor, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    // Blur horizontal
   glViewport(0, 0, SCR_WIDTH / factor, SCR_HEIGHT / factor);
    glBindFramebuffer(GL_FRAMEBUFFER, s_BlurBuffers[0].ID);
    
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glDrawBuffer(GL_COLOR_ATTACHMENT1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, s_BlurBuffers[0].textureA);
    blurHorizontalShader->use();
    blurHorizontalShader->setFloat("targetWidth", SCR_WIDTH / factor);
    Quad2D::RenderFullScreenQuad(blurHorizontalShader);
    // Blur vertical
    glViewport(0, 0, SCR_WIDTH / factor, SCR_HEIGHT / factor);
    glBindFramebuffer(GL_FRAMEBUFFER, s_BlurBuffers[0].ID);
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, s_BlurBuffers[0].textureB);
    blurVerticalShader->use();
    blurVerticalShader->setFloat("targetHeight", SCR_HEIGHT / factor);
    Quad2D::RenderFullScreenQuad(blurVerticalShader);

    // second downscale //
    for (int i = 1; i < levels; i++)
    {
        factor *= 2;
        // Source
        glBindFramebuffer(GL_READ_FRAMEBUFFER, s_BlurBuffers[i - 1].ID);
        glReadBuffer(GL_COLOR_ATTACHMENT0);

        // Destination
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, s_BlurBuffers[i].ID);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);

        // Blit
        glBlitFramebuffer(0, 0, SCR_WIDTH / (factor / 2), SCR_HEIGHT / (factor / 2), 0, 0, SCR_WIDTH / factor, SCR_HEIGHT / factor, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        // Blur horizontal
        glViewport(0, 0, SCR_WIDTH / factor, SCR_HEIGHT / factor);
        glBindFramebuffer(GL_FRAMEBUFFER, s_BlurBuffers[i].ID);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glDrawBuffer(GL_COLOR_ATTACHMENT1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, s_BlurBuffers[i].textureA);
        blurHorizontalShader->use();
        blurHorizontalShader->setFloat("targetWidth", SCR_WIDTH / factor);
        Quad2D::RenderFullScreenQuad(blurHorizontalShader);

        // Blur vertical
        glViewport(0, 0, SCR_WIDTH / factor, SCR_HEIGHT / factor);
        glBindFramebuffer(GL_FRAMEBUFFER, s_BlurBuffers[i].ID);
        glReadBuffer(GL_COLOR_ATTACHMENT1);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, s_BlurBuffers[i].textureB);
        blurVerticalShader->use();
        blurVerticalShader->setFloat("targetHeight", SCR_WIDTH / factor);
        Quad2D::RenderFullScreenQuad(blurVerticalShader);


        // blit back to original
        if (i == levels - 1) {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, s_BlurBuffers[i].textureA);
            glReadBuffer(GL_COLOR_ATTACHMENT0);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, sourceFBO);
            glDrawBuffer(sourceColorAttachment);
            glBlitFramebuffer(0, 0, SCR_WIDTH / factor, SCR_HEIGHT / factor, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        }
    }
    // return the viewport to it's orginal fucking size. this has stumped you for so long twice now. adding this will stop it.
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}

void Renderer::DrawLineByWorldCoords(Shader* shader, int x, int y, int x2, int y2, glm::vec3 color)
{
    shader->setVec3("color", color);

    float ndc_x = x / SCR_WIDTH * 2 - 1;
    float ndc_y = (SCR_HEIGHT - y) / SCR_HEIGHT * 2 - 1;
    float ndc_x2 = x2 / SCR_WIDTH * 2 - 1;
    float ndc_y2 = (SCR_HEIGHT - y2) / SCR_HEIGHT * 2 - 1;
    float vertices[] = { ndc_x,  ndc_y, ndc_x2, ndc_y2 };

    static unsigned int VAO = 0;
    static unsigned int VBO;

    if (VAO == 0) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
    }
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glPointSize(5);
    glDrawArrays(GL_POINTS, 0, 2);
    glDrawArrays(GL_LINES, 0, 2);
}