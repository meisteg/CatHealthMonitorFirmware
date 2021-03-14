/*
 * Copyright (C) 2019-2021 Gregory S. Meiste  <http://gregmeiste.com>
 */

#include "StateManager.h"
#include "Constants.h"

StateManager* StateManager::get()
{
    static StateManager stateManager;
    return &stateManager;
}

StateManager::StateManager() : mStateCurrent(STATE_TARE)
{
    // Nothing to do
}

bool StateManager::registerVariable()
{
    return Particle.variable("state", mStateCurrent);
}

bool StateManager::isState(int state)
{
    return (state == mStateCurrent);
}

String StateManager::getStateString(int state)
{
    if (state < STATE__MAX)
    {
        return mStates[state]->getName();
    }

    return "UNKNOWN";
}

void StateManager::setState(int state_new)
{
    if ((state_new < STATE__MAX) && (state_new != mStateCurrent))
    {
        mStates[mStateCurrent]->exit();

        Log.info("state_current = %s, state_new = %s",
                 getStateString(mStateCurrent).c_str(),
                 getStateString(state_new).c_str());

        Particle.publish("state_change", getStateString(state_new).c_str(), PRIVATE);

        mStateCurrent = state_new;
        mStates[state_new]->enter();
    }
}

State* StateManager::getState()
{
    return mStates[mStateCurrent];
}
