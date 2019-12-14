/*
 * Copyright (C) 2019 Gregory S. Meiste  <http://gregmeiste.com>
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

private:
    ScaleConfig();
    void save();

    struct ScaleCfg
    {
        uint32_t magic;
        int32_t calibration_factor;
        char aio_key[AIO_KEY_LEN + 1];
    };

    ScaleCfg mScaleCfg;
};

#endif