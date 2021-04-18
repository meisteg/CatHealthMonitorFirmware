/*
 * Copyright (C) 2019-2021 Gregory S. Meiste  <http://gregmeiste.com>
 */

#include "CatScale.h"
#include "StateManager.h"
#include "Constants.h"
#include "CatManager.h"
#include "ScaleConfig.h"

// USB Serial configuration
SerialLogHandler logHandler(LOG_LEVEL_WARN, { // Logging level for non-application messages
    {"app", LOG_LEVEL_TRACE}                  // Logging level for all application messages
});

// Serial over TX/RX configuration
Serial1LogHandler log1Handler(SERIAL_BAUD, LOG_LEVEL_WARN, { // Logging level for non-application messages
    {"app", LOG_LEVEL_TRACE}                                 // Logging level for all application messages
});

// Used to setup the system LED theme
LEDSystemTheme theme;

int catTrain(String cat_name)
{
    if (StateManager::get()->isState(StateManager::STATE_EMPTY) &&
        CatManager::get()->setupToTrain(cat_name))
    {
        Log.info("Train New Cat: %s", cat_name.c_str());
        StateManager::get()->setState(StateManager::STATE_TRAIN);

        return 0;
    }

    return -1;
}

int resetCats(String unused)
{
    Log.warn("Removing all cats from memory");
    CatManager::get()->reset();

    return 0;
}

int scaleCalibrate(String calibration)
{
    ScaleConfig::get()->calibrationFactor(atoi(calibration.c_str()));

    Log.info("New calibration factor: %ld", ScaleConfig::get()->calibrationFactor());
    StateManager::get()->setState(StateManager::STATE_TARE);

    return 0;
}

int setAIOKey(String aioKey)
{
    if (aioKey.length() == AIO_KEY_LEN)
    {
        ScaleConfig::get()->aioKey(aioKey);
        Log.info("New AIO Key: %s", ScaleConfig::get()->aioKey());

        return 0;
    }

    return -1;
}

int setReadingsForStable(String readings)
{
    ScaleConfig::get()->numReadingsForStable(atoi(readings.c_str()));

    Log.info("New number of readings to be stable: %u",
             ScaleConfig::get()->numReadingsForStable());

    return 0;
}

int setNoVisitAlert(String secs)
{
    ScaleConfig::get()->noVisitAlertTime(atoi(secs.c_str()));

    Log.info("New no visit alert time: %lu seconds",
             ScaleConfig::get()->noVisitAlertTime());

    return 0;
}

int setMasterDevice(String master)
{
    ScaleConfig::get()->isMaster(atoi(master.c_str()) != 0);

    Log.info("New master state: %s",
             ScaleConfig::get()->isMaster() ? "Master" : "Slave");

    return 0;
}

void setup()
{
    Log.info("Cat Health Monitor");
    Log.info("Build date/time: %s %s", __DATE__, __TIME__);

    Particle.function("calibration", scaleCalibrate);
    Particle.function("aio_key", setAIOKey);
    Particle.function("readings_for_stable", setReadingsForStable);
    Particle.function("master", setMasterDevice);
    if (ScaleConfig::get()->isMaster())
    {
        // Master devices have additional functions available
        Particle.function("train", catTrain);
        Particle.function("reset", resetCats);
        Particle.function("no_visit_alert", setNoVisitAlert);
    }

    StateManager::get()->registerVariable();

    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_DEBUG_MODE, INPUT_PULLDOWN);
    pinMode(PWR, INPUT);
    pinMode(CHG, INPUT);

    CatManager::get()->publishCatDatabase();

    CatScale::get()->begin();

    // Publish vitals each hour
    Particle.publishVitals(3600);

    // Setup for graceful disconnects from the cloud
    Particle.setDisconnectOptions(CloudDisconnectOptions().graceful(true).timeout(5s));

    // Disable the breathing white when network disabled
    theme.setColor(LED_SIGNAL_NETWORK_OFF, RGB_COLOR_NONE);
    theme.apply();
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
    if (((millis() - Particle.timeSyncedLast()) > TIME_SYNC_MILLIS) &&
        Particle.connected() &&
        Particle.syncTimeDone())
    {
        Particle.syncTime();
    }
}
