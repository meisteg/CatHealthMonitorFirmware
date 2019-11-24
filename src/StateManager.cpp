#include "StateManager.h"

StateManager* getStateManager()
{
    static StateManager stateManager;
    return &stateManager;
}

StateManager::StateManager() : mStateCurrent(STATE_INIT)
{
    // Nothing to do
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
        Serial.print("state_current = ");
        Serial.print(getStateString(mStateCurrent));
        Serial.print(", state_new = ");
        Serial.println(getStateString(state_new));

        mStates[mStateCurrent]->exit();

        mStateCurrent = state_new;
        mStates[state_new]->enter();
    }
}

State* StateManager::getState()
{
    return mStates[mStateCurrent];
}
