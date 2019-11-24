#ifndef STATE_CAT_PRESENT_H
#define STATE_CAT_PRESENT_H

#include "application.h"

#include "State.h"

class StateCatPresent : public State
{
public:
    String getName() override;
    void processReading(float reading) override;
    void enter() override;
    void exit() override;

private:
    system_tick_t mTimeEnter;
    int mNumReadingsLessThanThreshold;
};

#endif
