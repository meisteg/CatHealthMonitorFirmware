#include "StateInit.h"
#include "CatHealthMonitor.h"

String StateInit::getName()
{
    return "INIT";
}

void StateInit::enter()
{
    scale.tare();
    val.reset();
}
