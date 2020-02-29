#include "display/glcd.h"
#include "hwlibs.h"
#include "i2c.h"
#include "ks0066.h"
#include "pins.h"
#include "pt6311.h"
#include "usart.h"
#include "utils.h"

#include <ctype.h>
#include <string.h>

#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0    ((uint32_t)0x00000007)
#define NVIC_PRIORITYGROUP_1    ((uint32_t)0x00000006)
#define NVIC_PRIORITYGROUP_2    ((uint32_t)0x00000005)
#define NVIC_PRIORITYGROUP_3    ((uint32_t)0x00000004)
#define NVIC_PRIORITYGROUP_4    ((uint32_t)0x00000003)
#endif

void LL_Init(void)
{
#ifdef STM32F1
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
#endif
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
#ifdef STM32F3
    LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_SYSCLK);
    LL_RCC_SetUSARTClockSource(LL_RCC_USART2_CLKSOURCE_SYSCLK);
    LL_RCC_SetUSARTClockSource(LL_RCC_USART3_CLKSOURCE_SYSCLK);
    LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_SYSCLK);
#endif
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

        usartSendString(USART_DBG, utilMkStr("%02x: ", reg));

        if (!hasData) {
            usartSendString(USART_DBG, "\r");
            continue;
        }

        for (uint8_t i = 0; i < num; i++) {
            if (args[i]) {
                usartSendString(USART_DBG, utilMkStr("%04x ", args[i]));
            } else {
                usartSendString(USART_DBG, "---- ");
            }
        }
        usartSendString(USART_DBG, "\n\r");
    }
#else
    usartSendString(USART_DBG, "\rRead pin is disabled\r\n");
#endif
}

typedef struct {
    color_t color;
    char *name;
} ColorNames;

static ColorNames cn[] = {
    {COLOR_WHITE,   "White"},
    {COLOR_RED,     "Red"},
    {COLOR_YELLOW,  "Yellow"},
    {COLOR_LIME,    "Lime"},
    {COLOR_AQUA,    "Aqua"},
    {COLOR_BLUE,    "Blue"},
    {COLOR_MAGENTA, "Magenta"},
};

static uint8_t rxBuf[32];
static uint8_t resBuf[32];

static Glcd *glcd;

void rx_cb(void)
{
    memcpy(resBuf, rxBuf, 32);

    for (size_t i = 0; i < 32; i++) {
        resBuf[i] = toupper(resBuf[i]);
    }
}

int main(void)
{
    // System
    LL_Init();
    SystemClock_Config();
    LL_SYSTICK_EnableIT();

    utilInit();

    pinsInit();

#ifndef _DISP_16BIT
#if IS_GPIO_LO(DISP_DATA)
    i2cInit(I2C_MASTER, 100000);
    i2cInit(I2C_SLAVE, 100000);
#endif
#endif

    ks0066Init();

    ks0066SetXY(0, 0);
    ks0066WriteString("Color:");

    usartInit(USART_DBG, 115200);
    usartSendString(USART_DBG, "\rUsart init done\r\n");
    printDispRegs();

    pt6311Init();

    glcdInit(&glcd);

    // Graphics
    int16_t w = glcd->drv->width;
    int16_t h = glcd->drv->height;

    glcdDrawRect(0, 0, w, h, COLOR_BLACK);

    if (h >= 240) {
        glcdSetFont(&fontterminus32);
    } else if (h >= 176) {
        glcdSetFont(&fontterminus24);
    } else {
        glcdSetFont(&fontterminus16);
    }

    int16_t tw = w / 16;
    int16_t th = h / 4;

    glcdDrawRect(w / 2 + tw * 1, h / 8 * 5, tw / 4 * 6, th, COLOR_RED);
    glcdDrawRect(w / 2 + tw * 3, h / 8 * 5, tw / 4 * 6, th, COLOR_LIME);
    glcdDrawRect(w / 2 + tw * 5, h / 8 * 5, tw / 4 * 6, th, COLOR_BLUE);

    int16_t rx = w / 4 * 3;
    int16_t ry = h / 4 * 1;
    int16_t rr = ry - 8;
    glcdDrawRing(rx, ry, ry - 2, 3, COLOR_WHITE);
    glcdFbSync();

    static char txBuf[8];

    i2cSetRxCb(I2C_SLAVE, rx_cb);

    i2cBegin(I2C_SLAVE, 0x28);
    i2cSlaveTransmitReceive(I2C_SLAVE, rxBuf, sizeof(rxBuf));

    while (1) {
        static uint8_t seg = PT6311_SEG_NUM;
        static uint8_t dig = PT6311_DIG_NUM;

        static size_t it = 0;
        if (++it >= sizeof(cn) / sizeof (cn[0])) {
            it = 0;
        }

        memcpy(txBuf, cn[it].name, 8);

        i2cBegin(I2C_MASTER, 0x28);
        for (size_t i = 0; i <= strlen(txBuf); i++) {
            i2cSend(I2C_MASTER, txBuf[i]);
        }
        i2cTransmit(I2C_MASTER);

        glcdDrawCircle(rx, ry, rr, cn[it].color);

        glcdSetFontColor(cn[it].color);
        glcdSetXY(0, h / 16 * 1);
        glcdWriteString(utilMkStr("Iteration: %d ", it));

        glcdSetXY(0, h / 16 * 5);
        glcdWriteString(utilMkStr("Tx: %-8s", txBuf));

        glcdSetXY(0, h / 16 * 9);
        glcdWriteString(utilMkStr("Rx: %-8s", resBuf));

        glcdSetXY(0, h / 16 * 13);
        glcdWriteString("S: ");
        glcdWriteString(seg == PT6311_SEG_NUM ? "All " : utilMkStr("%2d, ", seg));
        glcdWriteString(" D: ");
        glcdWriteString(dig == PT6311_DIG_NUM ? "All " : utilMkStr("%d ", dig));

        ks0066SetXY(0, 1);
        ks0066WriteString(utilMkStr("%-8s", cn[it].name));

        usartSendString(USART_DBG, utilMkStr("Color: %s\r\n", cn[it].name));
        glcdFbSync();

        for (uint8_t d = 0; d < PT6311_DIG_NUM; d++) {
            if (d == dig || dig == PT6311_DIG_NUM) {
                pt6311SetDigit(d, seg == PT6311_SEG_NUM ? 0x0FFFFF : 1 << seg);
            } else {
                pt6311SetDigit(d, 0);
            }
        }

        pt6311UpdateScreen();

        LL_mDelay(200);

        while (READ(BTN_0) && READ(BTN_1));
        if (!READ(BTN_0)) {
            if (++seg > PT6311_SEG_NUM) {
                seg = 0;
            }
        }
        if (!READ(BTN_1)) {
            if (++dig > PT6311_DIG_NUM) {
                dig = 0;
            }
        }
    }

    return 0;
}
