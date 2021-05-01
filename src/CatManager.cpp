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
        Log.error("CatManager: EEPROM was empty or invalid");
        mCatDataBase.magic = CAT_MAGIC_NUMBER;
        mCatDataBase.num_cats = 0;
        mCatDataBase.version = 1;
        EEPROM.put(CAT_DATABASE_ADDR, mCatDataBase);
    }

    // Check if the database needs to be upgraded to a new version schema
    if (mCatDataBase.version < 1)
    {
        Log.info("Updating cat database to version 1");

        // Version 1 introduced the flags attribute to each cat. Need to set to known value.
        for (int i = 0; i < mCatDataBase.num_cats; ++i)
        {
            mCatDataBase.cats[i].flags = 0;
        }

        mCatDataBase.version = 1;
        EEPROM.put(CAT_DATABASE_ADDR, mCatDataBase);
    }
}

void CatManager::setup()
{
    if (ScaleConfig::get()->isMaster())
    {
        // Master devices handle cat database requests (slave_req_cat_db) and
        // cat visits to slave devices (slave_cat_visit)
        Particle.subscribe("slave_", &CatManager::subscriptionHandler, this);
    }
    else
    {
        // Slave devices consume the cat database.
        Particle.subscribe("cat_database", &CatManager::subscriptionHandler, this);
    }

    publishCatDatabase();
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
        Log.info("No cats found in database");
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
    char buf[512];

    memset(buf, 0, sizeof(buf));
    JSONBufferWriter writer(buf, sizeof(buf) - 1);

    writer.beginObject();
    writer.name("num_cats").value(mCatDataBase.num_cats);
    writer.name("version").value(mCatDataBase.version);
    writer.name("cats").beginObject();

    for (int i = 0; i < mCatDataBase.num_cats; ++i)
    {
        writer.name(mCatDataBase.cats[i].name).beginObject();
        writer.name("weight").value(mCatDataBase.cats[i].weight, 1);
        writer.name("last_visit").value((int)(mCatDataBase.cats[i].last_visit));
        writer.endObject();
    }

    writer.endObject(); // cats
    writer.endObject(); // top level

    Log.info(buf);

    // Only publish database to the cloud if a master device.
    if (ScaleConfig::get()->isMaster())
    {
        return Particle.publish("cat_database", buf, PRIVATE);
    }
    return false;
}

bool CatManager::selectCatByWeight(float weight)
{
    deselectCat();

    for (int i = 0; i < mCatDataBase.num_cats; ++i)
    {
        if (fabs(mCatDataBase.cats[i].weight - weight) <= MAX_CAT_WEIGHT_CHANGE)
        {
            if (isCatSelected())
            {
                Log.error("Multiple cats too close in weight");
                Particle.publish("cat_alert", "{\"msg\": \"Multiple cats too close in weight!\"}", PRIVATE);
                mSelectedCat = -1;
                break;
            }

            mSelectedCat = i;
        }
    }

    if (isCatSelected())
    {
        Log.info("%s selected", mCatDataBase.cats[mSelectedCat].name);
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
    if (isCatSelected())
    {
        if (mCatDataBase.cats[mSelectedCat].weight != weight)
        {
            Log.info("Updating weight to %.2f lbs", weight);
            mCatDataBase.cats[mSelectedCat].weight = weight;
        }

        return true;
    }

    return false;
}

bool CatManager::setCatLastDuration(uint16_t duration)
{
    if (isCatSelected())
    {
        mCatDataBase.cats[mSelectedCat].last_duration = duration;
        mCatDataBase.cats[mSelectedCat].last_visit = Time.now();

        return true;
    }

    return false;
}

bool CatManager::setCatLastDeposit(float deposit)
{
    if (isCatSelected())
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

    if (isCatSelected())
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

        Log.info("Publishing: %s", publish);
        ret = Particle.publish(ScaleConfig::get()->isMaster() ? "cat_visit" : "slave_cat_visit", publish, PRIVATE);
        if (!ret)
        {
            Log.error("Failed to publish to Particle!");
        }

#if USE_ADAFRUIT_IO
        // Build Adafruit IO feed name
        snprintf(feed, sizeof(feed), "cat-health-monitor.%s-weight", String(entry->name).toLowerCase().c_str());
        Adafruit_IO_Feed aioFeed = mAIOClient.getFeed(feed);

        // Build Adafruit IO publish string
        snprintf(publish, sizeof(publish), "%.1f", entry->weight);

        // Send to Adafruit IO
        if (!aioFeed.send(publish))
        {
            Log.error("Failed to publish to Adafruit IO!");
            ret = false;
        }
#endif

        mSelectedCat = -1;
    }

    return ret;
}

void CatManager::checkLastCatVisits()
{
    ScaleConfig* scaleCfg = ScaleConfig::get();

    if (scaleCfg->isMaster())
    {
        uint32_t alertTime = scaleCfg->noVisitAlertTime();
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
    }
}

