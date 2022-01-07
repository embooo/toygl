#pragma once

#include <memory>

#include "OpenGLRenderer.h"
#include "UserInterface.h"
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
    virtual  void update(float deltaTime);
    virtual  void onUpdate(Event& event) override;
    virtual  void onKeyPressed(KeyEvent& event);
    inline  float getDeltaTime();
    virtual  void onWindowResize(WindowResizeEvent& event);
    virtual  void onWindowMinimize(WindowMinimizeEvent& event);

    static void printSystemInfo();

    bool m_Minimized;

private:
    gltf::Model model;
    Camera m_Camera;
    Shader m_Shader, m_InfiniteGridShader;
    float m_lastFrameTime;

    std::unique_ptr<UserInterface> m_UserInterface;
    std::unique_ptr<Window> m_Window;
    std::unique_ptr<OpenGLRenderer> m_glRenderer;
};

