#include "StateCatPresent.h"
#include "StateManager.h"

#define CAT_NOT_PRESENT_THRESHOLD         1.0f
#define NUM_REQ_READINGS_BELOW_THRESHOLD  3

String StateCatPresent::getName()
{
    return "CAT_PRESENT";
}

void StateCatPresent::processReading(float reading)
{
    if (reading < CAT_NOT_PRESENT_THRESHOLD)
    {
        mNumReadingsLessThanThreshold++;

        if (mNumReadingsLessThanThreshold >= NUM_REQ_READINGS_BELOW_THRESHOLD)
        {
            // Cat is no longer present
            getStateManager()->setState(StateManager::STATE_DEPOSIT_CHECK);
        }
    }
}

void StateCatPresent::enter()
{
    mTimeEnter = millis();
    mNumReadingsLessThanThreshold = 0;
}

void StateCatPresent::exit()
{
    system_tick_t duration = millis() - mTimeEnter;
    mTimeEnter = 0;
    mNumReadingsLessThanThreshold = 0;

    // TODO: Do something with duration besides print
    Serial.print("Duration: ");
    Serial.println(duration);
}
