#pragma once

#include <iostream>
enum class EventType
{
    NONE, 
    // Mouse event
    MouseScroll, MouseMove, MouseButtonPressed, MouseButtonReleased,
    // Keyboard
    KeyPressed, KeyReleased, KeyRepeat,
    // Window event
    WindowResize, WindowMinimize, WindowClose
};

class Event
{
public:
    Event();
    Event(const EventType& type);

    friend std::ostream& operator<<(std::ostream& os, const Event& event)
    {
        switch (event.getType())
        {
            case EventType::NONE:                os << "NONE";                  return os;
            case EventType::MouseScroll:         os << "Mouse Scroll";          return os;
            case EventType::MouseMove:           os << "Mouse Move";            return os;
            case EventType::MouseButtonPressed:  os << "Mouse button pressed";  return os;
            case EventType::MouseButtonReleased: os << "Mouse button released"; return os;
            case EventType::KeyPressed:          os << "Key press";             return os;
            case EventType::KeyReleased:         os << "Key release";           return os;
            case EventType::KeyRepeat:           os << "Key repeat";            return os;
            case EventType::WindowResize:        os << "Window resize";         return os;
            case EventType::WindowMinimize:      os << "Window minimize";         return os;
            case EventType::WindowClose:         os << "Window close";          return os;

            default: os << "Unknown event."; return os;
        }
    }

    virtual EventType getType() const { return m_Type; }

    EventType m_Type;
    virtual ~Event() {};
};


