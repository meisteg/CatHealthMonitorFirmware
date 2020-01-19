/*
 * Copyright (C) 2019-2020 Gregory S. Meiste  <http://gregmeiste.com>
 */

#include <math.h>

#include "StateTrain.h"
#include "StateManager.h"
#include "Constants.h"
#include "CatManager.h"
#include "ScaleConfig.h"

StateTrain::StateTrain() : mNumSameReadings(0), mPrevReading(0.0f)
{
    // Nothing to do
}

String StateTrain::getName()
{
    return "TRAIN";
}

void StateTrain::processReading(CatScale *scale)
{
    float reading = scale->getPounds(true);

    if (reading < MIN_CAT_WEIGHT_LBS)
    {
        // Cat not yet on scale
        mPrevReading = 0.0f;
        return;
    }

    if (reading != mPrevReading)
    {
        // Cat on scale, but reading hasn't settled down
        mNumSameReadings = 0;
        mPrevReading = reading;
    }
    else
    {
        // Cat on scale, same as previous reading
        mNumSameReadings++;

        if (mNumSameReadings >= ScaleConfig::get()->numReadingsForStable())
        {
            // Cat on scale, and weight stable

            // Save weight into Cat Database
            if (!CatManager::get()->completeTraining(reading))
            {
                Serial.println("ERROR: Failed to complete training");
                Particle.publish("cat_alert", "{\"msg\": \"Failed to complete training!\"}", PRIVATE);
            }

            // Advance to next state
            StateManager::get()->setState(StateManager::STATE_CAT_PRESENT);
        }
    }
}

void StateTrain::enter()
{
    mLastLedToggleMillis = millis();
    mCurrentLedLevel = HIGH;
    digitalWrite(PIN_LED, mCurrentLedLevel);

    // Prevent OTA updates while training
    System.disableUpdates();
}

void StateTrain::exit()
{
    // Cleanup
    mNumSameReadings = 0;
    mPrevReading = 0.0f;
}

void StateTrain::loop()
{
    if ((millis() - mLastLedToggleMillis) > TRAINING_LED_TOGGLE_MS)
    {
        mLastLedToggleMillis = millis();
        mCurrentLedLevel = !mCurrentLedLevel;
        digitalWrite(PIN_LED, mCurrentLedLevel);
    }
}
