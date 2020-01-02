/*
 * Copyright (C) 2019-2020 Gregory S. Meiste  <http://gregmeiste.com>
 */

#include "CatScale.h"
#include "StateManager.h"
#include "Constants.h"
#include "CatManager.h"
#include "ScaleConfig.h"

int catTrain(String cat_name)
{
    if (StateManager::get()->isState(StateManager::STATE_EMPTY) &&
        CatManager::get()->setupToTrain(cat_name))
    {
        Serial.printlnf("Train New Cat: %s", cat_name.c_str());
        StateManager::get()->setState(StateManager::STATE_TRAIN);

        return 0;
    }

    return -1;
}

int resetCats(String unused)
{
    Serial.println("Removing all cats from memory");
    CatManager::get()->reset();

    return 0;
}

int scaleCalibrate(String calibration)
{
    ScaleConfig::get()->calibrationFactor(atoi(calibration.c_str()));

    Serial.printlnf("New calibration factor: %d", ScaleConfig::get()->calibrationFactor());
    StateManager::get()->setState(StateManager::STATE_INIT);

    return 0;
}

int setAIOKey(String aioKey)
{
    if (aioKey.length() == AIO_KEY_LEN)
    {
        ScaleConfig::get()->aioKey(aioKey);
        Serial.printlnf("New AIO Key: %s", ScaleConfig::get()->aioKey());

        return 0;
    }

    return -1;
}

int setReadingsForStable(String readings)
{
    ScaleConfig::get()->numReadingsForStable(atoi(readings.c_str()));

    Serial.printlnf("New number of readings to be stable: %u",
                    ScaleConfig::get()->numReadingsForStable());

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

    StateManager::get()->registerVariable();

    pinMode(PIN_LED, OUTPUT);

    // Wait for a USB serial connection for up to 10 seconds
    // waitFor(Serial.isConnected, 10000);

    Serial.println("Cat Health Monitor");
    CatManager::get()->printCatDatabase();

    CatScale::get()->begin();

    // Publish vitals each hour
    Particle.publishVitals(3600);
}

void loop()
{
    State *state = StateManager::get()->getState();
    CatScale *scale = CatScale::get();

    if (scale->isReady())
    {
        if (state->takeReading(scale))
        {
            state->processReading(scale->getPounds());
        }
    }
    else
    {
        state->loop();
    }
}
