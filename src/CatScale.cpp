/*
 * Copyright (C) 2020 Gregory S. Meiste  <http://gregmeiste.com>
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
        Serial.printlnf("%u\tPounds: %.2f\tGrams: %.0f", now, getPounds(), getGrams());
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
                Serial.printlnf("%u\tDropping bad reading: %.2f pounds", now, getPounds(mPrevScaleReading));
            }
            // Previous reading should be good
            else
            {
                mSmoothReading.newSample(mPrevScaleReading);
                Serial.printlnf("%u\tPounds: %.2f\tGrams: %.0f", now, getPounds(), getGrams());
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

float CatScale::getPounds()
{
    return getPounds(mSmoothReading.val());
}

float CatScale::getPounds(float value)
{
    return (value / (float)ScaleConfig::get()->calibrationFactor());
}

float CatScale::getGrams()
{
    return getGrams(mSmoothReading.val());
}

float CatScale::getGrams(float value)
{
    float calFactor = (float)ScaleConfig::get()->calibrationFactor() / GRAMS_IN_POUND;
    return (value / calFactor);
}
