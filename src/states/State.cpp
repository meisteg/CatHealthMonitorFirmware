#include <math.h>

#include "State.h"
#include "CatHealthMonitor.h"
#include "Constants.h"

bool State::takeReading()
{
    static unsigned int numDroppedReadings = 0;
    bool ret = false;
    float reading = scale.get_units();

    if ((fabs(reading - val.val()) < MAX_LBS_CHANGE) || (numDroppedReadings >= 5))
    {
        Serial.print(millis());
        Serial.print("\t Raw: ");
        Serial.print(reading, 2);
        reading = val.newSample(reading);
        Serial.print("\t Smooth: ");
        Serial.println(reading, 2);

        ret = true;
        numDroppedReadings = 0;
    }
    else
    {
        Serial.print(millis());
        Serial.print("\t drop: ");
        Serial.println(reading, 2);

        numDroppedReadings++;
    }

    return ret;
}

void State::processReading(float reading)
{
    // Base class does nothing
}

void State::enter()
{
    // Base class does nothing
}

void State::exit()
{
    // Base class does nothing
}

void State::loop()
{
    // Base class does nothing
}
