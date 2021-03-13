/*
 * Copyright (C) 2019-2021 Gregory S. Meiste  <http://gregmeiste.com>
 */

#include <math.h>

#include "application.h"
#include "CatManager.h"
#include "Constants.h"
#include "ScaleConfig.h"

// Flag to indicate that the no visit alert has already been sent
#define CAT_FLAG_NO_VISIT_ALERT_SENT   0x0001

CatManager* CatManager::get()
{
    static CatManager catManager;
    return &catManager;
}

#if USE_ADAFRUIT_IO
CatManager::CatManager() : mSelectedCat(-1), mAIOClient(mTCPClient, ScaleConfig::get()->aioKey())
#else
CatManager::CatManager() : mSelectedCat(-1)
#endif
{
    readCatDatabase();

#if USE_ADAFRUIT_IO
    mAIOClient.begin();
#endif
}

void CatManager::readCatDatabase()
{
    EEPROM.get(CAT_DATABASE_ADDR, mCatDataBase);
    if ((mCatDataBase.magic != CAT_MAGIC_NUMBER) || (mCatDataBase.num_cats > MAX_NUM_CATS))
    {
        SERIAL.println("CatManager: EEPROM was empty or invalid");
        mCatDataBase.magic = CAT_MAGIC_NUMBER;
        mCatDataBase.num_cats = 0;
        mCatDataBase.version = 1;
        EEPROM.put(CAT_DATABASE_ADDR, mCatDataBase);
    }

    // Check if the database needs to be upgraded to a new version schema
    if (mCatDataBase.version < 1)
    {
        SERIAL.println("Updating cat database to version 1");

        // Version 1 introduced the flags attribute to each cat. Need to set to known value.
        for (int i = 0; i < mCatDataBase.num_cats; ++i)
        {
            mCatDataBase.cats[i].flags = 0;
        }

        mCatDataBase.version = 1;
        EEPROM.put(CAT_DATABASE_ADDR, mCatDataBase);
    }
}

void CatManager::reset()
{
    if (mCatDataBase.num_cats)
    {
        mCatDataBase.num_cats = 0;
        EEPROM.put(CAT_DATABASE_ADDR, mCatDataBase);
        mSelectedCat = -1;

        publishCatDatabase();
    }
    else
    {
        SERIAL.println("No cats found in database");
    }
}

bool CatManager::ableToTrain(const String& cat_name) const
{
    // Verify room in database and new cat's name isn't too long
    if ((mCatDataBase.num_cats < MAX_NUM_CATS) && (cat_name.length() < MAX_CAT_NAME_LEN))
    {
        // Check if any existing cats have the same name
        for (int i = 0; i < mCatDataBase.num_cats; ++i)
        {
            if (strncmp(mCatDataBase.cats[i].name, cat_name.c_str(), MAX_CAT_NAME_LEN) == 0)
            {
                // Already have a cat with the same name in the database
                return false;
            }
        }

        // All good!
        return true;
    }

    // Either the database is full, or the new name is too long
    return false;
}

bool CatManager::setupToTrain(const String& cat_name)
{
    bool ret = ableToTrain(cat_name);
    if (ret)
    {
        CatDataBaseEntry* entry = &(mCatDataBase.cats[mCatDataBase.num_cats]);
        cat_name.getBytes((unsigned char *)entry->name, sizeof(entry->name));
        entry->weight = 0.0f;
        entry->last_visit = 0;
        entry->last_duration = 0;
        entry->last_deposit = 0.0f;
    }

    return ret;
}

bool CatManager::completeTraining(float weight)
{
    // Check if matches an existing cat's weight. If so, reject.
    bool ret = !selectCatByWeight(weight);
    if (ret)
    {
        mSelectedCat = mCatDataBase.num_cats++;
        mCatDataBase.cats[mSelectedCat].weight = weight;
        mCatDataBase.cats[mSelectedCat].last_visit = Time.now();
        EEPROM.put(CAT_DATABASE_ADDR, mCatDataBase);

        publishCatDatabase();
    }

    return ret;
}

bool CatManager::publishCatDatabase() const
{
    char publish[512];
    char entry[80];

    snprintf(publish, sizeof(publish), "{\"num_cats\":%u, \"version\":%u, \"cats\":{",
             mCatDataBase.num_cats, mCatDataBase.version);

    for (int i = 0; i < mCatDataBase.num_cats; ++i)
    {
        snprintf(entry, sizeof(entry), "%s\"%s\":{\"weight\":%.1f,\"last_visit\":%ld}",
                 (i > 0) ? ", " : "", mCatDataBase.cats[i].name, mCatDataBase.cats[i].weight, mCatDataBase.cats[i].last_visit);
        strncat(publish, entry, sizeof(publish) - strlen(publish));
    }

    strncat(publish, "}}", sizeof(publish) - strlen(publish));
    SERIAL.println(publish);

    return Particle.publish("cat_database", publish, PRIVATE);
}

