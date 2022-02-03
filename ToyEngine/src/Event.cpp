#include "PCH.h"

#include "Event.h"

Event::Event()
    : m_Type(EventType::NONE)
{
}

Event::Event(const EventType& type)
    : m_Type(type)
{

}



