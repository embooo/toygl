#pragma once
#include "PCH.h"

#include "Event.h"

class IObserver
{
public:
    virtual ~IObserver() {};
    virtual void onUpdate(Event& event) = 0;
};