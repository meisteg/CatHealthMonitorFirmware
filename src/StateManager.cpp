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
    char publishString[64];

    if ((state_new < STATE__MAX) && (state_new != mStateCurrent))
    {
        Serial.print("state_current = ");
        Serial.print(getStateString(mStateCurrent));
        Serial.print(", state_new = ");
        Serial.println(getStateString(state_new));

        mStates[mStateCurrent]->exit();

        snprintf(publishString, sizeof(publishString),
                 "{\"prev\": \"%s\", \"new\": \"%s\"}",
                 getStateString(mStateCurrent).c_str(), getStateString(state_new).c_str());
        Particle.publish("state_change", publishString, PRIVATE);

        mStateCurrent = state_new;
        mStates[state_new]->enter();
    }
}

State* StateManager::getState()
{
    return mStates[mStateCurrent];
}
