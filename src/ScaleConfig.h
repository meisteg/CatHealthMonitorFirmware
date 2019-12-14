#ifndef SCALE_CONFIG_H
#define SCALE_CONFIG_H

#include "application.h"

class ScaleConfig
{
public:
    static ScaleConfig* get();

    int32_t calibrationFactor();
    void calibrationFactor(int32_t calibrationFactor);

private:
    ScaleConfig();

    struct ScaleCfg
    {
        uint32_t magic;
        int32_t calibration_factor;
    };

    ScaleCfg mScaleCfg;
};

#endif