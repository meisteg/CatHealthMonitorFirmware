/*
 * Copyright (C) 2019-2020 Gregory S. Meiste  <http://gregmeiste.com>
 */

#include <math.h>

#include "StateCatPossible.h"
#include "StateManager.h"
#include "Constants.h"
#include "CatManager.h"
#include "ScaleConfig.h"

String StateCatPossible::getName()
{
    return "CAT_POSSIBLE";
}

void StateCatPossible::processReading(float reading)
{
    reading = roundf(reading * 10) / 10;

    if (reading != mPrevReading)
    {
        mNumSameReadings = 0;
        mPrevReading = reading;
    }
    else
    {
        mNumSameReadings++;

        if (mNumSameReadings >= ScaleConfig::get()->numReadingsForStable())
        {
            // Is it now a cat?
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

                // Is it still possible to be a cat?
                if ((reading >= MIN_CAT_WEIGHT_LBS) && (reading != mInitialReading))
                {
                    // Wait some more time
                    mNumSameReadings = 0;
                    mInitialReading = mPrevReading;
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

void StateCatPossible::enter()
{
    mNumSameReadings = 0;
    mInitialReading = mPrevReading = roundf(CatScale::get()->getPounds() * 10) / 10;

    // Prevent OTA updates while determining if a cat is present
    System.disableUpdates();
}
