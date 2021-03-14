/*
 * Copyright (C) 2019-2021 Gregory S. Meiste  <http://gregmeiste.com>
 */

#ifndef STATE_EMPTY_H
#define STATE_EMPTY_H

#include "application.h"

#include "State.h"

class StateEmpty : public State
{
public:
    StateEmpty();
    String getName() override;
    void processReading(CatScale *scale) override;
    void enter() override;
    void loop() override;

private:
    void checkBatteryState();

    int mNumSameNonZeroReadingsPounds;
    int mNumSameNonZeroReadingsGrams;
    float mPrevReadingPounds;
    float mPrevReadingGrams;
    bool mSentBatteryWarning;
    bool mWasBatteryCharging;
};

#endif
