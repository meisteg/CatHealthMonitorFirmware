/*
 * Copyright (C) 2019-2020 Gregory S. Meiste  <http://gregmeiste.com>
 */

#ifndef STATE_POSSIBLE_H
#define STATE_POSSIBLE_H

#include "application.h"

#include "State.h"

class StateCatPossible : public State
{
public:
    String getName() override;
    void processReading(CatScale *scale) override;
    void enter() override;

private:
    int mNumSameReadings;
    float mPrevReading;
    float mInitialReading;
};

#endif
