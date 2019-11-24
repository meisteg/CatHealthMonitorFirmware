#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

#include "application.h"

#include "StateCatPresent.h"
#include "StateTrain.h"
#include "StateEmpty.h"
#include "StateInit.h"
#include "State.h"

class StateManager
{
public:
    enum
    {
        STATE_INIT,
        STATE_EMPTY,
        STATE_TRAIN,
        STATE_CAT_PRESENT,

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
    State* mStates[STATE__MAX] = {new StateInit(), new StateEmpty(), new StateTrain(), new StateCatPresent()};
};

StateManager* getStateManager();

#endif
