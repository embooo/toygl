#pragma once

#include <memory>

#include "OpenGLRenderer.h"
#include "Window.h"
#include "IObserver.h"

// TODO : remove
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"

class Application : public IObserver
{
public:
    Application();
    ~Application();

    void run();
    void render();
    virtual void update(float deltaTime);
    virtual void onUpdate(Event& event) override;

    virtual void onWindowResize(WindowResizeEvent& event);

    static void printSystemInfo();

private:
    // TODO : remove
    Mesh m_Cube;
    Camera m_Camera;
    Shader m_Shader;

    float m_lastFrameTime;

    std::unique_ptr<Window> m_Window;
    std::unique_ptr<OpenGLRenderer> m_glRenderer;
};

