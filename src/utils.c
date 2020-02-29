#include "utils.h"

#include "hwlibs.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static char strbuf[256];    // String buffer

void utilInit(void)
{
    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    }
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void utilmDelay(uint32_t ms)
{
    LL_mDelay(ms);
}

void utiluDelay(uint32_t us)
{
    uint32_t tickNow = DWT->CYCCNT;
    uint32_t ticksInMhz = SystemCoreClock / 1000000;
    uint32_t ticksWait = us * ticksInMhz;

    while (DWT->CYCCNT - tickNow < ticksWait);
}

void utilnDelay(uint32_t ns)
{

    if (ns > 10000) {
        utiluDelay(ns / 1000);
    } else {
        uint32_t tickNow = DWT->CYCCNT;
        uint32_t ticksInMhz = SystemCoreClock / 1000000;
        uint32_t ticksWait = ns * ticksInMhz / 1000;

        while (DWT->CYCCNT - tickNow < ticksWait);
    }
}

char *utilMkStr(const char *fmt, ...)
{
    char *buffer = strbuf;

    va_list args;
    va_start(args, fmt);
    vsprintf(buffer, fmt, args);
    va_end (args);

    return buffer;
}

bool utilReadChar(LineParse *lp, char ch)
{
    switch (ch) {
    case '\n':
    case '\r':
        if (lp->idx == 0) {
            break;
        }
        lp->line[lp->idx] = 0;
        lp->idx = 0;
        return true;
    default:
        lp->line[lp->idx++] = ch;
        if (lp->idx >= LINE_SIZE) {
            lp->line[--lp->idx] = 0;
        }
        break;
    }

    return false;
}
