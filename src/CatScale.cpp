/*
 * Copyright (C) 2020-2021 Gregory S. Meiste  <http://gregmeiste.com>
 */

#include "CatScale.h"
#include "Constants.h"
#include "ScaleConfig.h"

static int setScaleReading(String reading)
{
    CatScale::get()->setReading(atoi(reading.c_str()));
    return 0;
}

CatScale* CatScale::get()
{
    static CatScale catScale;
    return &catScale;
}

CatScale::CatScale() : mScale(PIN_HX711_DOUT, PIN_HX711_CLK), mSmoothReading(SMOOTH_TIME_CONSTANT),
                       isTared(false), mLastReadingMillis(0), mPrevScaleReading(0)
{
    mDebugMode = (digitalRead(PIN_DEBUG_MODE) == HIGH);
}

void CatScale::begin()
{
    mScale.begin();
    tare();

    Particle.variable("batt_volt", [this](){ return this->getVoltage(); });
    Particle.variable("batt_percent", [this](){ return this->getBatteryPercent(); });

    if (mDebugMode)
    {
        Particle.function("set_scale_reading", setScaleReading);
    }
}

void CatScale::tare()
{
    if (!mDebugMode) mScale.tare();

    mSmoothReading.reset();
    isTared = true;
    mPrevScaleReading = 0;
}

bool CatScale::isReady()
{
    if (mDebugMode)
    {
        return (millis() - mLastReadingMillis) >= (MAX_MS_BETWEEN_READINGS / 2);
    }

    return mScale.is_ready();
}

bool CatScale::takeReading()
{
    bool ret = false;
    system_tick_t now = millis();
    double scaleValue = 0;

    if (mDebugMode)
    {
        scaleValue = mPrevScaleReading;
        mSmoothReading.newSample(scaleValue);

        Log.trace("Pounds: %.2f\tGrams: %.0f\tBattery: %.2f (%u)\tUSB: %d  Charging: %d",
                  getPounds(false), getGrams(false), getVoltage(), getBatteryPercent(), isUsbPowered(), isCharging());
        ret = true;
    }
    else
    {
        scaleValue = mScale.get_value();

        // If the scale was just tared, there is not a previous reading to compare against.
        // Simply take the reading in that case.
        if (!isTared)
        {
            // When a bad reading occurs, the HX711 takes much longer to be ready for the
            // next reading. If there is a large gap between readings, it is safe to say
            // the previous reading is garbage.
            if ((now - mLastReadingMillis) > MAX_MS_BETWEEN_READINGS)
            {
                Log.warn("Dropping bad reading: %.2f pounds", getPounds((float)mPrevScaleReading));
            }
            // Previous reading should be good
            else
            {
                mSmoothReading.newSample(mPrevScaleReading);
                Log.trace("Pounds: %.2f\tGrams: %.0f\tBattery: %.2f (%u)\tUSB: %d  Charging: %d",
                          getPounds(false), getGrams(false), getVoltage(), getBatteryPercent(), isUsbPowered(), isCharging());
                ret = true;
            }
        }
    }

    isTared = false;
    mLastReadingMillis = now;
    mPrevScaleReading = scaleValue;

    return ret;
}

bool CatScale::setReading(double reading)
{
    if (mDebugMode)
    {
        mPrevScaleReading = reading;
        return true;
    }

    return false;
}

float CatScale::getPounds(bool round)
{
    float pounds = getPounds(mSmoothReading.val());

    if (round)
    {
        pounds = roundf(pounds * 10) / 10;
    }

    return pounds;
}

float CatScale::getPounds(float value)
{
    return (value / (float)ScaleConfig::get()->calibrationFactor());
}

float CatScale::getGrams(bool round)
{
    float grams = getGrams(mSmoothReading.val());

    if (round)
    {
        grams = roundf(grams);
    }

    return grams;
}

float CatScale::getGrams(float value)
{
    float calFactor = (float)ScaleConfig::get()->calibrationFactor() / GRAMS_IN_POUND;
    return (value / calFactor);
}

float CatScale::getVoltage() const
{
    return analogRead(BATT) * 0.0011224;
}

bool CatScale::isUsbPowered() const
{
    // PWR: 0=no USB power, 1=USB powered
    return digitalRead(PWR);
}

bool CatScale::isCharging() const
{
    // CHG: 0=charging, 1=not charging
    return (isUsbPowered() && !digitalRead(CHG));
}

unsigned int CatScale::getBatteryPercent() const
{
    static ExponentiallySmoothedValue smoothPercent(BATT_PERCENT_TIME_CONSTANT);
    float percent = ((getVoltage() - BATTERY_MIN_VOLT) / (BATTERY_MAX_VOLT - BATTERY_MIN_VOLT)) * 100;

    if (percent > 100.0f) percent = 100.0f;
    else if (percent < 0.0f) percent = 0.0f;

    smoothPercent.newSample(percent);

    return (unsigned int)(smoothPercent.val());
}
