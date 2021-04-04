/*
 * Copyright (C) 2019-2021 Gregory S. Meiste  <http://gregmeiste.com>
 */

#ifndef STATE_EMPTY_H
#define STATE_EMPTY_H

#include "application.h"

#include "State.h"

class StateEmpty : public State
{
public:
    StateEmpty();
    String getName() override;
    void processReading(CatScale *scale) override;
    void enter() override;
    void loop() override;

private:
    // Reports battery charged or battery low events to the user as needed.
    void checkBatteryState();

    // Automatically bring network connection up or down as needed.
    void pwrManagement();

    // Signal that the network is needed. Returns true if network is available, false if not.
    bool networkNeeded();

    int mNumSameNonZeroReadingsPounds;
    int mNumSameNonZeroReadingsGrams;
    float mPrevReadingPounds;
    float mPrevReadingGrams;
    bool mSentBatteryWarning;
    bool mWasBatteryCharging;
    system_tick_t mTimeNetworkNeeded;
    bool mCanEnterUlp;
    bool mConnectingToParticle;
};

#endif
