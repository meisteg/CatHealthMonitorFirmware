/*
 * Copyright (C) 2019-2021 Gregory S. Meiste  <http://gregmeiste.com>
 */

#include "ScaleConfig.h"
#include "Constants.h"

// Flag to indicate that this device is the master device
#define FLAG_MASTER_DEVICE   0x01

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
        mScaleCfg.version = 2;
        mScaleCfg.flags = FLAG_MASTER_DEVICE;
        mScaleCfg.device_name[0] = 0;
        save();
    }

    // Check if the configuration needs to be upgraded to a new version schema
    if (mScaleCfg.version < 1)
    {
        Log.info("Updating configuration to version 1");

        mScaleCfg.flags = FLAG_MASTER_DEVICE;
        mScaleCfg.version = 1;
        save();
    }
    if (mScaleCfg.version < 2)
    {
        Log.info("Updating configuration to version 2");

        mScaleCfg.device_name[0] = 0;
        mScaleCfg.version = 2;
        save();
    }

#if USE_ADAFRUIT_IO
    Log.info("AIO Key: %s", aioKey());
#endif
    Log.info("Calibration factor: %ld", calibrationFactor());
    Log.info("Number of readings to be stable: %u", numReadingsForStable());
    Log.info("No visit alert time (0 to disable): %lu seconds", noVisitAlertTime());
    Log.info("Master Device: %s", isMaster() ? "Yes" : "No");
    Log.info("Device Name: %s", deviceName());
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

bool ScaleConfig::isMaster()
{
    return (mScaleCfg.flags & FLAG_MASTER_DEVICE) != 0;
}

void ScaleConfig::isMaster(bool master)
{
    if (master)
    {
        mScaleCfg.flags |= FLAG_MASTER_DEVICE;
    }
    else
    {
        mScaleCfg.flags &= ~(FLAG_MASTER_DEVICE);
    }
    save();
}

char* ScaleConfig::deviceName()
{
    return mScaleCfg.device_name;
}

void ScaleConfig::deviceName(String name)
{
    name.getBytes((unsigned char *)mScaleCfg.device_name, sizeof(mScaleCfg.device_name));
    save();
}
