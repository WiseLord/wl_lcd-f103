#include "pins.h"
#include <stdint.h>

static void pinsInitDisplay(void)
{
    LL_GPIO_InitTypeDef initDef;

    initDef.Mode = LL_GPIO_MODE_OUTPUT;
    initDef.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    initDef.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
#ifdef STM32F3
    initDef.Pull = LL_GPIO_PULL_NO;
#endif

    initDef.Pin = DISP_BCKL_Pin;
    LL_GPIO_Init(DISP_BCKL_Port, &initDef);

    SET(DISP_BCKL);
}

void pinsInit(void)
{
    LL_GPIO_InitTypeDef initDef;

    initDef.Mode = LL_GPIO_MODE_OUTPUT;
    initDef.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    initDef.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
#ifdef STM32F3
    initDef.Pull = LL_GPIO_PULL_NO;
#endif

    initDef.Pin = LED1_Pin;
    LL_GPIO_Init(LED1_Port, &initDef);
    initDef.Pin = LED3_Pin;
    LL_GPIO_Init(LED1_Port, &initDef);

    pinsInitDisplay();
}
