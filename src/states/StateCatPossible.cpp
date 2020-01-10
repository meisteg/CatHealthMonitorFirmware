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

void StateCatPossible::processReading(CatScale *scale)
{
    float reading = scale->getPounds();
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
            stableReading(reading);
        }
    }
}

void StateCatPossible::stableReading(float reading)
{
    // Is it now a cat?
    if (CatManager::get()->selectCatByWeight(reading))
    {
        StateManager::get()->setState(StateManager::STATE_CAT_PRESENT);
    }
    // Not yet a known cat
    else
    {
        system_tick_t now = millis();

        // Only publish new stable reading if different than the previously published reading
        if (reading != mInitialReading)
        {
            char publishString[32];
            snprintf(publishString, sizeof(publishString),
                     "{\"reading\": %.1f}", reading);
            Particle.publish("stable_reading", publishString, PRIVATE);

            mTimeStable = now;
        }

        // Is it still possible to be a cat?
        if ((reading >= MIN_CAT_WEIGHT_LBS) && ((now - mTimeStable) < CAT_POSSIBLE_TIMEOUT_MS))
        {
            // Wait some more time
            mNumSameReadings = 0;
            mInitialReading = mPrevReading;
        }
        // Scale drift, cat deposits or litter box cleaning
        else
        {
            Serial.printlnf("Automatic tare due to non-zero reading: %.1f", reading);
            StateManager::get()->setState(StateManager::STATE_TARE);
        }
    }
}

void StateCatPossible::enter()
{
    mNumSameReadings = 0;
    mInitialReading = mPrevReading = roundf(CatScale::get()->getPounds() * 10) / 10;
    mTimeStable = millis();

    // Prevent OTA updates while determining if a cat is present
    System.disableUpdates();
}
