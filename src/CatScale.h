/*
 * Copyright (C) 2020 Gregory S. Meiste  <http://gregmeiste.com>
 */

#ifndef CAT_SCALE_H
#define CAT_SCALE_H

#include "application.h"
#include "ExponentiallySmoothedValue.h"
#include "HX711ADC.h"

class CatScale
{
public:
    // Get the CatScale singleton
    static CatScale* get();

    // Setup the pins of the scale
    void begin();

    // Reset the scale to 0
    void tare();

    // Returns true to indicate that the scale is ready for another reading
    bool isReady();

    // Takes a new scale reading
    bool takeReading();

    // FOR DEBUG ONLY: Set a new value for the scale reading
    bool setReading(double reading);

    // Get the current reading value in pounds
    float getPounds();

    // Get the current reading value in grams
    float getGrams();

private:
    // Constructor
    CatScale();

    // Get the specified reading value in pounds
    float getPounds(float value);

    // Get the specified reading value in grams
    float getGrams(float value);

    // HX711 scale driver
    HX711ADC mScale;

    // Exponentially smoothed scale reading value
    ExponentiallySmoothedValue mSmoothReading;

    // Flag to indicate the scale was just tared
    bool isTared;

    // Time of the last reading in milliseconds
    system_tick_t mLastReadingMillis;

    // The previous scale reading. Not valid after tare (use isTared to check if valid).
    double mPrevScaleReading;

    // Flag to indicate debug mode. In debug mode, the HX711 is not used.
    bool mDebugMode;
};

#endif