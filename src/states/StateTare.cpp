/*
 * Copyright (C) 2019-2021 Gregory S. Meiste  <http://gregmeiste.com>
 */

#include <math.h>

#include "StateTare.h"
#include "StateManager.h"
#include "Constants.h"
#include "ScaleConfig.h"

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
    static int32_t prev_calibration = 0;
    int32_t curr_calibration = ScaleConfig::get()->calibrationFactor();

    // If we get here, the scale is properly tared. Move to next state.
    if ((prev_calibration == 0) || (prev_calibration == curr_calibration))
    {
        // Normal operation
        StateManager::get()->setState(StateManager::STATE_EMPTY);
    }
    else
    {
        // Calibration has changed, so move to calibration state for tuning
        StateManager::get()->setState(StateManager::STATE_CALIBRATE);
    }

    prev_calibration = curr_calibration;
}

void StateTare::enter()
{
    CatScale::get()->tare();
}