bool CatManager::requestDatabase()
{
    if (!ScaleConfig::get()->isMaster())
    {
        return Particle.publish("slave_req_cat_db", PRIVATE);
    }

    return false; // No request made
}

void CatManager::subscriptionHandler(const char *event, const char *data)
{
    // Slave device reporting a cat visit
    if (strcmp("slave_cat_visit", event) == 0)
    {
        slaveCatVisit(data);
    }
    // Remaining events cannot be handled if a cat is selected. If a cat is
    // selected, ignore the event.
    else if (isCatSelected())
    {
        Log.warn("Event (%s) ignored due to cat selected", event);
    }
    // Slave request for cat database from master
    else if (strcmp("slave_req_cat_db", event) == 0)
    {
        publishCatDatabase();
    }
    // Cat database response from master to slave
    else if (strcmp("cat_database", event) == 0)
    {
        updateCatDatabaseJson(data);
    }
    else
    {
        Log.error("Event (%s) not recognized!", event);
    }
}

bool CatManager::slaveCatVisit(const char *json)
{
    if (!json)
    {
        Log.error("JSON is NULL!");
        return false;
    }

    CatDataBaseEntry catVisit;
    JSONValue outerObj = JSONValue::parseCopy(json);
    JSONObjectIterator iter(outerObj);
    while(iter.next())
    {
        if (iter.name() == "cat")
        {
            String name(iter.value().toString());
            name.getBytes((unsigned char *)catVisit.name, sizeof(catVisit.name));
        }
        else if (iter.name() == "weight")
        {
            catVisit.weight = iter.value().toDouble();
        }
        else if (iter.name() == "duration")
        {
            catVisit.last_duration = iter.value().toInt();
        }
        else if (iter.name() == "deposit")
        {
            catVisit.last_deposit = iter.value().toDouble();
        }
    }

    // Look for cat in the database
    for (int i = 0; i < mCatDataBase.num_cats; ++i)
    {
        if (strncmp(mCatDataBase.cats[i].name, catVisit.name, MAX_CAT_NAME_LEN) == 0)
        {
            mCatDataBase.cats[i].weight = catVisit.weight;
            mCatDataBase.cats[i].last_duration = catVisit.last_duration;
            mCatDataBase.cats[i].last_deposit = catVisit.last_deposit;
            mCatDataBase.cats[i].last_visit = Time.now();

            // Clear no visit alert flag
            mCatDataBase.cats[i].flags &= ~(CAT_FLAG_NO_VISIT_ALERT_SENT);

            // Can only save updates to the cat database when no cat is selected.
            // If currently selected cat, wait to save the changes until active
            // cat visit is completed.
            if (!isCatSelected())
            {
                EEPROM.put(CAT_DATABASE_ADDR, mCatDataBase);
            }

            Log.info("Slave device cat visit handled");
            return Particle.publish("cat_visit", json, PRIVATE);
        }
    }

    Log.error("Slave device cat not found - ignoring visit");
    return false;
}

bool CatManager::updateCatDatabaseJson(const char *json)
{
    if (!json)
    {
        Log.error("JSON is NULL!");
        return false;
    }

    unsigned int i = 0;
    JSONValue outerObj = JSONValue::parseCopy(json);
    JSONObjectIterator iter(outerObj);
    while(iter.next())
    {
        if (iter.name() == "num_cats")
        {
            mCatDataBase.num_cats = iter.value().toInt();
        }
        else if (iter.name() == "version")
        {
            // Is it okay to accept a cat database that is a different version?
            if (iter.value().toInt() != mCatDataBase.version)
            {
                Log.warn("Cat database version mismatch: json=%d, local=%d",
                         iter.value().toInt(), mCatDataBase.version);
            }
        }
        else if (iter.name() == "cats")
        {
            JSONObjectIterator catsIter(iter.value());
            while(catsIter.next())
            {
                CatDataBaseEntry* entry = &(mCatDataBase.cats[i++]);
                String cat_name(catsIter.name());
                cat_name.getBytes((unsigned char *)entry->name, sizeof(entry->name));

                JSONObjectIterator catIter(catsIter.value());
                while(catIter.next())
                {
                    if (catIter.name() == "weight")
                    {
                        entry->weight = catIter.value().toDouble();
                    }
                    else if (catIter.name() == "last_visit")
                    {
                        entry->last_visit = catIter.value().toInt();
                    }
                    else
                    {
                        Log.warn("Unknown cat obj key: %s", (const char *)catIter.name());
                    }
                }
            }
        }
        else
        {
            Log.warn("Unknown outer obj key: %s", (const char *)iter.name());
        }
    }

    // Sanity check that expected number of cats were in the json
    if (i != mCatDataBase.num_cats)
    {
        Log.error("JSON didn't have expected number of cats");
        readCatDatabase();
        return false;
    }

    EEPROM.put(CAT_DATABASE_ADDR, mCatDataBase);
    Log.info("Cat database updated via JSON");

    return true;
}
