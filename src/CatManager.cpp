#include <math.h>

#include "application.h"
#include "CatManager.h"
#include "Constants.h"

CatManager* getCatManager()
{
    static CatManager catManager;
    return &catManager;
}

CatManager::CatManager() : mSelectedCat(0)
{
    EEPROM.get(CAT_DATABASE_ADDR, mCatDataBase);
    if ((mCatDataBase.magic != CAT_MAGIC_NUMBER) || (mCatDataBase.num_cats > MAX_NUM_CATS))
    {
        Serial.println("CatManager: EEPROM was empty or invalid");
        mCatDataBase.magic = CAT_MAGIC_NUMBER;
        mCatDataBase.num_cats = 0;
        EEPROM.put(CAT_DATABASE_ADDR, mCatDataBase);
    }
}

void CatManager::reset()
{
    if (mCatDataBase.num_cats)
    {
        mCatDataBase.num_cats = 0;
        EEPROM.put(CAT_DATABASE_ADDR, mCatDataBase);
    }
    else
    {
        Serial.println("No cats found in database");
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
    }

    return ret;
}

bool CatManager::completeTraining(float weight)
{
    // Check if matches an existing cat's weight. If so, reject.
    bool ret = !selectCatByWeight(weight);
    if (ret)
    {
        mCatDataBase.cats[mCatDataBase.num_cats++].weight = weight;
        EEPROM.put(CAT_DATABASE_ADDR, mCatDataBase);

        printCatDatabase();
    }

    return ret;
}

void CatManager::printCatDatabase() const
{
    Serial.print(mCatDataBase.num_cats);
    Serial.println(" cats trained");

    for (int i = 0; i < mCatDataBase.num_cats; ++i)
    {
        Serial.print(mCatDataBase.cats[i].name);
        Serial.print(": ");
        Serial.print(mCatDataBase.cats[i].weight);
        Serial.println(" lbs");
    }
}

bool CatManager::selectCatByWeight(float weight)
{
    mSelectedCat = -1;

    for (int i = 0; i < mCatDataBase.num_cats; ++i)
    {
        if (fabs(mCatDataBase.cats[i].weight - weight) <= MAX_CAT_WEIGHT_CHANGE)
        {
            if (mSelectedCat >= 0)
            {
                // Multiple cats too close in weight!
                // TODO: notify user of error
                // NOTE: Consider the training scenario. Is this an error then?
                mSelectedCat = -1;
                break;
            }

            mSelectedCat = i;
        }
    }

    if (mSelectedCat >= 0)
    {
        Serial.print(mCatDataBase.cats[mSelectedCat].name);
        Serial.println(" selected");

        if (mCatDataBase.cats[mSelectedCat].weight != weight)
        {
            Serial.print("Updating weight to ");
            Serial.print(weight);
            Serial.println(" lbs");

            mCatDataBase.cats[mSelectedCat].weight = weight;
            EEPROM.put(CAT_DATABASE_ADDR, mCatDataBase);
        }

        return true;
    }

    return false;
}
