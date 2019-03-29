#include "ssd1286a.h"

#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_spi.h>
#include <stm32f1xx_ll_utils.h>

#include "../../pins.h"
#include "../dispdrv.h"

static DispDriver drv = {
    .width = 176,
    .height = 132,
    .setWindow = ssd1286aSetWindow,
};

static void ssd1286aInitSeq(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x01);

    dispdrvSelectReg8(0x10);
    dispdrvSendData8(0x1f);
    dispdrvSendData8(0x92);

    dispdrvSelectReg8(0x11);
    dispdrvSendData8(0x61);
    dispdrvSendData8(0x1c);

    dispdrvSelectReg8(0x28);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x06);

    dispdrvSelectReg8(0x02);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);

    dispdrvSelectReg8(0x12);
    dispdrvSendData8(0x04);
    dispdrvSendData8(0x0f);

    dispdrvSelectReg8(0x01);
    dispdrvSendData8(0x30);
    dispdrvSendData8(0xb0);

    dispdrvSelectReg8(0x03);
    dispdrvSendData8(0x78);
    dispdrvSendData8(0x30);

    dispdrvSelectReg8(0x07);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x33);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ssd1286aInit(DispDriver **driver)
{
    *driver = &drv;
    ssd1286aInitSeq();
}

void ssd1286aSleep(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x07);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);

    dispdrvSelectReg8(0x10);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x01);

    dispdrvSelectReg8(0x11);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);

    dispdrvSelectReg8(0x12);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ssd1286aWakeup(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x10);
    dispdrvSendData8(0x1f);
    dispdrvSendData8(0x92);

    LL_mDelay(20);

    dispdrvSelectReg8(0x11);
    dispdrvSendData8(0x61);
    dispdrvSendData8(0x1c);

    LL_mDelay(20);

    dispdrvSelectReg8(0x12);
    dispdrvSendData8(0x04);
    dispdrvSendData8(0x0f);

    LL_mDelay(20);

    dispdrvSelectReg8(0x07);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x33);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ssd1286aSetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    uint8_t x0 = (uint8_t)x;
    uint8_t y0 = (uint8_t)y;
    uint8_t x1 = (uint8_t)(x + w - 1);
    uint8_t y1 = (uint8_t)(y + h - 1);

    dispdrvSelectReg8(0x44);
    dispdrvSendData8(y1);
    dispdrvSendData8(y0);

    dispdrvSelectReg8(0x47);
    dispdrvSendData8(x1);
    dispdrvSendData8(x0);

    dispdrvSelectReg8(0x21);
    dispdrvSendData8(x0);
    dispdrvSendData8(y0);

    dispdrvSelectReg8(0x22);
}
