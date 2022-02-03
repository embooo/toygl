#pragma once
#include "PCH.h"

#include <glad/glad.h>

#include "Event.h"
#include "MouseEvent.h"
#include "KeyboardEvent.h"
#include "WindowEvent.h"
#include "ISubject.h"
#include "IObserver.h"
#include "UserInterface.h"

// Creates a GLFW window containing an OpengGL graphics context

class Window : public ISubject
{
public:
    Window();
    Window(const char* title, int width, int height, int glMajor = 4, int glMinor = 3, int swapInterval = 1, bool vsync = true);
    ~Window();

    void init();
    void swapBuffers();
    void update();
    void terminate();

    bool isClosed() const;

    GLFWwindow* getRawPtr() const;

    // Observer pattern
    virtual void attach(IObserver* observer) override;
    virtual void detach(IObserver* observer) override;
    virtual void notify(Event& event) override;

    // Helper function
    void setCursorVisibility(bool visible = false);
    bool isVsyncActive() const;

    // Getters & setters
    int  height();
    int  width();
    void toggleVsync();

    UserInterface& getUI();

private:
    GLFWwindow* m_pWindow;
    UserInterface m_UserInterface;
    std::vector<IObserver*> m_Observers;

private:

private:
    struct GLContextProps
    {
        int major;
        int minor;
    };

    struct WindowProperties
    {
        const char*  title;
        int width;
        int height;
        GLContextProps GLProps;
        int  swapInterval;
        bool vsyncEnabled;
    };

public:
    WindowProperties m_Props;
};