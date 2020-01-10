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
    float pounds = scale->getPounds();
    pounds = roundf(pounds * 10) / 10;

    if ((pounds != mPrevReadingPounds) || (pounds == 0.0f))
    {
        mNumSameNonZeroReadingsPounds = 0;
        mPrevReadingPounds = pounds;

        if (pounds == 0.0f)
        {
            // In order to have an accurate deposit check, need to have minimal scale drift.
            float grams = roundf(scale->getGrams());
            if ((grams != mPrevReadingGrams) || (fabs(scale->getGrams()) < 8.0f))
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
                    Serial.printlnf("Automatic tare due to non-zero grams: %.0f", grams);
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
                    Serial.printlnf("Automatic tare due to non-zero pounds: %.1f", pounds);
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
