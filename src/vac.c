#include "vac.h"

#include "input.h"

static Event event = {
    .type = EVENT_NONE,
    .value = 0,
};

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

Event vacGetButtons()
{
    Event ret = {.type = EVENT_NONE, .value = 0};

    CmdBtn cmdBtn = inputGetBtnCmd();

    if (cmdBtn.btn) {
        ret.value = cmdBtn.btn;
        if (cmdBtn.flags & BTN_FLAG_LONG_PRESS) {
            ret.type = EVENT_BTN_LONG;
        } else {
            ret.type = EVENT_BTN_SHORT;
        }
    }

    return ret;
}

void vacEventGet()
{
    event = vacGetButtons();

    // Remap buttons events
    if (event.type == EVENT_BTN_SHORT) {
        if (event.value == BTN_D0) {
            switch (vacCtx.state) {
            case VAC_OFF:
                event.type = EVENT_CONTINUE_TIMER;
                break;
            case VAC_ON:
                event.type = EVENT_PAUSE_TIMER;
                break;
            default:
                break;
            }
        }
    }
}

void vacEventHandle()
{
    switch (event.type) {
    case EVENT_PAUSE_TIMER:
        vacCtx.state = VAC_OFF;
        break;
    case EVENT_CONTINUE_TIMER:
        vacCtx.state = VAC_ON;
        break;
    default:
        break;
    }

    event.type = EVENT_NONE;
}
