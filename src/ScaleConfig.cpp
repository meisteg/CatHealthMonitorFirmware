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
        EEPROM.put(SCALE_CONFIG_ADDR, mScaleCfg);
    }
}

int32_t ScaleConfig::calibrationFactor()
{
    return mScaleCfg.calibration_factor;
}

void ScaleConfig::calibrationFactor(int32_t calibrationFactor)
{
    mScaleCfg.calibration_factor = calibrationFactor;
    EEPROM.put(SCALE_CONFIG_ADDR, mScaleCfg);
}
