/*
 * Copyright (C) 2019 Gregory S. Meiste  <http://gregmeiste.com>
 */

#ifndef CAT_HEALTH_MONITOR_H
#define CAT_HEALTH_MONITOR_H

#include "ExponentiallySmoothedValue.h"
#include "HX711ADC.h"

extern HX711ADC scale;
extern ExponentiallySmoothedValue val;

#endif