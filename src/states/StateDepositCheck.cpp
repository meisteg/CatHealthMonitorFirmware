/*
 * Copyright (C) 2019-2020 Gregory S. Meiste  <http://gregmeiste.com>
 */

#include <math.h>

#include "StateDepositCheck.h"
#include "StateManager.h"
#include "Constants.h"
#include "CatManager.h"
#include "ScaleConfig.h"

String StateDepositCheck::getName()
{
    return "DEPOSIT_CHECK";
}

void StateDepositCheck::processReading(CatScale *scale)
{
    float reading = roundf(scale->getGrams());

    if (fabs(reading - mPrevReading) > 2.0f)
    {
        // Reading hasn't settled down
        mNumSameReadings = 0;
        mPrevReading = reading;
    }
    else
    {
        mNumSameReadings++;

        if (mNumSameReadings >= ScaleConfig::get()->numReadingsForStable())
        {
            // Deposit determined
            Serial.printlnf("Deposit: %.0f", reading);
            CatManager::get()->setCatLastDeposit(reading);

            StateManager::get()->setState(StateManager::STATE_EMPTY);
        }
    }
}

void StateDepositCheck::enter()
{
    mNumSameReadings = 0;
    mPrevReading = 0.0f;

    // Prevent OTA updates while checking cat deposit
    System.disableUpdates();
}

void StateDepositCheck::exit()
{
    mNumSameReadings = 0;
    mPrevReading = 0.0f;

    CatManager::get()->publishCatVisit();
}