bool CatManager::selectCatByWeight(float weight)
{
    deselectCat();

    for (int i = 0; i < mCatDataBase.num_cats; ++i)
    {
        if (fabs(mCatDataBase.cats[i].weight - weight) <= MAX_CAT_WEIGHT_CHANGE)
        {
            if (mSelectedCat >= 0)
            {
                SERIAL.println("ERROR: Multiple cats too close in weight");
                Particle.publish("cat_alert", "{\"msg\": \"Multiple cats too close in weight!\"}", PRIVATE);
                mSelectedCat = -1;
                break;
            }

            mSelectedCat = i;
        }
    }

    if (mSelectedCat >= 0)
    {
        SERIAL.printlnf("%s selected", mCatDataBase.cats[mSelectedCat].name);
        setCatWeight(weight);

        return true;
    }

    return false;
}

bool CatManager::changeSelectedCatIfNecessary(float weight)
{
    bool ret = false;

    for (int i = 0; i < mCatDataBase.num_cats; ++i)
    {
        if ((mSelectedCat != i) && (fabs(mCatDataBase.cats[i].weight - weight) <= MAX_CAT_WEIGHT_CHANGE))
        {
            ret = selectCatByWeight(weight);
            break;
        }
    }

    return ret;
}

void CatManager::deselectCat()
{
    mSelectedCat = -1;

    // In case changes were made to the selected cat, restore data from cat database.
    readCatDatabase();
}

bool CatManager::setCatWeight(float weight)
{
    if (mSelectedCat >= 0)
    {
        if (mCatDataBase.cats[mSelectedCat].weight != weight)
        {
            SERIAL.printlnf("Updating weight to %.2f lbs", weight);
            mCatDataBase.cats[mSelectedCat].weight = weight;
        }

        return true;
    }

    return false;
}

bool CatManager::setCatLastDuration(uint16_t duration)
{
    if (mSelectedCat >= 0)
    {
        mCatDataBase.cats[mSelectedCat].last_duration = duration;
        mCatDataBase.cats[mSelectedCat].last_visit = Time.now();

        return true;
    }

    return false;
}

bool CatManager::setCatLastDeposit(float deposit)
{
    if (mSelectedCat >= 0)
    {
        mCatDataBase.cats[mSelectedCat].last_deposit = deposit;
        return true;
    }

    return false;
}

bool CatManager::publishCatVisit()
{
    char publish[255];
    CatDataBaseEntry* entry;
    bool ret = false;

#if USE_ADAFRUIT_IO
    char feed[64];
#endif

    if (mSelectedCat >= 0)
    {
        entry = &(mCatDataBase.cats[mSelectedCat]);

        // Clear no visit alert flag
        entry->flags &= ~(CAT_FLAG_NO_VISIT_ALERT_SENT);

        // Save updates to cat database
        EEPROM.put(CAT_DATABASE_ADDR, mCatDataBase);

        if (entry->last_duration > 60)
        {
            int mins = (entry->last_duration / 60);
            int secs = (entry->last_duration % 60);
            snprintf(publish, sizeof(publish),
                     "{\"cat\": \"%s\", \"weight\": %.1f, \"duration\": %d, \"duration_str\": \"%d minute%s %d second%s\", \"deposit\": %.1f}",
                     entry->name, entry->weight, entry->last_duration, mins,
                     (mins > 1) ? "s" : "", secs, (secs == 1) ? "" : "s", entry->last_deposit);
        }
        else
        {
            snprintf(publish, sizeof(publish),
                     "{\"cat\": \"%s\", \"weight\": %.1f, \"duration\": %d, \"duration_str\": \"%d seconds\", \"deposit\": %.1f}",
                     entry->name, entry->weight, entry->last_duration,
                     entry->last_duration, entry->last_deposit);
        }

        SERIAL.printlnf("Publishing: %s", publish);
#if IS_MASTER_DEVICE
        ret = Particle.publish("cat_visit", publish, PRIVATE);
        if (!ret)
        {
            SERIAL.println("Failed to publish to Particle!");
        }
#else
        ret = true;
#endif

#if USE_ADAFRUIT_IO
        // Build Adafruit IO feed name
        snprintf(feed, sizeof(feed), "cat-health-monitor.%s-weight", String(entry->name).toLowerCase().c_str());
        Adafruit_IO_Feed aioFeed = mAIOClient.getFeed(feed);

        // Build Adafruit IO publish string
        snprintf(publish, sizeof(publish), "%.1f", entry->weight);

        // Send to Adafruit IO
        if (!aioFeed.send(publish))
        {
            SERIAL.println("Failed to publish to Adafruit IO!");
            ret = false;
        }
#endif

        mSelectedCat = -1;
    }

    return ret;
}

void CatManager::checkLastCatVisits()
{
#if IS_MASTER_DEVICE
    uint32_t alertTime = ScaleConfig::get()->noVisitAlertTime();
    CatDataBaseEntry* entry;

    if (alertTime > 0)
    {
        time_t now = Time.now();

        for (int i = 0; i < mCatDataBase.num_cats; ++i)
        {
            entry = &(mCatDataBase.cats[i]);

            if (((uint32_t)(now - entry->last_visit) > alertTime) &&
                !(entry->flags & CAT_FLAG_NO_VISIT_ALERT_SENT))
            {
                entry->flags |= CAT_FLAG_NO_VISIT_ALERT_SENT;
                EEPROM.put(CAT_DATABASE_ADDR, mCatDataBase);

                char publish[128];
                snprintf(publish, sizeof(publish), "{\"msg\": \"%s has not visited for over %lu hours.\"}",
                         entry->name, alertTime / 3600);
                Particle.publish("cat_alert", publish, PRIVATE);
            }
        }
    }
#endif
}
