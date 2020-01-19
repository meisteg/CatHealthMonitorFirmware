/*
 * Copyright (C) 2019-2020 Gregory S. Meiste  <http://gregmeiste.com>
 */

#include "StateCatPresent.h"
#include "StateManager.h"
#include "Constants.h"
#include "CatManager.h"
#include "ScaleConfig.h"

String StateCatPresent::getName()
{
    return "CAT_PRESENT";
}

void StateCatPresent::processReading(CatScale *scale)
{
    float reading = scale->getPounds(true);

    if (reading < CAT_NOT_PRESENT_THRESHOLD)
    {
        mNumReadingsLessThanThreshold++;

        if (mNumReadingsLessThanThreshold >= ScaleConfig::get()->numReadingsForStable())
        {
            // Cat is no longer present
            uint16_t duration = ((millis() - mTimeEnter) + 500) / 1000;
            Serial.printlnf("Duration: %u seconds", duration);
            CatManager::get()->setCatLastDuration(duration);

            StateManager::get()->setState(StateManager::STATE_DEPOSIT_CHECK);
        }
    }
    else
    {
        mNumReadingsLessThanThreshold = 0;

        // If is possible if a cat enters just right (slowly), a heavier cat can
        // falsely be detected as a lighter cat. As a result, while cat is present,
        // verify the selected cat is still correct, and if not, update the selection
        // as necessary. Only do this is the new weight is higher than the initial
        // weight to prevent introducing the same bug for cat exit.
        if ((mMinReading > reading) || (reading != mPrevReading))
        {
            mNumSameReadings = 0;
            mPrevReading = reading;
        }
        else if (mNumSameReadings++ >= ScaleConfig::get()->numReadingsForStable())
        {
            if (CatManager::get()->changeSelectedCatIfNecessary(reading))
            {
                // Lock in heavier weight to prevent selecting lighter cat on exit
                mMinReading = reading;
            }
            mNumSameReadings = 0;
        }
    }
}

void StateCatPresent::enter()
{
    mTimeEnter = millis();
    mNumReadingsLessThanThreshold = 0;
    mNumSameReadings = 0;
    mMinReading = mPrevReading = CatScale::get()->getPounds(true);

    digitalWrite(PIN_LED, HIGH);

    // Prevent OTA updates while a cat is present
    System.disableUpdates();
}

void StateCatPresent::exit()
{
    digitalWrite(PIN_LED, LOW);
}

void StateCatPresent::loop()
{
    if ((millis() - mTimeEnter) > MAX_CAT_PRESENT_TIME_MS)
    {
        // Been in this state too long, so a cat probably isn't present
        CatManager::get()->deselectCat();
        StateManager::get()->setState(StateManager::STATE_TARE);
    }
}
