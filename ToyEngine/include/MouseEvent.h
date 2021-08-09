#pragma once

#include "Event.h"

class MouseScroll : public Event
{
public:
    MouseScroll(double xoffset, double yoffset)
        : m_Type(EventType::MouseScroll), m_xOffset(xoffset), m_yOffset(yoffset) {}

    float getXOffset() { return m_xOffset; }
    float getYOffset() { return m_yOffset; }

    virtual EventType getType() const { return m_Type; }
    EventType m_Type;
    float m_xOffset, m_yOffset;
};

class MouseMove : public Event
{
public:
    MouseMove(double xpos, double ypos)
        : m_Type(EventType::MouseMove), m_xPos(xpos), m_yPos(ypos)  {}

    double getX() { return m_xPos; }
    double getY() { return m_yPos; }

    virtual EventType getType() const { return m_Type; }

    EventType m_Type;
private:
    double m_xPos, m_yPos;
};

class MouseClick : public Event
{
public:
    MouseClick(int button, int action, int mods)
        : m_Type(EventType::MouseButtonPressed), m_Button(button), m_Action(action), m_Mods(mods) 
    {
    }

    int m_Button;
    int m_Action;
    int m_Mods;

    virtual EventType getType() const { return m_Type; }

    EventType m_Type;
};