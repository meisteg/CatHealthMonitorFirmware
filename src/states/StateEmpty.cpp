/*
 * Copyright (C) 2019-2020 Gregory S. Meiste  <http://gregmeiste.com>
 */

#include <math.h>

#include "StateEmpty.h"
#include "StateManager.h"
#include "Constants.h"
#include "CatManager.h"
#include "ScaleConfig.h"

String StateEmpty::getName()
{
    return "EMPTY";
}

void StateEmpty::processReading(CatScale *scale)
{
    float reading = scale->getPounds();
    reading = roundf(reading * 10) / 10;

    if ((reading != mPrevReading) || (reading == 0.0f))
    {
        mNumSameNonZeroReadings = 0;
        mPrevReading = reading;
    }
    else
    {
        mNumSameNonZeroReadings++;

        if (mNumSameNonZeroReadings >= ScaleConfig::get()->numReadingsForStable())
        {
            // Is it a cat?
            if (CatManager::get()->selectCatByWeight(reading))
            {
                StateManager::get()->setState(StateManager::STATE_CAT_PRESENT);
            }
            else
            {
                char publishString[64];
                snprintf(publishString, sizeof(publishString),
                         "{\"reading\": %.1f}", reading);
                Particle.publish("stable_reading", publishString, PRIVATE);

                // Is it possible to be a cat?
                if (reading >= MIN_CAT_WEIGHT_LBS)
                {
                    StateManager::get()->setState(StateManager::STATE_CAT_POSSIBLE);
                }
                // Scale drift, cat deposits or litter box cleaning
                else
                {
                    Serial.printlnf("Automatic tare due to non-zero reading: %.1f", reading);
                    StateManager::get()->setState(StateManager::STATE_INIT);
                }
            }
        }
    }
}

void StateEmpty::enter()
{
    mNumSameNonZeroReadings = 0;
    mPrevReading = 0.0f;

    // Enable OTA updates while empty
    System.enableUpdates();
}

void StateEmpty::exit()
{
    mNumSameNonZeroReadings = 0;
    mPrevReading = 0.0f;
}
