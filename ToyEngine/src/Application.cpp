#include <cassert>
#include <iostream>
#include "Application.h"
#include "Light.h"

static Light light     = PointLight();
static Light& dirLight = DirectionalLight();

Application::Application()
{
    // Create a window with a rendering context attached
    m_Window        = std::make_unique<Window>("ToyGL", 2560, 1080, 4, 3);

    // Render commands handler
    m_glRenderer = std::make_unique<OpenGLRenderer>();
    m_glRenderer->init();

    // Register to observer list to get updated when events occur
    m_Window->attach(this);

    printSystemInfo();

    // Mesh test
    m_Camera = Camera(m_Window.get(), "Default", ViewFrustum(45.0f, (float)m_Window->width(), (float)m_Window->height(), 0.1f, 100.0f));
    m_Shader            .build("./data/shaders/vertex.glsl", "./data/shaders/CookTorrance.glsl");
    m_InfiniteGridShader.build("./data/shaders/VS_InfiniteGrid.glsl", "./data/shaders/FS_InfiniteGrid.glsl");


    // glTF
    model.loadFromFile("./data/models/sponza-pbr/scene.glb");
    //model.loadFromFile("./data/models/suzanne/scene.gltf");
    //model.loadFromFile("./data/models/duck/scene.gltf");
    //model.loadFromFile("./data/models/bistro/scene.glb");
    //model.loadFromFile("./data/models/lantern4k/scene.gltf");
    //model.loadFromFile("./data/models/MetalRoughSpheres/scene.gltf");

    m_lastFrameTime = (float)glfwGetTime();

    m_Minimized = false;
}

void Application::run()
{
    while(!m_Window->isClosed())
    {
        if (!m_Minimized)
        {
            glfwPollEvents();
            update(getDeltaTime());
            render();
        }
        else
        {
            glfwWaitEvents();
        }
    }
}

void Application::render()
{
    m_glRenderer->clear();
    {
        // Prepare user interface
        m_Window->getUI().beginFrame(model.textureBufferIds, dirLight, m_Camera, *m_glRenderer);
        {
            // Render geometry
            m_glRenderer->render(model, m_Shader, m_Camera, dirLight);
        }
    
        // Render user interface
        m_Window->getUI().render();
    }

    m_Window->swapBuffers();
}

void Application::update(float deltaTime)
{
    m_Camera.update(deltaTime);
}

void Application::onUpdate(Event& event)
{

    if (m_Window->getUI().wantCaptureKeyboard() || m_Window->getUI().wantCaptureMouse())
    {
        m_Camera.DisableRotation();
    }

    switch(event.getType())
    {
        case EventType::WindowResize:
            onWindowResize(static_cast<WindowResizeEvent&>(event));
        break;

        case EventType::WindowMinimize:
            onWindowMinimize(static_cast<WindowMinimizeEvent&>(event));
        break;

        // Keyboard events
        case EventType::KeyPressed:
        {
            KeyEvent& ev = static_cast<KeyEvent&>(event);
            onKeyPressed(ev);
            m_Camera.onKeyPressed(ev);
        }
        break;

        case EventType::KeyReleased:
            m_Camera.onKeyReleased(static_cast<KeyEvent&>(event));
        break;

        // Mouse events
        case EventType::MouseMove:
            m_Camera.onMouseMove(static_cast<MouseMove&>(event));
        break;
        
        case EventType::MouseScroll:
            m_Camera.onMouseScroll(static_cast<MouseScroll&>(event));
        break;

        case EventType::MouseButtonPressed:
            m_Camera.onMouseClickEvent(static_cast<MouseClick&>(event));
        break;

        case EventType::MouseButtonReleased:
            m_Camera.onMouseClickEvent(static_cast<MouseClick&>(event));
        break;
    }
}

void Application::onKeyPressed(KeyEvent& event)
{
    switch (event.m_Key)
    {
        case GLFW_KEY_R:
            m_Shader.rebuild();

        break;
    }
}

float Application::getDeltaTime()
{
    float time = (float)glfwGetTime();
    float deltaTime = time - m_lastFrameTime;
    m_lastFrameTime = time;

    return deltaTime;
}

void Application::onWindowResize(WindowResizeEvent& event)
{
    if (!m_Minimized)
    {
        const int& width = event.getWidth();
        const int& height = event.getHeight();

        // Update framebuffer size and projection matrix
        m_glRenderer->setViewport(width, height);
        m_Camera.updateAspectRatio(width / (float)height);

        render();
    }
}

void Application::onWindowMinimize(WindowMinimizeEvent& event)
{
    m_Minimized = event.m_Minimized;
}

void Application::printSystemInfo()
{
    std::cout << "ToyGL Engine \n";
    std::cout << "GPU : " << glGetString(GL_VENDOR) << " " << glGetString(GL_RENDERER) << "\n";
    std::cout << "API : OpenGL " << glGetString(GL_VERSION) << "\n\n";
}

Application::~Application()
{

}



