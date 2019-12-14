/*
 * Copyright (C) 2019 Gregory S. Meiste  <http://gregmeiste.com>
 */

#ifndef STATE_H
#define STATE_H

#include "application.h"

class State
{
public:
    // Gets the name of the state
    virtual String getName() = 0;

    // Takes a new scale reading
    virtual bool takeReading();

    // Processes the new scale reading
    virtual void processReading(float reading);

    // Called by the State Manager when this state is entered
    virtual void enter();

    // Called by the State Manager when this state is exited
    virtual void exit();

    // The loop function gives the state a chance to perform some loop processing
    // in between scale readings
    virtual void loop();
};

#endif
