/*
 * Copyright (C) 2019 Gregory S. Meiste  <http://gregmeiste.com>
 */

#include "ExponentiallySmoothedValue.h"
#include "HX711ADC.h"
#include "StateManager.h"
#include "Constants.h"
#include "CatManager.h"
#include "ScaleConfig.h"

// parameter "gain" is ommited; the default value 128 is used by the library
HX711ADC scale(PIN_HX711_DOUT, PIN_HX711_CLK);

ExponentiallySmoothedValue val(0.5f);

int catTrain(String cat_name)
{
    if (getStateManager()->isState(StateManager::STATE_EMPTY) &&
        getCatManager()->setupToTrain(cat_name))
    {
        Serial.print("Train New Cat: ");
        Serial.println(cat_name);

        getStateManager()->setState(StateManager::STATE_TRAIN);

        return 0;
    }

    return -1;
}

int resetCats(String unused)
{
    Serial.println("Removing all cats from memory");
    getCatManager()->reset();

    return 0;
}

int scaleCalibrate(String calibration)
{
    ScaleConfig::get()->calibrationFactor(atoi(calibration.c_str()));

    Serial.print("New calibration factor: ");
    Serial.println(ScaleConfig::get()->calibrationFactor());

    scale.set_scale(ScaleConfig::get()->calibrationFactor());
    getStateManager()->setState(StateManager::STATE_INIT);

    return 0;
}

int setAIOKey(String aioKey)
{
    if (aioKey.length() == AIO_KEY_LEN)
    {
        ScaleConfig::get()->aioKey(aioKey);

        Serial.print("New AIO Key: ");
        Serial.println(ScaleConfig::get()->aioKey());

        return 0;
    }

    return -1;
}

int setReadingsForStable(String readings)
{
    ScaleConfig::get()->numReadingsForStable(atoi(readings.c_str()));

    Serial.print("New number of readings to be stable: ");
    Serial.println(ScaleConfig::get()->numReadingsForStable());

    return 0;
}

void setup()
{
    Serial.begin(SERIAL_BAUD);

    Particle.function("train", catTrain);
    Particle.function("reset", resetCats);
    Particle.function("calibration", scaleCalibrate);
    Particle.function("aio_key", setAIOKey);
    Particle.function("readings_for_stable", setReadingsForStable);

    getStateManager()->registerVariable();

    scale.begin();

    pinMode(PIN_LED, OUTPUT);

    // Wait for a USB serial connection for up to 10 seconds
    // waitFor(Serial.isConnected, 10000);

    Serial.println("Cat Health Monitor");
    getCatManager()->printCatDatabase();

    scale.set_scale(ScaleConfig::get()->calibrationFactor());
    scale.tare(); //Reset the scale to 0
}

void loop()
{
    State *state = getStateManager()->getState();

    if (scale.is_ready())
    {
        if (state->takeReading())
        {
            state->processReading(val.val());
        }
    }
    else
    {
        state->loop();
    }
}
