#ifndef STATE_H
#define STATE_H

#include "application.h"

class State
{
public:
    // Gets the name of the state
    virtual String getName() = 0;

    // Passes a new scale reading to the state for processing
    virtual void processReading(float reading);

    // Called by the State Manager when this state is entered
    virtual void enter();

    // Called by the State Manager when this state is exited
    virtual void exit();
};

#endif
