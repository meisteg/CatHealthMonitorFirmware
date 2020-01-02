/*
 * Copyright (C) 2019-2020 Gregory S. Meiste  <http://gregmeiste.com>
 */

#ifndef STATE_INIT_H
#define STATE_INIT_H

#include "application.h"

#include "State.h"

class StateInit : public State
{
public:
    String getName() override;
    bool takeReading(CatScale *scale) override;
    void processReading(CatScale *scale) override;
    void enter() override;
};

#endif
