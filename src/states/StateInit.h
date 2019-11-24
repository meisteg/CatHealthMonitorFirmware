#ifndef STATE_INIT_H
#define STATE_INIT_H

#include "application.h"

#include "State.h"

class StateInit : public State
{
public:
    String getName() override;
    void enter() override;
};

#endif
