#include <math.h>

#include "StateTrain.h"
#include "StateManager.h"
#include "Constants.h"
#include "CatManager.h"

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
    {
        // Cat on scale, same as previous reading
        mNumSameReadings++;

        if (mNumSameReadings >= NUM_REQ_SAME_READINGS)
        {
            // Cat on scale, and weight stable

            // Save weight into Cat Database
            if (!getCatManager()->completeTraining(reading))
            {
                // TODO: Notify user that training was rejected
            }

            // Advance to next state
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