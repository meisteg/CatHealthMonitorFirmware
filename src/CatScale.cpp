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

CatScale::CatScale() : mScale(PIN_HX711_DOUT, PIN_HX711_CLK), mSmoothPounds(SMOOTH_TIME_CONSTANT), isTared(false)
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
    mSmoothPounds.reset();
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
    float reading = (mScale.get_value() / (float)ScaleConfig::get()->calibrationFactor());

    // If the scale was just tared, there is not a previous reading to compare against.
    // Simply take the reading in that case.
    //
    // If not just tared, verify the reading is reasonable by comparing against previous
    // reading. Drop readings that are drastically different than the previous reading.
    //
    // However, it is possible that a drastically different reading is valid. If multiple
    // readings in a row are dropped, assume the new reading is actually good. This prevents
    // a senario where readings are dropped forever.
    if (isTared || (fabs(reading - getPounds()) < MAX_LBS_CHANGE) || (numDroppedReadings >= 5))
    {
        Serial.printf("%u\t Raw: %.2f", millis(), reading);
        reading = mSmoothPounds.newSample(reading);
        Serial.printlnf("\t Smooth: %.2f", reading);

        ret = true;
        numDroppedReadings = 0;
        isTared = false;
    }
    else
    {
        Serial.printlnf("%u\t drop: %.2f", millis(), reading);
        numDroppedReadings++;
    }

    return ret;
}

float CatScale::getPounds()
{
    return mSmoothPounds.val();
}
