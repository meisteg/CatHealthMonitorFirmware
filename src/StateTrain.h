#ifndef STATE_TRAIN_H
#define STATE_TRAIN_H

#include "application.h"

#include "State.h"

class StateTrain : public State
{
public:
    StateTrain();
    String getName();
    void newReading(float reading);

private:
    int mNumSameReadings;
    float mPrevReading;
};

#endif
