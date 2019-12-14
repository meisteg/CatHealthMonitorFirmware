/*
 * Copyright (C) 2019 Gregory S. Meiste  <http://gregmeiste.com>
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
    void processReading(float reading) override;
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
