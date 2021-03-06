/*
 * Copyright (C) 2019-2021 Gregory S. Meiste  <http://gregmeiste.com>
 */

#include <math.h>

#include "StateEmpty.h"
#include "StateManager.h"
#include "Constants.h"
#include "CatManager.h"
#include "ScaleConfig.h"

StateEmpty::StateEmpty() : mSentBatteryWarning(false), mWasBatteryCharging(false)
{
    // Nothing to do
}

String StateEmpty::getName()
{
    return "EMPTY";
}

void StateEmpty::processReading(CatScale *scale)
{
    float pounds = scale->getPounds(true);

    if ((pounds != mPrevReadingPounds) || (pounds == 0.0f))
    {
        mNumSameNonZeroReadingsPounds = 0;
        mPrevReadingPounds = pounds;

        if (pounds == 0.0f)
        {
            // In order to have an accurate deposit check, need to have minimal scale drift.
            float grams = scale->getGrams(true);
            if ((grams != mPrevReadingGrams) || (fabs(grams) < MAX_DRIFT_GRAMS))
            {
                mNumSameNonZeroReadingsGrams = 0;
                mPrevReadingGrams = grams;

                // Readings taken immediately after waking up seem to show more drift.
                // Only go back to ULP if drift looks good.
                mCanEnterUlp = (fabs(grams) < MAX_DRIFT_GRAMS);
            }
            else
            {
                mNumSameNonZeroReadingsGrams++;
                if (mNumSameNonZeroReadingsGrams >= ScaleConfig::get()->numReadingsForStable())
                {
                    // Scale drift, cat deposits or litter box cleaning
                    Log.info("Automatic tare due to non-zero grams: %.0f", grams);
                    StateManager::get()->setState(StateManager::STATE_TARE);
                }
            }
        }
    }
    else
    {
        mNumSameNonZeroReadingsPounds++;

        // Flag the network as needed first as it's likely going to be
        // needed very soon and don't want to wait too long for it. However,
        // it needs to be available before stable reading can be processed.
        if (networkNeeded() && (mNumSameNonZeroReadingsPounds >= ScaleConfig::get()->numReadingsForStable()))
        {
            // Is it a cat?
            if (CatManager::get()->selectCatByWeight(pounds))
            {
                StateManager::get()->setState(StateManager::STATE_CAT_PRESENT);
            }
            else
            {
                char publishString[64];
                snprintf(publishString, sizeof(publishString),
                         "{\"reading\": %.1f}", pounds);
                Particle.publish("stable_reading", publishString, PRIVATE);

                // Is it possible to be a cat?
                if (pounds >= MIN_CAT_WEIGHT_LBS)
                {
                    StateManager::get()->setState(StateManager::STATE_CAT_POSSIBLE);
                }
                // Scale drift, cat deposits or litter box cleaning
                else
                {
                    Log.info("Automatic tare due to non-zero pounds: %.1f", pounds);
                    StateManager::get()->setState(StateManager::STATE_TARE);
                }
            }
        }
    }
}

void StateEmpty::enter()
{
    mNumSameNonZeroReadingsPounds = 0;
    mPrevReadingPounds = 0.0f;

    mNumSameNonZeroReadingsGrams = 0;
    mPrevReadingGrams = 0.0f;

    mTimeNetworkNeeded = millis();
    mCanEnterUlp = false;
    mConnectingToParticle = false;

    // Enable OTA updates while empty
    System.enableUpdates();

    if (Particle.connected()) CatManager::get()->requestDatabase();
}

void StateEmpty::loop()
{
    if (Particle.connected()) CatManager::get()->checkLastCatVisits();

    checkBatteryState();
    pwrManagement();
}

void StateEmpty::checkBatteryState()
{
    char publish[128];

    CatScale *scale = CatScale::get();
    if (scale->isUsbPowered())
    {
        // Reset battery warning flag
        mSentBatteryWarning = false;

        bool isCharging = scale->isCharging();
        if (mWasBatteryCharging && !isCharging && (scale->getBatteryPercent() >= 100))
        {
            if (networkNeeded())
            {
                snprintf(publish, sizeof(publish),
                        "{\"msg\": \"Battery charging is complete! You may now unplug.\"}");
                Particle.publish("cat_alert", publish, PRIVATE);

                mWasBatteryCharging = isCharging;
            }
        }
        else
        {
            mWasBatteryCharging = isCharging;
        }
    }
    else
    {
        if ((scale->getBatteryPercent() < BATTERY_WARN_PERCENT) && !mSentBatteryWarning && networkNeeded())
        {
            snprintf(publish, sizeof(publish),
                     "{\"msg\": \"Battery less than %u percent. Please plug in to charge.\"}",
                     BATTERY_WARN_PERCENT);
            mSentBatteryWarning = Particle.publish("cat_alert", publish, PRIVATE);
        }

        mWasBatteryCharging = false;
    }
}

void StateEmpty::pwrManagement()
{
    CatScale *scale = CatScale::get();

#if STAY_CONNECTED_WHEN_ON_USB
    bool isUsbPowered = scale->isUsbPowered();

    // If we are plugged in, always be connected to the network
    if (isUsbPowered) networkNeeded();
#endif

    if (Particle.connected())
    {
        // Connection successful - clear connecting flag
        mConnectingToParticle = false;
    }

    // Disable network after idle delay
    if (((millis() - mTimeNetworkNeeded) > NETWORK_DISABLE_DELAY_MS)
#if STAY_CONNECTED_WHEN_ON_USB
        && !isUsbPowered
#endif
    )
    {
        if (Particle.connected())
        {
            Log.info("Disconnecting from Particle network");
            Particle.disconnect();
        }
        else if (WiFi.ready())
        {
            Log.info("Disconnecting from WiFi");
            WiFi.off();
        }
        else if (mCanEnterUlp)
        {
            Log.info("Entering ULTRA_LOW_POWER mode");

            // Workaround issue where LED would flash when exiting ULP.
            // Issue doesn't occur when LED under user control (true).
            RGB.control(true);

            // Turn off external scale circuit to save power
            scale->powerDown();

            SystemSleepConfiguration config;
            config.mode(SystemSleepMode::ULTRA_LOW_POWER).duration(ULP_DURATION_MS);
            System.sleep(config);

            // Turn on the external scale circuit to prepare for next reading
            scale->powerUp();

            // Let the system have control over the LED again.
            RGB.control(false);

            // Need another good reading before entering ULP again
            mCanEnterUlp = false;
        }
    }
}

bool StateEmpty::networkNeeded()
{
    mTimeNetworkNeeded = millis();
    bool available = Particle.connected();

    // There is no Particle.connecting() function that can be used to check that
    // Particle.connect() has already been called, so use connecting flag.
    if (!available && !mConnectingToParticle)
    {
        Log.info("Connecting to Particle network");
        Particle.connect();
        mConnectingToParticle = true;
    }

    return available;
}
