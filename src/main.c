#include "display/glcd.h"
#include "display/dispconf.h"
#include "gui/font7seg.h"
#include "hwlibs.h"
#include "i2c.h"
#include "ks0066.h"
#include "usart.h"
#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static void NVIC_Init(void)
{
    // System interrupt init
    NVIC_SetPriority(MemoryManagement_IRQn, 0);
    NVIC_SetPriority(BusFault_IRQn, 0);
    NVIC_SetPriority(UsageFault_IRQn, 0);
    NVIC_SetPriority(SVCall_IRQn, 0);
    NVIC_SetPriority(DebugMonitor_IRQn, 0);
    NVIC_SetPriority(PendSV_IRQn, 0);
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
    NVIC_SetPriority(SysTick_IRQn, 0);
}

static void sysInit(void)
{
    // System
    NVIC_Init();
    SystemClock_Config();

    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    // Enable clock for all GPIO peripherials
#ifdef STM32F1
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
#endif
#ifdef STM32F3
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
#endif

#ifdef STM32F1
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
#endif

#ifdef STM32F3
    LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_SYSCLK);
    LL_RCC_SetUSARTClockSource(LL_RCC_USART2_CLKSOURCE_SYSCLK);
    LL_RCC_SetUSARTClockSource(LL_RCC_USART3_CLKSOURCE_SYSCLK);
    LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_HSI);
#endif

#ifdef STM32F1
    LL_GPIO_AF_Remap_SWJ_NOJTAG();
#endif
}

void SysTick_Handler(void)
{
}

#ifdef _DISP_READ_ENABLED
void dispdrvReset(void);
void dispdrvReadReg(uint16_t reg, uint16_t *args, uint8_t nArgs);
#endif

void printDispRegs(void)
{
#ifdef _DISP_READ_ENABLED

    const uint8_t num = 10;

    uint16_t args[num];
    char buf[8];

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

        snprintf(buf, sizeof(buf), "%02x: ", reg);
        usartSendString(USART_DBG, buf);

        if (!hasData) {
            usartSendString(USART_DBG, "\r");
            continue;
        }

        for (uint8_t i = 0; i < num; i++) {
            if (args[i]) {
                snprintf(buf, sizeof(buf), "%04x ", args[i]);
                usartSendString(USART_DBG, buf);
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

void rx_cb(int16_t bytes)
{
    if (bytes > 32) {
        bytes = 32;
    }

    memcpy(resBuf, rxBuf, bytes);

    for (int16_t i = 0; i < bytes; i++) {
        resBuf[i] = toupper(resBuf[i]);
    }
}

int main(void)
{
    sysInit();

#ifndef _DISP_16BIT
#if DISP_DATA_Pin & 0x00FF
    i2cInit(I2C_MASTER, 100000, 0);
    i2cInit(I2C_SLAVE, 100000, 0x28);
#endif
#endif

#ifndef _DISP_16BIT
#if DISP_DATA_Pin & 0x00FF
    ks0066Init();

    ks0066SetXY(0, 0);
    ks0066WriteString("Color:");
#endif
#endif

    usartInit(USART_DBG, 115200);
    usartSendString(USART_DBG, "\rUsart init done\r\n");
//    printDispRegs();

    glcdInit(GLCD_LANDSCAPE);
    glcdSetBacklight(true);

    // Graphics
    int16_t w = glcdGet()->rect.w;
    int16_t h = glcdGet()->rect.h;

    glcdDrawRect(0, 0, w, h, COLOR_BLACK);

    if (h >= 240) {
        glcdSetFont(&fontterminus32);
    } else if (h >= 176) {
        glcdSetFont(&fontterminus20);
    } else {
        glcdSetFont(&fontterminus12);
    }
    font7segLoad(font_7seg_3);

    int16_t tw = w / 16;
    int16_t th = h / 4;

    glcdDrawRect(w / 2 + tw * 1, h / 8 * 5, tw / 4 * 6, th, COLOR_RED);
    glcdDrawRect(w / 2 + tw * 3, h / 8 * 5, tw / 4 * 6, th, COLOR_LIME);
    glcdDrawRect(w / 2 + tw * 5, h / 8 * 5, tw / 4 * 6, th, COLOR_BLUE);

    int16_t rx = w / 4 * 3;
    int16_t ry = h / 4 * 1;
    int16_t rr = ry - 8;
    glcdDrawRing(rx, ry, ry - 2, 3, COLOR_WHITE);
    glcdSync();

    static char txBuf[8];

#ifndef _DISP_16BIT
#if DISP_DATA_Pin & 0x00FF
    i2cSetRxCb(I2C_SLAVE, rx_cb);
    i2cBegin(I2C_SLAVE, 0x28);
    i2cSlaveTransmitReceive(I2C_SLAVE, rxBuf, sizeof(rxBuf));
#endif
#endif

    char buf[32];

    while (1) {
        static size_t it = 0;
        if (++it >= sizeof(cn) / sizeof (cn[0])) {
            it = 0;
        }

        static uint16_t cnt = 0;
        cnt++;

        memcpy(txBuf, cn[it].name, 8);

#ifndef _DISP_16BIT
#if DISP_DATA_Pin & 0x00FF
        i2cBegin(I2C_MASTER, 0x28);
        for (size_t i = 0; i <= strlen(txBuf); i++) {
            i2cSend(I2C_MASTER, txBuf[i]);
        }
        i2cTransmit(I2C_MASTER);
#endif
#endif

        glcdDrawCircle(rx, ry, rr, cn[it].color);

        glcdSetFontColor(COLOR_WHITE);
        glcdSetFontBgColor(COLOR_NERO);

        glcdSetXY(2, h / 16 * 1);
        snprintf(buf, sizeof(buf), "%2d.%03d", cnt / 1000, cnt % 1000);
        font7segWriteString(buf);

        glcdSetFontColor(cn[it].color);
        glcdSetFontBgColor(COLOR_BLACK);

        glcdSetXY(2, h / 16 * 5);
        snprintf(buf, sizeof(buf), "Iteration: %d ", it);
        glcdWriteString(buf);

        glcdSetXY(2, h / 16 * 9);
        snprintf(buf, sizeof(buf), "Tx: %-8s", txBuf);
        glcdWriteString(buf);

        glcdSetXY(2, h / 16 * 13);
        snprintf(buf, sizeof(buf), "Rx: %-8s", resBuf);
        glcdWriteString(buf);

#ifndef _DISP_16BIT
#if DISP_DATA_Pin & 0x00FF
        ks0066SetXY(0, 1);
        snprintf(buf, sizeof(buf), "%-8s", cn[it].name);
        ks0066WriteString(buf);
#endif
#endif

        snprintf(buf, sizeof(buf), "Color: %s\r\n", cn[it].name);
        usartSendString(USART_DBG, buf);
        glcdSync();

        LL_mDelay(500);
    }

    return 0;
}
