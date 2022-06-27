#include "vac.h"

#include "input.h"
#include "hwlibs.h"
#include "ringbuf.h"
#include "string.h"
#include "usart.h"
#include "utils.h"

#include <stdio.h>

static Event event = {
    .type = EVENT_NONE,
    .value = 0,
};

volatile VacContext vacCtx;
RingBuf rbuf;
char rBufData[100];
static LineParse lp;


void vacInit(void)
{
    vacCtx.state = VAC_OFF;
    vacCtx.timer = 0;

    ringBufInit(&rbuf, rBufData, sizeof(rBufData));
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

static void parseVac(char *line)
{
    int ret;

    if (utilIsPrefix(line, "pause")) {
        event.type = EVENT_PAUSE_TIMER;
    } else if (utilIsPrefix(line, "continue")) {
        event.type = EVENT_CONTINUE_TIMER;
    } else if (utilIsPrefixInt(line, "start ", &ret)) {
        event.type = EVENT_START_TIMER;
        event.value = ret * 1000;
    }
}

static void parseLine(char *line)
{
    if (utilIsPrefix(line, "vac.")) {
        parseVac(line + strlen("vac."));
    }
}

void vacGetData(void)
{
    uint16_t size = ringBufGetSize(&rbuf);

    for (uint16_t i = 0; i < size; i++) {
        char ch = ringBufPopChar(&rbuf);
        if (utilReadChar(&lp, ch)) {
            parseLine(lp.line);
        }
    }
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

    vacGetData();
}

void vacEventHandle()
{
    char buf[32];

    switch (event.type) {
    case EVENT_PAUSE_TIMER:
        vacCtx.state = VAC_OFF;
        break;
    case EVENT_CONTINUE_TIMER:
        vacCtx.state = VAC_ON;
        break;
    case EVENT_START_TIMER:
        vacCtx.state = VAC_OFF;
        vacCtx.timer = event.value;
        snprintf(buf, sizeof(buf), "Timer reset to %d s\r\n", (int)vacCtx.timer/1000);
        usartSendString(USART_VAC, buf);

        break;
    default:
        break;
    }

    event.type = EVENT_NONE;
}

void USART_VAC_HANDLER(void)
{
    // Check RXNE flag value in SR register
    if (LL_USART_IsActiveFlag_RXNE(USART_VAC) && LL_USART_IsEnabledIT_RXNE(USART_VAC)) {
        char data = LL_USART_ReceiveData8(USART_VAC);
//        usartSendChar(USART_VAC, data);
        ringBufPushChar(&rbuf, data);
    } else {
        // Call Error function
    }
}
