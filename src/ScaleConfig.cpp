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
        SERIAL.println("ScaleConfig: EEPROM was empty or invalid");
        mScaleCfg.magic = SCALE_CONFIG_MAGIC_NUMBER;
        mScaleCfg.calibration_factor = CALIBRATION_FACTOR_INIT;
        // mScaleCfg.aio_key intentionally not set
        mScaleCfg.num_readings_for_stable = READINGS_TO_BE_STABLE_INIT;
        mScaleCfg.no_visit_alert_time = NO_VISIT_ALERT_TIME_INIT;
        save();
    }
    else
    {
#if USE_ADAFRUIT_IO
        SERIAL.printlnf("AIO Key: %s", aioKey());
#endif
        SERIAL.printlnf("Calibration factor: %ld", calibrationFactor());
        SERIAL.printlnf("Number of readings to be stable: %u", numReadingsForStable());
        SERIAL.printlnf("No visit alert time (0 to disable): %lu seconds", noVisitAlertTime());
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
