#include "vac.h"

volatile VacContext vacCtx;

void vacInit(void)
{
    vacCtx.state = VAC_OFF;
    vacCtx.timer = 0;
}

void vacSetTimer(int32_t value)
{
    vacCtx.timer = value;
}

void vacSetState(VacState value)
{
    vacCtx.state = value;
}

void vacUpdateTimers()
{
    if (vacCtx.state == VAC_ON) {
        if (vacCtx.timer > 0) {
            vacCtx.timer--;
        }
    }
}

int32_t vacGetTimer()
{
    return vacCtx.timer;
}
