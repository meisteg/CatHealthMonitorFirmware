#include "StateCatPresent.h"
#include "StateManager.h"
#include "Constants.h"
#include "CatManager.h"

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

    Serial.print("Duration: ");
    Serial.println(duration);
    getCatManager()->setCatLastDuration(duration);

    digitalWrite(PIN_LED, LOW);
}
