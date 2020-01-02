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

    // Get the current reading value in pounds
    float getPounds();

private:
    // Constructor
    CatScale();

    // HX711 scale driver
    HX711ADC mScale;

    // Exponentially smoothed value (pounds)
    ExponentiallySmoothedValue mSmoothPounds;

    // Flag to indicate the scale was just tared
    bool isTared;
};

#endif