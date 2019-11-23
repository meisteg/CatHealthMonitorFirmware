#include "StateCatPresent.h"
#include "StateTrain.h"
#include "StateEmpty.h"
#include "StateInit.h"
#include "State.h"
#include "ExponentiallySmoothedValue.h"
#include "HX711ADC.h"

#define HX711_DOUT          D3
#define HX711_CLK           D2

#define SERIAL_BAUD         115200

#define MAX_LBS_CHANGE      25

// TODO: Read this value from EEPROM?
#define CALIBRATION_FACTOR  -7050

enum
{
  STATE_INIT,
  STATE_EMPTY,
  STATE_TRAIN,
  STATE_CAT_PRESENT,

  // Add new states above this line
  STATE__MAX
};

HX711ADC scale(HX711_DOUT, HX711_CLK);		// parameter "gain" is ommited; the default value 128 is used by the library

ExponentiallySmoothedValue val(0.5f);
int state_current = STATE_INIT;
State* states[STATE__MAX] = {new StateInit(), new StateEmpty(), new StateTrain(), new StateCatPresent()};

bool isState(int state)
{
    return (state == state_current);
}

String getStateString(int state)
{
    if (state < STATE__MAX)
    {
        return states[state]->getName();
    }

    return "UNKNOWN";
}

void setState(int state_new)
{
    if (state_new < STATE__MAX)
    {
        Serial.print("state_current = ");
        Serial.print(getStateString(state_current));
        Serial.print(", state_new = ");
        Serial.println(getStateString(state_new));

        state_current = state_new;
    }
}

int scaleTare(String unused)
{
    Serial.println("===== Scale Tare =====");
    scale.tare();
    val.reset();
    setState(STATE_INIT);

    return 0;
}

int catTrain(String cat_name)
{
    if (isState(STATE_EMPTY))
    {
        Serial.print("Train New Cat: ");
        Serial.println(cat_name);

        setState(STATE_TRAIN);

        return 0;
    }

    return -1;
}

void scaleReading()
{
    float reading = scale.get_units();

    if (isState(STATE_INIT))
    {
        // Just tared the scale, so initial reading should be close to zero
        if (fabs(reading) > 1.0f)
        {
            Serial.print("drop: ");
            Serial.println(reading, 1);

            // Try to tare again
            scale.tare();
        }
        else
        {
            val.newSample(reading);
            setState(STATE_EMPTY);
        }
    }
    else if (fabs(reading - val.val()) < MAX_LBS_CHANGE)
    {
        Serial.print("Raw: ");
        Serial.print(reading, 1);
        reading = val.newSample(reading);
        Serial.print("\t Smooth: ");
        Serial.println(reading, 1);
    }
    else
    {
        Serial.print("drop: ");
        Serial.println(reading, 1);
    }
}

void setup()
{
    Serial.begin(SERIAL_BAUD);

    Particle.function("tare", scaleTare);
    Particle.function("train", catTrain);
    Particle.variable("state_current", state_current);

    scale.begin();

    // Wait for a USB serial connection for up to 10 seconds
    waitFor(Serial.isConnected, 10000);

    Serial.println("Cat Health Monitor");

    scale.set_scale(CALIBRATION_FACTOR);
    scale.tare(); //Reset the scale to 0
}

void loop()
{
    scaleReading();
}
