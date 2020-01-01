/*
 * Copyright (C) 2019 Gregory S. Meiste  <http://gregmeiste.com>
 */

#include <math.h>

#include "StateCatPossible.h"
#include "StateManager.h"
#include "Constants.h"
#include "CatManager.h"
#include "CatHealthMonitor.h"
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
            if (getCatManager()->selectCatByWeight(reading))
            {
                getStateManager()->setState(StateManager::STATE_CAT_PRESENT);
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
                    Serial.print("Automatic tare due to non-zero reading: ");
                    Serial.println(reading, 1);

                    getStateManager()->setState(StateManager::STATE_INIT);
                }
            }
        }
    }
}

void StateCatPossible::enter()
{
    mNumSameReadings = 0;
    mInitialReading = mPrevReading = roundf(val.val() * 10) / 10;

    // Prevent OTA updates while determining if a cat is present
    System.disableUpdates();
}
