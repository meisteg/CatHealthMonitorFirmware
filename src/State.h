#ifndef STATE_H
#define STATE_H

#include "application.h"

class State
{
public:
    virtual String getName() = 0;
    virtual void newReading(float reading);
};

#endif
