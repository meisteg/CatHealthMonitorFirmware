#include "StateCatPresent.h"
#include "StateManager.h"
#include "Constants.h"

String StateCatPresent::getName()
{
    return "CAT_PRESENT";
}

void StateCatPresent::processReading(float reading)
{
    if (reading < CAT_NOT_PRESENT_THRESHOLD)
    {
        mNumReadingsLessThanThreshold++;

        if (mNumReadingsLessThanThreshold >= NUM_REQ_SAME_READINGS)
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

    digitalWrite(PIN_LED, HIGH);
}

void StateCatPresent::exit()
{
    system_tick_t duration = millis() - mTimeEnter;
    mTimeEnter = 0;
    mNumReadingsLessThanThreshold = 0;

    // TODO: Do something with duration besides print
    Serial.print("Duration: ");
    Serial.println(duration);

    digitalWrite(PIN_LED, LOW);
}
