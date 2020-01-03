/*
 * Copyright (C) 2020 Gregory S. Meiste  <http://gregmeiste.com>
 */

#include "CatScale.h"
#include "Constants.h"
#include "ScaleConfig.h"

CatScale* CatScale::get()
{
    static CatScale catScale;
    return &catScale;
}

CatScale::CatScale() : mScale(PIN_HX711_DOUT, PIN_HX711_CLK), mSmoothReading(SMOOTH_TIME_CONSTANT), isTared(false)
{
    // Nothing to do
}

void CatScale::begin()
{
    mScale.begin();
    tare();
}

void CatScale::tare()
{
    mScale.tare();
    mSmoothReading.reset();
    isTared = true;
}

bool CatScale::isReady()
{
    return mScale.is_ready();
}

bool CatScale::takeReading()
{
    static unsigned int numDroppedReadings = 0;
    bool ret = false;
    double scaleValue = mScale.get_value();
    float newPounds = getPounds(scaleValue);

    // If the scale was just tared, there is not a previous reading to compare against.
    // Simply take the reading in that case.
    //
    // If not just tared, verify the reading is reasonable by comparing against previous
    // reading. Drop readings that are drastically different than the previous reading.
    //
    // However, it is possible that a drastically different reading is valid. If multiple
    // readings in a row are dropped, assume the new reading is actually good. This prevents
    // a scenario where readings are dropped forever.
    if (isTared || (fabs(newPounds - getPounds()) < MAX_LBS_CHANGE) || (numDroppedReadings >= 5))
    {
        mSmoothReading.newSample(scaleValue);
        Serial.printlnf("%u\tPounds: %.2f\tGrams: %.0f", millis(), getPounds(), getGrams());

        ret = true;
        numDroppedReadings = 0;
        isTared = false;
    }
    else
    {
        Serial.printlnf("%u\tdrop: %.2f", millis(), newPounds);
        numDroppedReadings++;
    }

    return ret;
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
