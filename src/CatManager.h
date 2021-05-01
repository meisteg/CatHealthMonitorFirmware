/*
 * Copyright (C) 2019-2021 Gregory S. Meiste  <http://gregmeiste.com>
 */

#ifndef CAT_MANAGER_H
#define CAT_MANAGER_H

#include "Constants.h"

#if USE_ADAFRUIT_IO
#include "Adafruit_IO_Client.h"
#endif

class CatManager
{
public:
    // Get the CatManager singleton
    static CatManager* get();

    // Perform initial setup: subscribe to database events and publish
    // the current database
    void setup();

    // Removes all saved cats from memory
    void reset();

    // Returns true if able to train another cat
    bool ableToTrain(const String& cat_name) const;

    // Setup to train a new cat
    // Returns true if setup is successful
    bool setupToTrain(const String& cat_name);

    // Completes a cat's training and saves new cat into the database
    // Returns true on success, and false if weight matches existing cat
    bool completeTraining(float weight);

    // Selects a cat in the database with the specified weight (ish)
    // Returns true if cat selected, false if no cat with that weight found
    bool selectCatByWeight(float weight);

    // Updates cat selection if the specified weight indicates another cat.
    // Returns true if the cat selection changed, false if not.
    bool changeSelectedCatIfNecessary(float weight);

    // Deselects selected cat (if any). Will lose any unsaved changes to cat.
    void deselectCat();

    // Sets the weight for the selected cat.
    // Returns false if no cat selected.
    bool setCatWeight(float weight);

    // Sets the duration (in seconds) of the selected cat's last visit.
    // Returns false if no cat selected.
    bool setCatLastDuration(uint16_t duration);

    // Sets the amount of deposit for the selected cat's last visit.
    // Returns false if no cat selected.
    bool setCatLastDeposit(float deposit);

    // Publish event for selected cat's visit
    bool publishCatVisit();

    // Checks the last visits of the cats in the database. Send an alert if the
    // last visit occured too long ago. Only functional on the master device.
    void checkLastCatVisits();

    // Requests latest cat database from master device. Only functional on the
    // slave device.
    bool requestDatabase();

private:
    // Constructor
    CatManager();

    // Reads cat database from the EEPROM
    void readCatDatabase();

    // Publish the cat database if the master device
    bool publishCatDatabase() const;

    // Handles the cat database events from other devices
    void subscriptionHandler(const char *event, const char *data);

    // Returns true if a cat is selected, else false
    bool isCatSelected() { return (mSelectedCat >= 0); };

    // Update cat database using provided JSON
    bool updateCatDatabaseJson(const char *json);

    struct CatDataBaseEntry
    {
        char name[MAX_CAT_NAME_LEN];
        float weight;
        time32_t last_visit;
        uint16_t flags;
        uint16_t last_duration;
        float last_deposit;
    };

    struct CatDataBase
    {
        // Magic number used to verify that database has been initialized
        uint32_t magic;

        // Number of cats in the database
        uint8_t num_cats;

        // Version of the database schema
        uint8_t version;

        // Reserved for future use
        uint16_t reserved;

        // Cat entries
        CatDataBaseEntry cats[MAX_NUM_CATS];
    };

    CatDataBase mCatDataBase;

    // Currently selected cat in the cat database
    int mSelectedCat;

#if USE_ADAFRUIT_IO
    // TCP Client used by Adafruit IO library
    TCPClient mTCPClient;

    // Adafruit IO Client
    Adafruit_IO_Client mAIOClient;
#endif
};

#endif
