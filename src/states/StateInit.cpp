#include <math.h>

#include "StateInit.h"
#include "CatHealthMonitor.h"
#include "StateManager.h"

String StateInit::getName()
{
    return "INIT";
}

bool StateInit::takeReading()
{
    bool ret = false;
    float reading = scale.get_units();

    // Just tared the scale, so initial reading should be close to zero
    if (fabs(reading) > 0.05f)
    {
        Serial.print(millis());
        Serial.print("\t drop: ");
        Serial.println(reading, 2);

        // Try to tare again
        scale.tare();
    }
    else
    {
        val.newSample(reading);
        ret = true;
    }

    return ret;
}

void StateInit::processReading(float reading)
{
    // If we get here, the scale is properly tared. More to next state.
    getStateManager()->setState(StateManager::STATE_EMPTY);
}

void StateInit::enter()
{
    scale.tare();
    val.reset();
}
