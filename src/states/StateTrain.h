#ifndef STATE_TRAIN_H
#define STATE_TRAIN_H

#include "application.h"

#include "State.h"

class StateTrain : public State
{
public:
    StateTrain();
    String getName() override;
    void processReading(float reading) override;
    void exit() override;

private:
    int mNumSameReadings;
    float mPrevReading;
};

#endif
