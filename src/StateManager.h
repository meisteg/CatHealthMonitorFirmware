/*
 * Copyright (C) 2019 Gregory S. Meiste  <http://gregmeiste.com>
 */

#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

#include "application.h"

#include "states/StateCatPresent.h"
#include "states/StateTrain.h"
#include "states/StateEmpty.h"
#include "states/StateInit.h"
#include "states/State.h"
#include "states/StateDepositCheck.h"

class StateManager
{
public:
    enum
    {
        STATE_INIT,
        STATE_EMPTY,
        STATE_TRAIN,
        STATE_CAT_PRESENT,
        STATE_DEPOSIT_CHECK,

        // Add new states above this line
        STATE__MAX
    };

    StateManager();

    bool isState(int state);
    String getStateString(int state);
    void setState(int state_new);
    State* getState();

private:
    int mStateCurrent;

    State* mStates[STATE__MAX] = {
        new StateInit(),
        new StateEmpty(),
        new StateTrain(),
        new StateCatPresent(),
        new StateDepositCheck()
    };
};

StateManager* getStateManager();

#endif
