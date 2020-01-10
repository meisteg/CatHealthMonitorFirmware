/*
 * Copyright (C) 2019-2020 Gregory S. Meiste  <http://gregmeiste.com>
 */

#ifndef STATE_TARE_H
#define STATE_TARE_H

#include "application.h"

#include "State.h"

class StateTare : public State
{
public:
    String getName() override;
    bool takeReading(CatScale *scale) override;
    void processReading(CatScale *scale) override;
    void enter() override;
};

#endif
