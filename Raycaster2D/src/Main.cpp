#include "Header.h"
#include "Core/CoreGL.h"
#include "Core/Input.h"
#include "Core/Camera.h"
#include "Helpers/Util.h"
#include "Renderer/Texture.h"
#include "Renderer/TextBlitter.h"
#include "Renderer/Renderer.h"

/////////////////////
//   Entry Point   //

int main()
{
    CoreGL::InitGL(SCR_WIDTH, SCR_HEIGHT);
    Renderer::Init();
    
    Input::s_showCursor = true;

    Camera camera;
    CoreGL::SetCamera(&camera);

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

        // Hotload shader?
        if (Input::s_keyPressed[HELL_KEY_H])
            Renderer::HotLoadShaders();

        TextBlitter::BlitLine("Raycast 2D");

        // RENDER FRAME
        Renderer::RenderFrame(&camera);
        CoreGL::SwapBuffersAndPollEvents();
    }

    CoreGL::Terminate();
    return 0;
}
