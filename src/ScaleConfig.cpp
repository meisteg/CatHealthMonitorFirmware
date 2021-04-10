/*
 * Copyright (C) 2019-2021 Gregory S. Meiste  <http://gregmeiste.com>
 */

#include "ScaleConfig.h"
#include "Constants.h"

ScaleConfig* ScaleConfig::get()
{
    static ScaleConfig scaleConfig;
    return &scaleConfig;
}

ScaleConfig::ScaleConfig()
{
    EEPROM.get(SCALE_CONFIG_ADDR, mScaleCfg);
    if (mScaleCfg.magic != SCALE_CONFIG_MAGIC_NUMBER)
    {
        Log.error("ScaleConfig: EEPROM was empty or invalid");
        mScaleCfg.magic = SCALE_CONFIG_MAGIC_NUMBER;
        mScaleCfg.calibration_factor = CALIBRATION_FACTOR_INIT;
        mScaleCfg.aio_key[0] = 0;
        mScaleCfg.num_readings_for_stable = READINGS_TO_BE_STABLE_INIT;
        mScaleCfg.no_visit_alert_time = NO_VISIT_ALERT_TIME_INIT;
        mScaleCfg.version = 0;
        save();
    }
    else
    {
#if USE_ADAFRUIT_IO
        Log.info("AIO Key: %s", aioKey());
#endif
        Log.info("Calibration factor: %ld", calibrationFactor());
        Log.info("Number of readings to be stable: %u", numReadingsForStable());
        Log.info("No visit alert time (0 to disable): %lu seconds", noVisitAlertTime());
    }

    // Ensure version is set correctly on existing devices.
    if (mScaleCfg.version != 0)
    {
        mScaleCfg.version = 0;
        save();
    }
}

void ScaleConfig::save()
{
    EEPROM.put(SCALE_CONFIG_ADDR, mScaleCfg);
}

int32_t ScaleConfig::calibrationFactor()
{
    return mScaleCfg.calibration_factor;
}

void ScaleConfig::calibrationFactor(int32_t calibrationFactor)
{
    mScaleCfg.calibration_factor = calibrationFactor;
    save();
}

char* ScaleConfig::aioKey()
{
    return mScaleCfg.aio_key;
}

void ScaleConfig::aioKey(String aioKey)
{
    aioKey.getBytes((unsigned char *)mScaleCfg.aio_key, sizeof(mScaleCfg.aio_key));
    save();
}

uint8_t ScaleConfig::numReadingsForStable()
{
    return mScaleCfg.num_readings_for_stable;
}

void ScaleConfig::numReadingsForStable(uint8_t readings)
{
    mScaleCfg.num_readings_for_stable = readings;
    save();
}

uint32_t ScaleConfig::noVisitAlertTime()
{
    return mScaleCfg.no_visit_alert_time;
}

void ScaleConfig::noVisitAlertTime(uint32_t secs)
{
    // NOTE: Setting a larger time will not resend an alert if an alert was
    // already sent for a cat. The cat must visit to reset the alert.
    mScaleCfg.no_visit_alert_time = secs;
    save();
}
