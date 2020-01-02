/*
 * Copyright (C) 2019-2020 Gregory S. Meiste  <http://gregmeiste.com>
 */

#ifndef STATE_H
#define STATE_H

#include "application.h"
#include "CatScale.h"

class State
{
public:
    // Gets the name of the state
    virtual String getName() = 0;

    // Takes a new scale reading
    virtual bool takeReading(CatScale *scale);

    // Processes the new scale reading
    virtual void processReading(CatScale *scale);

    // Called by the State Manager when this state is entered
    virtual void enter();

    // Called by the State Manager when this state is exited
    virtual void exit();

    // The loop function gives the state a chance to perform some loop processing
    // in between scale readings
    virtual void loop();
};

#endif
