/*
 * Copyright (C) 2019-2020 Gregory S. Meiste  <http://gregmeiste.com>
 */

#ifndef STATE_TRAIN_H
#define STATE_TRAIN_H

#include "application.h"

#include "State.h"

class StateTrain : public State
{
public:
    StateTrain();
    String getName() override;
    void processReading(CatScale *scale) override;
    void enter() override;
    void exit() override;
    void loop() override;

private:
    int mNumSameReadings;
    float mPrevReading;
    system_tick_t mLastLedToggleMillis;
    int mCurrentLedLevel;
};

#endif
