#include <cassert>
#include <iostream>
#include "Application.h"


Application::Application()
{
    // Create a window with a rendering context attached
    m_Window = std::make_unique<Window>("ToyGL", 1280, 720, 3, 3);
    // Create an object that will issue the render commands
    m_glRenderer = std::make_unique<OpenGLRenderer>();


    // Mesh test
    m_Camera = Camera("DefaultCam", ViewFrustum(45.0f, m_Window->width()/m_Window->height()));
    m_Shader = Shader("./data/shaders/vertex.glsl", "./data/shaders/fragment.glsl");

    // Register to observer list to get updated when events occur
    m_Window->attach(this);
    m_Window->attach(&m_Camera);

    m_Shader.use();
    m_Shader.setMat4("model", glm::identity<glm::mat4>());
    m_Shader.setMat4("view", m_Camera.getViewMat());
    m_Shader.setMat4("projection", m_Camera.getProjectionMat());

    std::vector<Vertex> cubeVertices
    {
        Vertex(glm::vec3(-0.5f, -0.5f, 0.5f)).addColor(glm::vec3(-0.5f, -0.5f, 0.5f)),
        Vertex(glm::vec3(0.5f, -0.5f, 0.5f)).addColor(glm::vec3(0.5f, -0.5f, 0.5f)),
        Vertex(glm::vec3(0.5f, 0.5f, 0.5f)).addColor(glm::vec3(0.5f, 0.5f, 0.5f)),
        Vertex(glm::vec3(-0.5f, 0.5f, 0.5f)).addColor(glm::vec3(-0.5f, 0.5f, 0.5f)),
        Vertex(glm::vec3(-0.5f, -0.5f, -0.5f)).addColor(glm::vec3(-0.5f, -0.5f, -0.5f)),
        Vertex(glm::vec3(0.5f, -0.5f, -0.5f)).addColor(glm::vec3(0.5f, -0.5f, -0.5f)),
        Vertex(glm::vec3(0.5f, 0.5f, -0.5f)).addColor(glm::vec3(0.5f, 0.5f, -0.5f)),
        Vertex(glm::vec3(-0.5f, 0.5f, -0.5f)).addColor(glm::vec3(-0.5f, 0.5f, -0.5f))
    };
    std::vector<GLuint> cubeIndices = {0, 1, 2,  0, 2, 3, /**/ 4,5,6, 4,6,7, /**/4,0,3, 4,3,7, /**/5,1,2, 5,2,6, /**/ 0,1,5, 0,5,4, /**/ 3,2,6, 3,6,7};

    m_Cube = Mesh(cubeVertices, cubeIndices );

    m_lastFrameTime = (float)glfwGetTime();
}

void Application::run()
{
    while(!m_Window->isClosed())
    {
        render();
        // Compute delta time
        float time = (float)glfwGetTime();
        float deltaTime = time - m_lastFrameTime;
        m_lastFrameTime = time;

        update(deltaTime);
    }
}

void Application::render()
{
    m_glRenderer->init();
    //m_glRenderer->setViewport(m_Window->width(), m_Window->height());
    m_glRenderer->clear();

    m_Shader.use();
    m_Shader.setMat4("model", glm::identity<glm::mat4>());
    m_Shader.setMat4("view", m_Camera.getViewMat());
    m_Shader.setMat4("projection", m_Camera.getProjectionMat());

    m_Cube.draw();

    m_Window->swapBuffers();
    glfwPollEvents();
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
    }
}

void Application::onWindowResize(WindowResizeEvent& event)
{
    m_glRenderer->setViewport(event.getWidth(), event.getHeight());
}

void Application::printSystemInfo()
{
    std::cout << "ToyGL Engine \n";
    std::cout << "GPU : " << glGetString(GL_VENDOR) << " " << glGetString(GL_RENDERER) << "\n";
    std::cout << "API : OpenGL " << glGetString(GL_VERSION) << "\n";
}

Application::~Application()
{

}



