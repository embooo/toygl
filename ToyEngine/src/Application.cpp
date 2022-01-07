#include <cassert>
#include <iostream>
#include "Application.h"
#include "Light.h"

static Light light;

Application::Application()
{
    // Create a window with a rendering context attached
    m_Window        = std::make_unique<Window>("ToyGL", 2560, 1080, 4, 3);
    m_UserInterface = std::make_unique<UserInterface>();

    // Render commands handler
    m_glRenderer = std::make_unique<OpenGLRenderer>();
    m_glRenderer->init();

    // Register to observer list to get updated when events occur
    m_Window->attach(this);
    m_Window->attach(&m_Camera);
    m_Window->attach(m_UserInterface.get());

    // ImGui user interface
    m_UserInterface->init(*m_Window);

    printSystemInfo();

    // Mesh test
    m_Camera = Camera("DefaultCam", m_Window.get(), ViewFrustum(45.0f, (float)m_Window->width(), (float)m_Window->height(), 0.1f, 100.0f), glm::vec3(0.0f, 0.0f, 5.0f));
    m_Shader            .build("./data/shaders/vertex.glsl", "./data/shaders/CookTorrance.glsl");
    m_InfiniteGridShader.build("./data/shaders/VS_InfiniteGrid.glsl", "./data/shaders/FS_InfiniteGrid.glsl");


    // glTF
    model.loadFromFile("./data/models/sponza-pbr/sponza.glb");
    //model.loadFromFile("./data/models/gas_stations_fixed/scene.gltf");
    m_Camera.lookAt(model.bbox.min, model.bbox.max);

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
        m_UserInterface->beginFrame(light);

        {
            // Render geometry
            m_Shader.use();

            m_Shader.setMat4("model", glm::identity<glm::mat4>() * glm::scale(glm::vec3(10, 10, 10)));
            m_Shader.setMat4("view", m_Camera.getViewMat());
            m_Shader.setMat4("projection", m_Camera.getProjectionMat());

            m_Shader.setFloat3("cameraPos", m_Camera.pos());
            m_Shader.setFloat3("lightPos",  light.pos());
            m_Shader.setFloat4("lightColor",  light.color());
            
            m_Shader.setFloat("lightRadius",((PointLight&)light).radius());
            
            model.draw(m_Shader);
        }
    
        // Render user interface
        m_UserInterface->render();
    }

    m_Window->swapBuffers();
}

void Application::update(float deltaTime)
{
    m_Camera.update(deltaTime);
}

void Application::onUpdate(Event& event)
{
    switch(event.getType())
    {
        case EventType::WindowResize:
            onWindowResize(static_cast<WindowResizeEvent&>(event)); 
            break;

        case EventType::WindowMinimize:
            onWindowMinimize(static_cast<WindowMinimizeEvent&>(event));
            break;

        case EventType::KeyPressed:
            onKeyPressed(static_cast<KeyEvent&>(event));
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



