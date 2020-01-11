/*
 * Copyright (C) 2019-2020 Gregory S. Meiste  <http://gregmeiste.com>
 */

#include <math.h>

#include "StateTare.h"
#include "StateManager.h"

String StateTare::getName()
{
    return "TARE";
}

bool StateTare::takeReading(CatScale *scale)
{
    bool ret = scale->takeReading();

    if (ret)
    {
        // Just tared the scale, so initial reading should be close to zero
        if (fabs(scale->getGrams(false)) > 1.0f)
        {
            Serial.println("Non-zero reading: Tare the scale again");
            scale->tare();
            ret = false;
        }
    }

    return ret;
}

void StateTare::processReading(CatScale *scale)
{
    // If we get here, the scale is properly tared. Move to next state.
    StateManager::get()->setState(StateManager::STATE_EMPTY);
}

void StateTare::enter()
{
    CatScale::get()->tare();
}
