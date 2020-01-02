/*
 * Copyright (C) 2019-2020 Gregory S. Meiste  <http://gregmeiste.com>
 */

#ifndef STATE_DEPOSIT_CHECK_H
#define STATE_DEPOSIT_CHECK_H

#include "application.h"

#include "State.h"

class StateDepositCheck : public State
{
public:
    String getName() override;
    void processReading(CatScale *scale) override;
    void enter() override;
    void exit() override;

private:
    int mNumSameReadings;
    float mPrevReading;
};

#endif
