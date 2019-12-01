#ifndef CAT_MANAGER_H
#define CAT_MANAGER_H

#include "Constants.h"

class CatManager
{
public:
    // Constructor
    CatManager();

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

    // Sets the duration (in milliseconds) of the selected cat's last visit
    bool setCatLastDuration(uint32_t duration);

    // Sets the amount of deposit for the selected cat's last visit
    bool setCatLastDeposit(float deposit);

    // Publish event for selected cat's visit
    bool publishCatVisit();

private:
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
};

CatManager* getCatManager();

#endif
