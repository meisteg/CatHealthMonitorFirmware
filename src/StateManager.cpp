/*
 * Copyright (C) 2019-2020 Gregory S. Meiste  <http://gregmeiste.com>
 */

#include "StateManager.h"

StateManager* StateManager::get()
{
    static StateManager stateManager;
    return &stateManager;
}

StateManager::StateManager() : mStateCurrent(STATE_INIT)
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

        Serial.print("state_current = ");
        Serial.print(getStateString(mStateCurrent));
        Serial.print(", state_new = ");
        Serial.println(getStateString(state_new));

        Particle.publish("state_change", getStateString(state_new).c_str(), PRIVATE);

        mStateCurrent = state_new;
        mStates[state_new]->enter();
    }
}

State* StateManager::getState()
{
    return mStates[mStateCurrent];
}
