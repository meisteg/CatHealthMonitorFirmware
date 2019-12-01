#include <math.h>

#include "StateDepositCheck.h"
#include "StateManager.h"
#include "Constants.h"
#include "CatManager.h"

String StateDepositCheck::getName()
{
    return "DEPOSIT_CHECK";
}

void StateDepositCheck::processReading(float reading)
{
    reading = roundf(reading * 10) / 10;

    if (reading != mPrevReading)
    {
        // Reading hasn't settled down
        mNumSameReadings = 0;
        mPrevReading = reading;
    }
    else
    {
        mNumSameReadings++;

        if (mNumSameReadings >= NUM_REQ_SAME_READINGS)
        {
            // Deposit determined
            Serial.print("Deposit: ");
            Serial.println(reading, 1);
            getCatManager()->setCatLastDeposit(reading);

            getStateManager()->setState(StateManager::STATE_EMPTY);
        }
    }
}

void StateDepositCheck::enter()
{
    mNumSameReadings = 0;
    mPrevReading = 0.0f;
}

void StateDepositCheck::exit()
{
    mNumSameReadings = 0;
    mPrevReading = 0.0f;

    getCatManager()->publishCatVisit();
}
