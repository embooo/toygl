#include <cassert>
#include <iostream>
#include "Application.h"

Application::Application()
{
    // Create a window with a rendering context attached
    m_Window = std::make_unique<Window>("ToyGL", 1280, 720, 4, 3);
    // Create an object that will issue the render commands
    m_glRenderer = std::make_unique<OpenGLRenderer>();
    m_glRenderer->init();

    // Info log
    printSystemInfo();

    // Mesh test
    m_Camera = Camera("DefaultCam", m_Window.get(), ViewFrustum(45.0f, (float)m_Window->width(), (float)m_Window->height(), 0.1f, 100.0f), glm::vec3(0.0f, 0.0f, 5.0f));
    m_Shader.build("./data/shaders/vertex.glsl", "./data/shaders/fragmentEnv.glsl");

    // Register to observer list to get updated when events occur
    m_Window->attach(this);
    m_Window->attach(&m_Camera);

    m_Shader.use();
    m_Shader.setMat4("model", glm::identity<glm::mat4>());
    m_Shader.setMat4("view", m_Camera.getViewMat());
    m_Shader.setMat4("projection", m_Camera.getProjectionMat());

    // glTF
    model.loadFromFile("./data/models/Sponza/Sponza.gltf");

    m_lastFrameTime = (float)glfwGetTime();
}

void Application::run()
{
    while(!m_Window->isClosed())
    {
        // Compute delta time
        update( getDeltaTime() );
        
        render();
    }
}

void Application::render()
{
    m_glRenderer->clear();

    m_Shader.use();
    
    m_Shader.setMat4("model", glm::identity<glm::mat4>());
    m_Shader.setMat4("view", m_Camera.getViewMat());
    m_Shader.setMat4("projection", m_Camera.getProjectionMat());

    m_Shader.setFloat4("cLight", glm::vec4(0.9, 0.9, 0.9, 1.0));
    m_Shader.setFloat3("pLight", glm::vec3(0.0, 0.0, 0.0));
    m_Shader.setFloat3("pCam", m_Camera.pos());

    model.draw(m_Shader);

    m_Window->swapBuffers();
}

void Application::update(float deltaTime)
{
    m_Camera.update(deltaTime);
    glfwPollEvents();
}

void Application::onUpdate(Event& event)
{
    switch(event.getType())
    {
        case EventType::WindowResize:
            onWindowResize(static_cast<WindowResizeEvent&>(event)); 
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
    m_glRenderer->setViewport(event.getWidth(), event.getHeight());
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



