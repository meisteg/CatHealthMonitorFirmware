/*
 * Copyright (C) 2019-2021 Gregory S. Meiste  <http://gregmeiste.com>
 */

#include <math.h>

#include "StateEmpty.h"
#include "StateManager.h"
#include "Constants.h"
#include "CatManager.h"
#include "ScaleConfig.h"

StateEmpty::StateEmpty() : mSentBatteryWarning(false), mWasBatteryCharging(false)
{
    // Nothing to do
}

String StateEmpty::getName()
{
    return "EMPTY";
}

void StateEmpty::processReading(CatScale *scale)
{
    float pounds = scale->getPounds(true);

    if ((pounds != mPrevReadingPounds) || (pounds == 0.0f))
    {
        mNumSameNonZeroReadingsPounds = 0;
        mPrevReadingPounds = pounds;

        if (pounds == 0.0f)
        {
            // In order to have an accurate deposit check, need to have minimal scale drift.
            float grams = scale->getGrams(true);
            if ((grams != mPrevReadingGrams) || (fabs(grams) < 8.0f))
            {
                mNumSameNonZeroReadingsGrams = 0;
                mPrevReadingGrams = grams;
            }
            else
            {
                mNumSameNonZeroReadingsGrams++;
                if (mNumSameNonZeroReadingsGrams >= ScaleConfig::get()->numReadingsForStable())
                {
                    // Scale drift, cat deposits or litter box cleaning
                    Log.info("Automatic tare due to non-zero grams: %.0f", grams);
                    StateManager::get()->setState(StateManager::STATE_TARE);
                }
            }
        }
    }
    else
    {
        mNumSameNonZeroReadingsPounds++;

        if (mNumSameNonZeroReadingsPounds >= ScaleConfig::get()->numReadingsForStable())
        {
            // Is it a cat?
            if (CatManager::get()->selectCatByWeight(pounds))
            {
                StateManager::get()->setState(StateManager::STATE_CAT_PRESENT);
            }
            else
            {
                char publishString[64];
                snprintf(publishString, sizeof(publishString),
                         "{\"reading\": %.1f}", pounds);
                Particle.publish("stable_reading", publishString, PRIVATE);

                // Is it possible to be a cat?
                if (pounds >= MIN_CAT_WEIGHT_LBS)
                {
                    StateManager::get()->setState(StateManager::STATE_CAT_POSSIBLE);
                }
                // Scale drift, cat deposits or litter box cleaning
                else
                {
                    Log.info("Automatic tare due to non-zero pounds: %.1f", pounds);
                    StateManager::get()->setState(StateManager::STATE_TARE);
                }
            }
        }
    }
}

void StateEmpty::enter()
{
    mNumSameNonZeroReadingsPounds = 0;
    mPrevReadingPounds = 0.0f;

    mNumSameNonZeroReadingsGrams = 0;
    mPrevReadingGrams = 0.0f;

    // Enable OTA updates while empty
    System.enableUpdates();
}

void StateEmpty::loop()
{
    CatManager::get()->checkLastCatVisits();
    checkBatteryState();
}

void StateEmpty::checkBatteryState()
{
    char publish[128];

    CatScale *scale = CatScale::get();
    if (scale->isUsbPowered())
    {
        // Reset battery warning flag
        mSentBatteryWarning = false;

        bool isCharging = scale->isCharging();
        if (mWasBatteryCharging && !isCharging)
        {
            snprintf(publish, sizeof(publish),
                     "{\"msg\": \"Battery charging is complete! You may now unplug.\"}");
            Particle.publish("cat_alert", publish, PRIVATE);
        }

        mWasBatteryCharging = isCharging;
    }
    else
    {
        if ((scale->getBatteryPercent() < BATTERY_WARN_PERCENT) && !mSentBatteryWarning)
        {
            snprintf(publish, sizeof(publish),
                     "{\"msg\": \"Battery less than %u percent. Please plug in to charge.\"}",
                     BATTERY_WARN_PERCENT);
            mSentBatteryWarning = Particle.publish("cat_alert", publish, PRIVATE);
        }

        mWasBatteryCharging = false;
    }
}
