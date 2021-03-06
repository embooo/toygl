#include "PCH.h"

#include "UserInterface.h"
#include "Window.h"

#include "glm/gtc/type_ptr.hpp"

UserInterface::UserInterface()
{
}

void UserInterface::onUpdate(Event& event)
{
}

void UserInterface::init(Window& window)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    const char* glsl_version = "#version 130";
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    m_pWindow = &window;
    ImGui_ImplGlfw_InitForOpenGL(window.getRawPtr(), true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    GLTexture tex;
    tex.create("data/models/Sponza/11474523244911310074.jpg");
    resources.textures.push_back(tex);

    resources.framebuffer.create(128, 128, GL_RGBA8, GL_RGBA8);
}

void UserInterface::render(std::vector<GLuint>& tex, Light& light, const Camera& camera,  OpenGLRenderer& renderer)
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        ImGui::Begin("Panel");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Checkbox("Shader Parameters", &show_another_window);
        ImGui::Checkbox("Framebuffer view", &show_framebuffer_window);

        if (ImGui::Checkbox("VSync", &enable_vsync))
        {
            m_pWindow->toggleVsync();
        }

        ImGui::Checkbox("Multi Draw Indirect", &renderer.m_bEnableMultiDrawIndirect);
        
        ImGui::Text("Num. draw calls  %d", renderer.statistics.numDrawCalls);
        ImGui::Text("Num. indices  %d",    renderer.statistics.numIndices);
        ImGui::Text("Num. vertices  %d",   renderer.statistics.numVertices);

        const float* pos = glm::value_ptr(camera.pos());
        ImGui::Text("Camera world position (%.1f, %.1f, %.1f)", pos[0], pos[1], pos[2]);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);


        for (GLTexture& texture : resources.textures)
        {
            texture.bind();
            ImGui::ImageButton((void*)texture.getId(), { 128, 128 });
        }




        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Shader Parameters",   &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Checkbox("Normal Mapping",   &renderer.m_bEnableNormalMap);
        ImGui::Checkbox("Gamma Correction", &renderer.m_bEnableGammaCorrection);

        ImGui::Text("Light");               // Display some text (you can use a format strings too)

        ImGui::ColorEdit4("Light Color", glm::value_ptr(light.color()));

        if (light.type() == LightType::POINT_LIGHT)
        {
            ImGui::DragFloat3("Light Position", glm::value_ptr(light.pos()), 0.1f, -1000.0f, 1000.0f);
            ImGui::DragFloat("Light Radius", &((PointLight&)light).radius(), 0.01f, 0.0f, 100.0f);
        }

        if (light.type() == LightType::DIR_LIGHT)
        {
            ImGui::DragFloat3("Light Direction", glm::value_ptr(((DirectionalLight&)light).dir()), 0.01f, -1000.0f, 1000.0f);
        }

        ImGui::DragFloat3("Translation", glm::value_ptr(renderer.translation), 0.1f, -1000.0f, 1000.0f);
        ImGui::DragFloat3("Scale",       glm::value_ptr(renderer.scale),       0.1f, -1000.0f, 1000.0f);

        if (ImGui::Button("Close"))
            show_another_window = false;

        ImGui::End();
    }


    if (show_framebuffer_window)
    {
        ImGui::Begin("Framebuffer view", &show_framebuffer_window); 

        resources.framebuffer.get(FBAttach::Color).bind();
        ImGui::ImageButton((void*)resources.framebuffer.get(FBAttach::Color).getId(), { 128, 128 });
        resources.framebuffer.get(FBAttach::Depth).bind();
        ImGui::ImageButton((void*)resources.framebuffer.get(FBAttach::Depth).getId(), {128, 128});

        ImGui::End();
    }

    // Render
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UserInterface::render()
{

}

bool UserInterface::wantCaptureMouse() const
{
    return io->WantCaptureMouse;
}

bool UserInterface::wantCaptureKeyboard() const
{
    return io->WantCaptureKeyboard;
}

void UserInterface::terminate()
{
    ImGui_ImplOpenGL3_Shutdown();
    //ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

UserInterface::~UserInterface()
{
    std::cerr << "Destructor : UserInterface\n";
    for (GLTexture& texture : resources.textures)
        texture.release();

    resources.framebuffer.release();

    terminate();
}
