#pragma once

#include "Event.h"

class WindowResizeEvent : public Event
{
public:
    WindowResizeEvent(int width, int height)
        : m_Type(EventType::WindowResize), m_Width(width), m_Height(height) {}

    int getWidth() { return m_Width; }
    int getHeight() { return m_Height; }

    virtual EventType getType() const override { return m_Type; }

    EventType m_Type;
    int m_Width, m_Height;
};

class WindowCloseEvent : public Event
{
public:
    WindowCloseEvent()
        : m_Type(EventType::WindowClose){}

    virtual EventType getType() const override { return m_Type; }

    EventType m_Type;
};

class WindowMinimizeEvent : public Event
{
public:
    WindowMinimizeEvent(bool minimized)
        : m_Type(EventType::WindowMinimize), m_Minimized(minimized) {}

    bool m_Minimized;

    virtual EventType getType() const override { return m_Type; }
    EventType m_Type;
};