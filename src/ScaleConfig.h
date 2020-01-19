/*
 * Copyright (C) 2019-2020 Gregory S. Meiste  <http://gregmeiste.com>
 */

#ifndef SCALE_CONFIG_H
#define SCALE_CONFIG_H

#include "application.h"
#include "Constants.h"

class ScaleConfig
{
public:
    static ScaleConfig* get();

    int32_t calibrationFactor();
    void calibrationFactor(int32_t calibrationFactor);

    char* aioKey();
    void aioKey(String aioKey);

    uint8_t numReadingsForStable();
    void numReadingsForStable(uint8_t readings);

    uint32_t noVisitAlertTime();
    void noVisitAlertTime(uint32_t secs);

private:
    ScaleConfig();
    void save();

    struct ScaleCfg
    {
        uint32_t magic;
        int32_t calibration_factor;
        char aio_key[AIO_KEY_LEN + 1];
        uint8_t num_readings_for_stable;
        uint32_t no_visit_alert_time;
    };

    ScaleCfg mScaleCfg;
};

#endif