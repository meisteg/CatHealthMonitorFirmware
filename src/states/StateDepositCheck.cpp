/*
 * Copyright (C) 2019-2021 Gregory S. Meiste  <http://gregmeiste.com>
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
    float pounds = scale->getPounds(true);
    float grams = scale->getGrams(true);

    if ((fabs(grams - mPrevReading) > 2.0f) || (pounds >= MIN_CAT_WEIGHT_LBS))
    {
        // Reading hasn't settled down or cat has returned
        mNumSameReadings = 0;
        mPrevReading = grams;
    }
    else
    {
        mNumSameReadings++;

        if (mNumSameReadings >= ScaleConfig::get()->numReadingsForStable())
        {
            // Deposit determined
            Log.info("Deposit: %.0f", grams);
            CatManager::get()->setCatLastDeposit(grams);

            StateManager::get()->setState(StateManager::STATE_TARE);
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
