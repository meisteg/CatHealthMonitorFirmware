/*
 * Copyright (C) 2019-2021 Gregory S. Meiste  <http://gregmeiste.com>
 */

#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

#include "application.h"

#include "states/StateCatPresent.h"
#include "states/StateTrain.h"
#include "states/StateEmpty.h"
#include "states/StateTare.h"
#include "states/State.h"
#include "states/StateDepositCheck.h"
#include "states/StateCatPossible.h"
#include "states/StateCalibrate.h"

class StateManager
{
public:
    // List of states of the state machine
    enum
    {
        STATE_TARE,
        STATE_EMPTY,
        STATE_TRAIN,
        STATE_CAT_PRESENT,
        STATE_DEPOSIT_CHECK,
        STATE_CAT_POSSIBLE,
        STATE_CALIBRATE,

        // Add new states above this line
        STATE__MAX
    };

    // Get the StateManager singleton
    static StateManager* get();

    // Registers a Particle variable on the cloud to retrieve the current state
    bool registerVariable();

    // Returns true if the current state is the specified state
    bool isState(int state);

    // Gets the string name of the specified state
    String getStateString(int state);

    // Set the new state. State enter/exit functions are only called if state has changed.
    void setState(int state_new);

    // Get the current state object
    State* getState();

private:
    // Constructor
    StateManager();

    // Index of the current state
    int mStateCurrent;

    // Array of state objects
    State* mStates[STATE__MAX] = {
        new StateTare(),
        new StateEmpty(),
        new StateTrain(),
        new StateCatPresent(),
        new StateDepositCheck(),
        new StateCatPossible(),
        new StateCalibrate()
    };
};

#endif
