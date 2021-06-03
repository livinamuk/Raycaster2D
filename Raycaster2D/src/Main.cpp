#include "Header.h"
#include "Core/CoreGL.h"
#include "Core/Input.h"
#include "Core/Camera.h"
#include "Core/WorldMap.h"
#include "Helpers/Util.h"
#include "Renderer/Texture.h"
#include "Renderer/TextBlitter.h"
#include "Renderer/Renderer.h"
#include "Core/Camera2D.h"
#include "Core/Scene.h"

/////////////////////
//   Entry Point   //


int main()
{
    CoreGL::InitGL(SCR_WIDTH, SCR_HEIGHT);

    Renderer::Init();
    Light::Init(SCR_WIDTH, SCR_HEIGHT);

    Input::s_showCursor = true;

    Camera camera;
    CoreGL::SetCamera(&camera);

    WorldMap::LoadMap();

    // Add lights
    Scene::AddLight(816, 433, HELL_RED, 5, LightType::ROOM_LIGHT, 1);
    Scene::AddLight(620, 730, HELL_WHITE, 10, LightType::ROOM_LIGHT, 1);
    /* Scene::AddLight(611, 930, HELL_WHITE, 2, LightType::SPOT_LIGHT_E, 1);

    Scene::AddLight(235, 370, HELL_WHITE, 2, LightType::SPOT_LIGHT_E, 1);
    Scene::AddLight(235, 370, HELL_WHITE, 2, LightType::SPOT_LIGHT_E, 1);
    Scene::AddLight(235, 370, HELL_WHITE, 2, LightType::SPOT_LIGHT_E, 1);
    Scene::AddLight(235, 370, HELL_WHITE, 2, LightType::SPOT_LIGHT_E, 1);
    Scene::AddLight(235, 370, HELL_WHITE, 2, LightType::SPOT_LIGHT_E, 1);
    Scene::AddLight(235, 370, HELL_WHITE, 2, LightType::SPOT_LIGHT_E, 1);*/

    float lastFrame = CoreGL::GetGLTime();

    ///////////////////////
    //   Main game loop  //

    while (CoreGL::IsRunning() && !Input::s_keyDown[HELL_KEY_ESCAPE])
    {
        float deltaTime = CoreGL::GetGLTime() - lastFrame;
        lastFrame = CoreGL::GetGLTime();
        camera.Update(deltaTime);

        // Update OpenGL and get keyboard state
        CoreGL::OnUpdate();
        CoreGL::ProcessInput();
        Input::HandleKeypresses();
        WorldMap::Update();
        CoreGL::SetVSync(true);
        Camera2D::AdjustProjection();

        Input::s_mouseWorldX = Input::s_mouseX + Camera2D::s_scrollX - SCR_WIDTH / 2;
        Input::s_mouseWorldY = Input::s_mouseY + Camera2D::s_scrollY - SCR_HEIGHT / 2;

        int speed = 3;

        if (Input::s_keyDown[HELL_KEY_D])
            Camera2D::s_scrollX+= speed;
        if (Input::s_keyDown[HELL_KEY_A])
            Camera2D::s_scrollX-= speed;
        if (Input::s_keyDown[HELL_KEY_S])
            Camera2D::s_scrollY+= speed;
        if (Input::s_keyDown[HELL_KEY_W])
            Camera2D::s_scrollY-= speed;

        // Hotload shader?
        if (Input::s_keyPressed[HELL_KEY_H])
            Renderer::HotLoadShaders();

        if (Input::s_keyPressed[HELL_KEY_L]) {
            WorldMap::SaveMap();
            Scene::SaveScene("res/scene.txt");
        }

        TextBlitter::BlitLine("Raycast 2D");
        TextBlitter::BlitLine("FPS: " + std::to_string(CoreGL::GetFPS()));
        TextBlitter::BlitLine("X: " + std::to_string(Input::s_mouseX));
        TextBlitter::BlitLine("Y: " + std::to_string(Input::s_mouseY));
        TextBlitter::BlitLine("GridX: " + std::to_string(Input::s_gridX));
        TextBlitter::BlitLine("GridY: " + std::to_string(Input::s_gridY));
        TextBlitter::BlitLine("WorldX: " + std::to_string(Input::s_mouseWorldX));
        TextBlitter::BlitLine("WorldY: " + std::to_string(Input::s_mouseWorldY));
        TextBlitter::BlitLine("CamX: " + std::to_string(Camera2D::s_scrollX));
        TextBlitter::BlitLine("CamY: " + std::to_string(Camera2D::s_scrollY));
        TextBlitter::BlitLine("Rays: " + std::to_string(WorldMap::s_visibilityPolygonPoints.size()));
        TextBlitter::BlitLine("Unique Edges: " + std::to_string(WorldMap::s_edges.size()));
        TextBlitter::BlitLine("Unique Points: " + std::to_string(WorldMap::s_uniqueEdgePoints.size()));
        TextBlitter::BlitLine("Selected Light: " + std::to_string(Renderer::s_selectedLight));
        TextBlitter::BlitLine("Hovered Light: " + std::to_string(Renderer::s_hoveredLight));
        TextBlitter::BlitLine("Render Mode: " + std::to_string(Renderer::s_renderMode));
        TextBlitter::BlitLine("Ray Count: " + std::to_string(WorldMap::s_rayCount));

        
        // RENDER FRAME
        Renderer::RenderFrame(&camera);
        CoreGL::SwapBuffersAndPollEvents();
    }

    CoreGL::Terminate();
    return 0;
}
