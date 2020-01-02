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

void StateCatPresent::processReading(float reading)
{
    if (reading < CAT_NOT_PRESENT_THRESHOLD)
    {
        mNumReadingsLessThanThreshold++;

        if (mNumReadingsLessThanThreshold >= ScaleConfig::get()->numReadingsForStable())
        {
            // Cat is no longer present
            StateManager::get()->setState(StateManager::STATE_DEPOSIT_CHECK);
        }
    }
}

void StateCatPresent::enter()
{
    mTimeEnter = millis();
    mNumReadingsLessThanThreshold = 0;

    digitalWrite(PIN_LED, HIGH);

    // Prevent OTA updates while a cat is present
    System.disableUpdates();
}

void StateCatPresent::exit()
{
    system_tick_t duration = millis() - mTimeEnter;
    mTimeEnter = 0;
    mNumReadingsLessThanThreshold = 0;

    Serial.printlnf("Duration: %u", duration);
    CatManager::get()->setCatLastDuration(duration);

    digitalWrite(PIN_LED, LOW);
}
