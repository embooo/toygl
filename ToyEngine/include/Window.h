#pragma once
#include <string>
#include <vector>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Event.h"
#include "MouseEvent.h"
#include "KeyboardEvent.h"
#include "WindowEvent.h"
#include "ISubject.h"
#include "IObserver.h"

// Creates a GLFW window containing an OpengGL graphics context

class Window : public ISubject
{
public:
    Window();
    Window(const char* title, int width, int height, int glMajor = 3, int glMinor = 3);
    ~Window();

    void init();
    void swapBuffers();
    void update();

    bool isClosed() const;

    GLFWwindow* getRawPtr();

    // Observer pattern
    virtual void attach(IObserver* observer) override;
    virtual void detach(IObserver* observer) override;
    virtual void notify(Event& event) override;

    // Getters & setters
    int height();
    int width();

private:
    GLFWwindow* m_pWindow;
    std::vector<IObserver*> m_Observers;

private:
    void terminate();

private:
    struct GLContextProperties
    {
        GLContextProperties(int major, int minor)
            : m_Major(major), m_Minor(minor) {}

        int m_Major;
        int m_Minor;

        unsigned int glMajor() { return m_Major; }
        unsigned int glMinor() { return m_Minor; }
    };
    struct WindowProperties
    {
        WindowProperties(const char* title, unsigned int width, unsigned int height,
            int major, int minor)
            : m_Title(title), m_Width(width), m_Height(height),
            m_ContextProps(major, minor)
        {
        }

        const std::string& title() { return m_Title; }
        int getHeight() { return m_Height; }
        void setHeight(int height) {  m_Height = height; }
        void setWidth(int width) { m_Width = width; }
        int getWidth() { return m_Width; }

        GLContextProperties& glContextProps() { return m_ContextProps; }

        std::string  m_Title;
        int m_Height;
        int m_Width;

        GLContextProperties m_ContextProps;
    };

public:
    WindowProperties m_Props;
};