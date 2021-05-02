/*
 * Copyright (C) 2021 Gregory S. Meiste  <http://gregmeiste.com>
 */

#ifndef STATE_CALIBRATE_H
#define STATE_CALIBRATE_H

#include "application.h"

#include "State.h"

class StateCalibrate : public State
{
public:
    String getName() override;
};

#endif
