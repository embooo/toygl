#pragma once
#include "IObserver.h"
#include "Event.h"
class ISubject
{
public:
    virtual ~ISubject() {};

    virtual void attach(IObserver* observer) = 0;
    virtual void detach(IObserver* observer) = 0;
    virtual void notify(Event& event) = 0;
};