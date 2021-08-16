#pragma once

#include "Event.h"
#include <GLFW/glfw3.h>

class KeyEvent : public Event
{
public:
    KeyEvent(int key, int scancode, int action, int mods)
        : m_Key(key), m_Scancode(scancode), m_Action(action), m_Mods(mods)
    {
        m_Type = action == GLFW_PRESS ? EventType::KeyPressed
            : action == GLFW_REPEAT ? EventType::KeyRepeat
            : EventType::KeyReleased;
    }

    virtual EventType getType() const override { return m_Type; }

    EventType m_Type;
    int m_Key, m_Scancode, m_Action, m_Mods;
};
