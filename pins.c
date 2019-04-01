#include "pins.h"
#include <stdint.h>

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_cortex.h>
#include <stm32f1xx_ll_rcc.h>
#include <stm32f1xx_ll_system.h>
#include <stm32f1xx_ll_utils.h>

static void pinsInitDisplay(void)
{
    LL_GPIO_InitTypeDef initDef;

    initDef.Mode = LL_GPIO_MODE_OUTPUT;
    initDef.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    initDef.OutputType = LL_GPIO_OUTPUT_PUSHPULL;

#if defined(_DISP_HI_BYTE) || defined(_DISP_16BIT)
    initDef.Pin = DISP_DATA_HI_Pin;
    LL_GPIO_Init(DISP_DATA_HI_Port, &initDef);
#endif
#if defined(_DISP_LO_BYTE) || defined(_DISP_16BIT)
    initDef.Pin = DISP_DATA_LO_Pin;
    LL_GPIO_Init(DISP_DATA_LO_Port, &initDef);
#endif
    OUT_INIT(DISP_RS, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);
    OUT_INIT(DISP_CS, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);
    OUT_INIT(DISP_WR, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);
    OUT_INIT(DISP_BCKL, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);
#ifdef _DISP_READ_ENABLED
    OUT_INIT(DISP_RD, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);
#endif
#ifdef _DISP_RST_ENABLED
    OUT_INIT(DISP_RST, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);
#endif
#if defined(_DISP_SPI)
    ALT_INIT(DISP_SPI_SDI, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);
    ALT_INIT(DISP_SPI_SCK, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);
    OUT_INIT(DISP_SPI_DC,  LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);
    OUT_INIT(DISP_CS,  LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);
    OUT_INIT(DISP_BCKL, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_HIGH);
#endif
}

void pinsInit(void)
{
    // NOJTAG: JTAG-DP Disabled and SW-DP Enabled
    LL_GPIO_AF_Remap_SWJ_NOJTAG();

    // Enable clock for all GPIO peripherials
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);

    OUT_INIT(LED1, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_LOW);
    OUT_INIT(LED3, LL_GPIO_OUTPUT_PUSHPULL, LL_GPIO_SPEED_FREQ_LOW);

    pinsInitDisplay();
}
