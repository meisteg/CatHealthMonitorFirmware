/*
 * Copyright (C) 2019 Gregory S. Meiste  <http://gregmeiste.com>
 */

#ifndef STATE_POSSIBLE_H
#define STATE_POSSIBLE_H

#include "application.h"

#include "State.h"

class StateCatPossible : public State
{
public:
    String getName() override;
    void processReading(float reading) override;
    void enter() override;

private:
    int mNumSameReadings;
    float mPrevReading;
    float mInitialReading;
};

#endif
