#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_cortex.h>
#include <stm32f1xx_ll_rcc.h>
#include <stm32f1xx_ll_system.h>
#include <stm32f1xx_ll_usart.h>
#include <stm32f1xx_ll_utils.h>

#include "display/glcd.h"

#include <stm32f1xx_ll_utils.h>
#include "pins.h"
#include "usart.h"

#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0    ((uint32_t)0x00000007)
#define NVIC_PRIORITYGROUP_1    ((uint32_t)0x00000006)
#define NVIC_PRIORITYGROUP_2    ((uint32_t)0x00000005)
#define NVIC_PRIORITYGROUP_3    ((uint32_t)0x00000004)
#define NVIC_PRIORITYGROUP_4    ((uint32_t)0x00000003)
#endif

void LL_Init(void)
{
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    // System interrupt init
    NVIC_SetPriority(MemoryManagement_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(BusFault_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(UsageFault_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(SVCall_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(DebugMonitor_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(PendSV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
}

void SystemClock_Config(void)
{
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

    // Configure HSE
    LL_RCC_HSE_Enable();
    // Wait till HSE is ready
    while (LL_RCC_HSE_IsReady() != 1);

    // Configure PLL
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
    LL_RCC_PLL_Enable();
    // Wait till PLL is ready
    while (LL_RCC_PLL_IsReady() != 1);

    // Configure system clock
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    // Wait till System clock is ready
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);

    // Configure APB1/APB2 clock
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

    // Configure SysTick
    LL_Init1msTick(72000000);
    LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
    LL_SetSystemCoreClock(72000000);
    // SysTick_IRQn interrupt configuration
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
}

void SysTick_Handler(void)
{
    static uint16_t tick = 0;

    if (++tick >= 200)
        tick = 0;

    switch (tick) {
    case 0:
        CLR(LED1);
        SET(LED3);
        break;
    case 100:
        CLR(LED3);
        SET(LED1);
        break;
    }
}

void printDispRegs(void)
{
    usartSendString(USART_DBG, "\r========\r\n");
#ifdef _DISP_READ_ENABLED

    const uint8_t num = 10;

    uint16_t args[num];

    for (uint16_t reg = 0x00; reg <= 0xFF; reg++) {
        dispdrvReset();
        dispdrvReadReg(reg, args, num);

        bool hasData = false;
        for (uint8_t i = 0; i < num; i++) {
            if (args[i]) {
                hasData = true;
                break;
            }
        }

        char *str = glcdPrepareNum(reg, 2, '0', 16);
        usartSendString(USART_DBG, str);
        usartSendString(USART_DBG, ": ");

        if (!hasData) {
            usartSendString(USART_DBG, "\r");
            continue;
        }

        for (uint8_t i = 0; i < num; i++) {
            if (args[i]) {
                str = glcdPrepareNum(args[i], 4, '0', 16);
            } else {
                str = "----";
            }
            usartSendString(USART_DBG, str);
            usartSendString(USART_DBG, " ");
        }
        usartSendString(USART_DBG, "\n\r");
    }
#else
    usartSendString(USART_DBG, "\rRead pin is disabled\r\n");
#endif
}

int main(void)
{
    // System
    LL_Init();
    SystemClock_Config();
    LL_SYSTICK_EnableIT();

    pinsInit();

    usartInit(USART_DBG, 115200);
    printDispRegs();

    static Glcd *glcd;
    glcdInit(&glcd);
//    glcdRotate(LCD_ROTATE_180);

    glcdDrawRect(0, 0, 320, 240, LCD_COLOR_BLACK);
    glcdDrawRing(250, 80, 56, 3, LCD_COLOR_WHITE);

    glcdDrawRect(200, 160, 30, 60, LCD_COLOR_RED);
    glcdDrawRect(240, 160, 30, 60, LCD_COLOR_LIME);
    glcdDrawRect(280, 160, 30, 60, LCD_COLOR_BLUE);

    glcdSetFont(&fontterminus32);

    glcdSetFontColor(LCD_COLOR_RED);
    glcdSetXY(10, 10);
    glcdWriteString("Red string");

    glcdSetFontColor(LCD_COLOR_LIME);
    glcdSetXY(10, 90);
    glcdWriteString("Green string");

    glcdSetFontColor(LCD_COLOR_BLUE);
    glcdSetXY(10, 170);
    glcdWriteString("Blue string");

    LL_mDelay(200);

    while (1) {
        glcdDrawCircle(250, 80, 50, LCD_COLOR_RED);
        LL_mDelay(500);
        glcdDrawCircle(250, 80, 50, LCD_COLOR_YELLOW);
        LL_mDelay(500);
        glcdDrawCircle(250, 80, 50, LCD_COLOR_LIME);
        LL_mDelay(500);
        glcdDrawCircle(250, 80, 50, LCD_COLOR_AQUA);
        LL_mDelay(500);
        glcdDrawCircle(250, 80, 50, LCD_COLOR_BLUE);
        LL_mDelay(500);
        glcdDrawCircle(250, 80, 50, LCD_COLOR_MAGENTA);
        LL_mDelay(500);
    }

    return 0;
}
