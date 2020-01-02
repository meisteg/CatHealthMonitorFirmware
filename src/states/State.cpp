/*
 * Copyright (C) 2019-2020 Gregory S. Meiste  <http://gregmeiste.com>
 */

#include <math.h>

#include "State.h"
#include "CatScale.h"

bool State::takeReading(CatScale *scale)
{
    return scale->takeReading();
}

void State::processReading(float reading)
{
    // Base class does nothing
}

void State::enter()
{
    // Base class does nothing
}

void State::exit()
{
    // Base class does nothing
}

void State::loop()
{
    // Base class does nothing
}
