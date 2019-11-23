#include "ExponentiallySmoothedValue.h"
#include "HX711ADC.h"
#include "StateManager.h"

#define HX711_DOUT          D3
#define HX711_CLK           D2

#define SERIAL_BAUD         115200

#define MAX_LBS_CHANGE      25

// TODO: Read this value from EEPROM?
#define CALIBRATION_FACTOR  -7050

HX711ADC scale(HX711_DOUT, HX711_CLK);		// parameter "gain" is ommited; the default value 128 is used by the library

ExponentiallySmoothedValue val(0.5f);

int scaleTare(String unused)
{
    Serial.println("===== Scale Tare =====");
    scale.tare();
    val.reset();
    getStateManager()->setState(StateManager::STATE_INIT);

    return 0;
}

int catTrain(String cat_name)
{
    if (getStateManager()->isState(StateManager::STATE_EMPTY))
    {
        Serial.print("Train New Cat: ");
        Serial.println(cat_name);

        getStateManager()->setState(StateManager::STATE_TRAIN);

        return 0;
    }

    return -1;
}

void scaleReading()
{
    float reading = scale.get_units();

    if (getStateManager()->isState(StateManager::STATE_INIT))
    {
        // Just tared the scale, so initial reading should be close to zero
        if (fabs(reading) > 1.0f)
        {
            Serial.print(millis());
            Serial.print("\t drop: ");
            Serial.println(reading, 1);

            // Try to tare again
            scale.tare();
        }
        else
        {
            val.newSample(reading);
            getStateManager()->setState(StateManager::STATE_EMPTY);
        }
    }
    else if (fabs(reading - val.val()) < MAX_LBS_CHANGE)
    {
        Serial.print(millis());
        Serial.print("\t Raw: ");
        Serial.print(reading, 1);
        reading = val.newSample(reading);
        Serial.print("\t Smooth: ");
        Serial.println(reading, 1);
    }
    else
    {
        Serial.print(millis());
        Serial.print("\t drop: ");
        Serial.println(reading, 1);
    }
}

void setup()
{
    Serial.begin(SERIAL_BAUD);

    Particle.function("tare", scaleTare);
    Particle.function("train", catTrain);

    scale.begin();

    // Wait for a USB serial connection for up to 10 seconds
    waitFor(Serial.isConnected, 10000);

    Serial.println("Cat Health Monitor");

    scale.set_scale(CALIBRATION_FACTOR);
    scale.tare(); //Reset the scale to 0
}

void loop()
{
    if (scale.is_ready())
    {
        scaleReading();
    }
}
