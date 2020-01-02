/*
 * Copyright (C) 2019-2020 Gregory S. Meiste  <http://gregmeiste.com>
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
        Serial.println("ScaleConfig: EEPROM was empty or invalid");
        mScaleCfg.magic = SCALE_CONFIG_MAGIC_NUMBER;
        mScaleCfg.calibration_factor = CALIBRATION_FACTOR_INIT;
        // mScaleCfg.aio_key intentionally not set
        mScaleCfg.num_readings_for_stable = READINGS_TO_BE_STABLE_INIT;
        save();
    }
    else
    {
        Serial.printlnf("AIO Key: %s", aioKey());
        Serial.printlnf("Number of readings to be stable: %u", numReadingsForStable());
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
