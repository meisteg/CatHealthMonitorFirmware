#include <math.h>

#include "StateTrain.h"
#include "StateManager.h"

#define MIN_CAT_WEIGHT_LBS     5.0f
#define NUM_REQ_SAME_READINGS  10

StateTrain::StateTrain() : mNumSameReadings(0), mPrevReading(0.0f)
{
    // Nothing to do
}

String StateTrain::getName()
{
    return "TRAIN";
}

void StateTrain::processReading(float reading)
{
    reading = roundf(reading * 10) / 10;

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
    {   // Cat on scale, same as previous reading
        mNumSameReadings++;

        if (mNumSameReadings >= NUM_REQ_SAME_READINGS)
        {
            // Cat on scale, and weight captured
            // TODO: Save weight into Cat Database

            getStateManager()->setState(StateManager::STATE_CAT_PRESENT);
        }
    }
}

void StateTrain::exit()
{
    // Cleanup
    mNumSameReadings = 0;
    mPrevReading = 0.0f;
}