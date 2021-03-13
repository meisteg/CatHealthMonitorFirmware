/*
 * Copyright (C) 2019-2021 Gregory S. Meiste  <http://gregmeiste.com>
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
        SERIAL.printlnf("Train New Cat: %s", cat_name.c_str());
        StateManager::get()->setState(StateManager::STATE_TRAIN);

        return 0;
    }

    return -1;
}

int resetCats(String unused)
{
    SERIAL.println("Removing all cats from memory");
    CatManager::get()->reset();

    return 0;
}

int scaleCalibrate(String calibration)
{
    ScaleConfig::get()->calibrationFactor(atoi(calibration.c_str()));

    SERIAL.printlnf("New calibration factor: %ld", ScaleConfig::get()->calibrationFactor());
    StateManager::get()->setState(StateManager::STATE_TARE);

    return 0;
}

int setAIOKey(String aioKey)
{
    if (aioKey.length() == AIO_KEY_LEN)
    {
        ScaleConfig::get()->aioKey(aioKey);
        SERIAL.printlnf("New AIO Key: %s", ScaleConfig::get()->aioKey());

        return 0;
    }

    return -1;
}

int setReadingsForStable(String readings)
{
    ScaleConfig::get()->numReadingsForStable(atoi(readings.c_str()));

    SERIAL.printlnf("New number of readings to be stable: %u",
                    ScaleConfig::get()->numReadingsForStable());

    return 0;
}

int setNoVisitAlert(String secs)
{
    ScaleConfig::get()->noVisitAlertTime(atoi(secs.c_str()));

    SERIAL.printlnf("New no visit alert time: %lu seconds",
                    ScaleConfig::get()->noVisitAlertTime());

    return 0;
}

void setup()
{
    SERIAL.begin(SERIAL_BAUD);

    Particle.function("train", catTrain);
    Particle.function("reset", resetCats);
    Particle.function("calibration", scaleCalibrate);
    Particle.function("aio_key", setAIOKey);
    Particle.function("readings_for_stable", setReadingsForStable);
    Particle.function("no_visit_alert", setNoVisitAlert);

    StateManager::get()->registerVariable();

    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_DEBUG_MODE, INPUT_PULLDOWN);

    SERIAL.println("Cat Health Monitor");
    SERIAL.printlnf("Build date/time: %s %s", __DATE__, __TIME__);
    CatManager::get()->publishCatDatabase();

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
            state->processReading(scale);
        }
    }
    else
    {
        state->loop();
    }

    // Ensure the time stays sync'd with the cloud
    if (((millis() - Particle.timeSyncedLast()) > TIME_SYNC_MILLIS) && Particle.syncTimeDone())
    {
        Particle.syncTime();
    }
}
