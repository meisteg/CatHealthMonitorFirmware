/*
 * Copyright (C) 2019-2021 Gregory S. Meiste  <http://gregmeiste.com>
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

    bool isMaster();
    void isMaster(bool master);

    char* deviceName();
    void deviceName(String name);

private:
    ScaleConfig();
    void save();

    struct ScaleCfg
    {
        // Magic number used to verify that scale configuration has been initialized
        uint32_t magic;

        // Calibration factor of the scale
        int32_t calibration_factor;

        // Key to use when using the Adafruit IO API
        char aio_key[AIO_KEY_LEN + 1];

        // The number of readings in a row that must match before action is taken
        uint8_t num_readings_for_stable;

        // Time (in seconds) to send an alert if a cat's last visit time exceeds (0 to disable)
        uint32_t no_visit_alert_time;

        // Version of this scale configuraton schema
        uint8_t version;

        // Flags for changing device behavior
        uint8_t flags;

        // Name of the device
        char device_name[DEVICE_NAME_LEN];
    };

    ScaleCfg mScaleCfg;
};

#endif
