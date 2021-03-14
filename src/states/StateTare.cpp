/*
 * Copyright (C) 2019-2021 Gregory S. Meiste  <http://gregmeiste.com>
 */

#include <math.h>

#include "StateTare.h"
#include "StateManager.h"
#include "Constants.h"

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
            Log.warn("Non-zero reading: Tare the scale again");
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
