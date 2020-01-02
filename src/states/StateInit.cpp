/*
 * Copyright (C) 2019-2020 Gregory S. Meiste  <http://gregmeiste.com>
 */

#include <math.h>

#include "StateInit.h"
#include "StateManager.h"

String StateInit::getName()
{
    return "INIT";
}

bool StateInit::takeReading(CatScale *scale)
{
    bool ret = scale->takeReading();

    if (ret)
    {
        // Just tared the scale, so initial reading should be close to zero
        if (fabs(scale->getGrams()) > 1.0f)
        {
            Serial.println("Non-zero reading: Tare the scale again");
            scale->tare();
            ret = false;
        }
    }

    return ret;
}

void StateInit::processReading(CatScale *scale)
{
    // If we get here, the scale is properly tared. Move to next state.
    StateManager::get()->setState(StateManager::STATE_EMPTY);
}

void StateInit::enter()
{
    CatScale::get()->tare();
}
