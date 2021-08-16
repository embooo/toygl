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
    m_Camera = Camera("DefaultCam", m_Window.get(), ViewFrustum(45.0f, (float)m_Window->width(), (float)m_Window->height(), 0.1f, 100.0f), glm::vec3(0.0f, 0.0f, -2.0f));
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
        Vertex(glm::vec3(-1.0, -1.0,  1.0f)).addColor(glm::vec3(-0.5f, -0.5f, -0.5f )), // 0
        Vertex(glm::vec3( 1.0, -1.0,  1.0f)).addColor(glm::vec3( 0.5f, -0.5f, -0.5f )), // 1
        Vertex(glm::vec3( 1.0,  1.0,  1.0) ).addColor(glm::vec3( 0.5f, -0.5f, 0.5f  )), // 2
        Vertex(glm::vec3(-1.0,  1.0,  1.0) ).addColor(glm::vec3(-0.5f, -0.5f, 0.5f  )), // 3
                            
        Vertex(glm::vec3(-1.0, -1.0, -1.0f)).addColor(glm::vec3(-0.5f, 0.5f, -0.5f  )), // 4
        Vertex(glm::vec3( 1.0, -1.0, -1.0f)).addColor(glm::vec3( 0.5f, 0.5f, -0.5f  )), // 5
        Vertex(glm::vec3( 1.0,  1.0, -1.0) ).addColor(glm::vec3( 0.5f, 0.5f, 0.5f   )), // 6
        Vertex(glm::vec3(-1.0,  1.0, -1.0) ).addColor(glm::vec3(-0.5f, 0.5f, 0.5f   ))  // 7
    };

    std::vector<GLuint> cubeIndices = {
 		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
    };

    m_Cube = Mesh(cubeVertices, cubeIndices );

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
    m_glRenderer->init();
    m_glRenderer->clear();

    m_Shader.use();
    m_Shader.setMat4("model", glm::identity<glm::mat4>());
    m_Shader.setMat4("view", m_Camera.getViewMat());
    m_Shader.setMat4("projection", m_Camera.getProjectionMat());

    m_Cube.draw();

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
    std::cout << "API : OpenGL " << glGetString(GL_VERSION) << "\n";
}

Application::~Application()
{

}



