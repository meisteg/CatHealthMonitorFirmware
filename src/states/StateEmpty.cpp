#include <math.h>

#include "StateEmpty.h"
#include "StateManager.h"
#include "Constants.h"

String StateEmpty::getName()
{
    return "EMPTY";
}

void StateEmpty::processReading(float reading)
{
    reading = roundf(reading * 10) / 10;

    if ((reading != mPrevReading) || (reading == 0.0f))
    {
        mNumSameNonZeroReadings = 0;
        mPrevReading = reading;
    }
    else
    {
        mNumSameNonZeroReadings++;

        if (mNumSameNonZeroReadings >= NUM_REQ_SAME_READINGS)
        {
            // Is it a cat?
            if (reading >= MIN_CAT_WEIGHT_LBS)
            {
                // TODO: Determine which cat
                getStateManager()->setState(StateManager::STATE_CAT_PRESENT);
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

void StateEmpty::enter()
{
    mNumSameNonZeroReadings = 0;
    mPrevReading = 0.0f;
}

void StateEmpty::exit()
{
    mNumSameNonZeroReadings = 0;
    mPrevReading = 0.0f;
}
