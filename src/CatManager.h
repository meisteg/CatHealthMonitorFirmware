/*
 * Copyright (C) 2019-2020 Gregory S. Meiste  <http://gregmeiste.com>
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

    // Prints the cat database to the serial console
    void printCatDatabase() const;

    // Selects a cat in the database with the specified weight (ish)
    // Returns true if cat selected, false if no cat with that weight found
    bool selectCatByWeight(float weight);

    // Deselects selected cat (if any). Will lose any unsaved changes to cat.
    void deselectCat();

    // Sets the weight for the selected cat.
    // Returns false if no cat selected.
    bool setCatWeight(float weight);

    // Sets the duration (in milliseconds) of the selected cat's last visit.
    // Returns false if no cat selected.
    bool setCatLastDuration(uint32_t duration);

    // Sets the amount of deposit for the selected cat's last visit.
    // Returns false if no cat selected.
    bool setCatLastDeposit(float deposit);

    // Publish event for selected cat's visit
    bool publishCatVisit();

private:
    // Constructor
    CatManager();

    // Reads cat database from the EEPROM
    void readCatDatabase();

    struct CatDataBaseEntry
    {
        char name[MAX_CAT_NAME_LEN];
        float weight;
        time_t last_visit;
        uint32_t last_duration;
        float last_deposit;
    };

    struct CatDataBase
    {
        uint32_t magic;
        uint8_t num_cats;
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
