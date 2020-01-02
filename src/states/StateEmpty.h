/*
 * Copyright (C) 2019-2020 Gregory S. Meiste  <http://gregmeiste.com>
 */

#ifndef STATE_EMPTY_H
#define STATE_EMPTY_H

#include "application.h"

#include "State.h"

class StateEmpty : public State
{
public:
    String getName() override;
    void processReading(CatScale *scale) override;
    void enter() override;
    void exit() override;

private:
    int mNumSameNonZeroReadings;
    float mPrevReading;
};

#endif
