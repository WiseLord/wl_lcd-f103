#include "ili9486.h"

#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_utils.h>

#include "../../pins.h"
#include "../dispdrv.h"

const DispDriver dispdrv = {
    .width = 480,
    .height = 320,
    .init = ili9486Init,
    .setWindow = ili9486SetWindow,
    .rotate = ili9486Rotate,
    .shift = ili9486Shift,
};

void ili9486Init(void)
{
    CLR(DISP_CS);

    // Initial Sequence
    dispdrvSelectReg8(0x28);     // Display OFF
    LL_mDelay(100);
    dispdrvSelectReg8(0x10);     // Sleep IN
    LL_mDelay(50);
    dispdrvSelectReg8(0xF2);     // ?????
    dispdrvSendData8(0x18);
    dispdrvSendData8(0xA3);
    dispdrvSendData8(0x12);
    dispdrvSendData8(0x02);
    dispdrvSendData8(0xB2);
    dispdrvSendData8(0x12);
    dispdrvSendData8(0xFF);
    dispdrvSendData8(0x10);
    dispdrvSendData8(0x00);

    dispdrvSelectReg8(0xF1);     // ?????
    dispdrvSendData8(0x36);
    dispdrvSendData8(0xA4);

    dispdrvSelectReg8(0xF8);     // ?????
    dispdrvSendData8(0x21);
    dispdrvSendData8(0x04);

    dispdrvSelectReg8(0xF9);     // ?????
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x08);

    dispdrvSelectReg8(0xC0);     // Power Control 1
    dispdrvSendData8(0x0d);
    dispdrvSendData8(0x0d);

    dispdrvSelectReg8(0xC1);     // Power Control 2
    dispdrvSendData8(0x41);
    dispdrvSendData8(0x00);

    dispdrvSelectReg8(0xC2);     // Power Control 3
    dispdrvSendData8(0x00);

    dispdrvSelectReg8(0xC3);     // Power Control 4
    dispdrvSendData8(0x33);

    dispdrvSelectReg8(0xC4);     // Power Control 5
    dispdrvSendData8(0x33);

    dispdrvSelectReg8(0xC5);     // VCOM Control
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x53);

    dispdrvSelectReg8(0xE0);     // PGAMCTRL (Positive Gamma Control)
    dispdrvSendData8(0x0F);
    dispdrvSendData8(0x1F);
    dispdrvSendData8(0x1C);
    dispdrvSendData8(0x0C);
    dispdrvSendData8(0x0F);
    dispdrvSendData8(0x08);
    dispdrvSendData8(0x48);
    dispdrvSendData8(0x98);
    dispdrvSendData8(0x37);
    dispdrvSendData8(0x0A);
    dispdrvSendData8(0x13);
    dispdrvSendData8(0x04);
    dispdrvSendData8(0x11);
    dispdrvSendData8(0x0D);
    dispdrvSendData8(0x00);

    dispdrvSelectReg8(0xE1);     // NGAMCTRL (Negative Gamma Control)
    dispdrvSendData8(0x0F);
    dispdrvSendData8(0x32);
    dispdrvSendData8(0x2E);
    dispdrvSendData8(0x0B);
    dispdrvSendData8(0x0D);
    dispdrvSendData8(0x05);
    dispdrvSendData8(0x47);
    dispdrvSendData8(0x75);
    dispdrvSendData8(0x37);
    dispdrvSendData8(0x06);
    dispdrvSendData8(0x10);
    dispdrvSendData8(0x03);
    dispdrvSendData8(0x24);
    dispdrvSendData8(0x20);
    dispdrvSendData8(0x00);

    dispdrvSelectReg8(0x20);     // Display Inversion OFF

    dispdrvSelectReg8(0x36);     // Memory Access Control
    dispdrvSendData8(0x18);

    dispdrvSelectReg8(0x3A);     // Interface Pixel Format
    dispdrvSendData8(0x55);

    dispdrvSelectReg8(0xB4);     // Display Inversion Control
    dispdrvSendData8(0x20);

    dispdrvSelectReg8(0x2A);     // Column Addess Set
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0xDF);

    dispdrvSelectReg8(0x2B);     // Page Address Set
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0x3f);

    dispdrvSelectReg8(0x11);     // Sleep OUT

    LL_mDelay(120);
    dispdrvSelectReg8(0x29);     // Display ON
    LL_mDelay(120);
    dispdrvSelectReg8(0x13);     // Normal Display Mode ON
    dispdrvSelectReg8(0x2C);     // Memory Write

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9486Rotate(uint8_t rotate)
{
    CLR(DISP_CS);

    if (rotate & LCD_ROTATE_180) {
        dispdrvSelectReg8(0x36);
        dispdrvSendData8(0xD8);
    } else {
        dispdrvSelectReg8(0x36);
        dispdrvSendData8(0x18);
    }

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9486Shift(int16_t value)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x33);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x01);
    dispdrvSendData8(0xE0);
    dispdrvSendData8(0x00);
    dispdrvSendData8(0x00);

    dispdrvSelectReg8(0x37);
    dispdrvSendData8((value >> 8) & 0xFF);
    dispdrvSendData8(value & 0xFF);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}
void ili9486Sleep(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x28);    // Display OFF
    LL_mDelay(100);
    dispdrvSelectReg8(0x10);

    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void ili9486Wakeup(void)
{
    CLR(DISP_CS);

    dispdrvSelectReg8(0x11);    // Display ON
    LL_mDelay(100);
    dispdrvSelectReg8(0x29);

    SET(DISP_CS);
}

void ili9486SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    dispdrvSelectReg8(0x2A);
    dispdrvSendData8((y >> 8) & 0xFF);
    dispdrvSendData8((y >> 0) & 0xFF);
    dispdrvSendData8((y1 >> 8) & 0xFF);
    dispdrvSendData8((y1 >> 0) & 0xFF);


    dispdrvSelectReg8(0x2B);
    dispdrvSendData8((x >> 8) & 0xFF);
    dispdrvSendData8((x >> 0) & 0xFF);
    dispdrvSendData8((x1 >> 8) & 0xFF);
    dispdrvSendData8((x1 >> 0) & 0xFF);

    dispdrvSelectReg8(0x2C);
}
