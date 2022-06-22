#include "input.h"

#include "display/glcd.h"
#include "hwlibs.h"
#include "timers.h"

static Input input;

static void inputHandleButtons(uint16_t btnNow)
{
    // Antibounce counter
    static int16_t btnCnt = 0;

    // Previous state
    static uint16_t btnPrev = BTN_NO;

    // On button event place it to command buffer
    if (btnNow) {
        if (btnNow == btnPrev) {
            btnCnt++;
            if (btnCnt == LONG_PRESS) {
                input.btn = btnPrev;
                input.flags |= BTN_FLAG_LONG_PRESS;
            }
        }
    } else {
        if ((btnCnt > SHORT_PRESS) && (btnCnt < LONG_PRESS - AUTOREPEAT)) {
            input.btn = btnPrev;
        }
        btnCnt = 0;
    }
    btnPrev = btnNow;
}

static void inputInitPins(void)
{
    LL_GPIO_InitTypeDef initDef = {0};

    initDef.Mode = LL_GPIO_MODE_INPUT;
    initDef.Pull = LL_GPIO_PULL_UP;

    initDef.Pin = INPUT_Pin;
    LL_GPIO_Init(INPUT_Port, &initDef);
}

void inputInit(void)
{
    inputInitPins();

    timerInit(TIM_INPUT, 199, 359);  // 1kHz polling
}

Input *inputGet()
{
    return &input;
}

void TIM_INPUT_HANDLER(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(TIM_INPUT)) {
        // Clear the update interrupt flag
        LL_TIM_ClearFlag_UPDATE(TIM_INPUT);

        uint8_t dispBus = ~glcdGetBus();

        inputHandleButtons(dispBus & BTN_ALL);
    }
}

CmdBtn inputGetBtnCmd(void)
{
    CmdBtn ret =  { .btn = input.btn, .flags = input.flags };

    input.btn = BTN_NO;
    input.flags = BTN_FLAG_NO;

    return ret;
}
