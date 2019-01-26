#include "ili9225.h"

#include <stm32f1xx_ll_utils.h>
#include "../../pins.h"
#include "../dispdrv.h"

static DispDriver drv = {
    .width = 220,
    .height = 176,
    .drawPixel = ili9225DrawPixel,
    .drawRectangle = ili9225DrawRectangle,
    .drawImage = ili9225DrawImage,
};

__attribute__((always_inline))
static inline void ili9225SelectReg(uint16_t reg)
{
    CLR(DISP_RS);
    dispdrvSendData16(reg);
    SET(DISP_RS);
}

static void ili9225WriteReg(uint16_t reg, uint16_t data)
{
    ili9225SelectReg(reg);
    dispdrvSendData16(data);
}

__attribute__((always_inline))
static inline void ili9225SetWindow(int16_t x, int16_t y, int16_t w, int16_t h)
{
    int16_t x1 = x + w - 1;
    int16_t y1 = drv.height - y - 1;    // TODO: rework it

    ili9225WriteReg(0x0037, (uint16_t)(y1 - h + 1));
    ili9225WriteReg(0x0036, (uint16_t)y1);
    ili9225WriteReg(0x0039, (uint16_t)x);
    ili9225WriteReg(0x0038, (uint16_t)x1);

    // Set cursor
    ili9225WriteReg(0x0020, (uint16_t)y1);
    ili9225WriteReg(0x0021, (uint16_t)x);

    // Select RAM mode
    ili9225SelectReg(0x0022);
}

static inline void ili9225InitSeq(void)
{
    CLR(DISP_CS);

    // Start Initial Sequence
    ili9225WriteReg(0x0001, 0x011C);    // set SS and NL bit
    ili9225WriteReg(0x0002, 0x0100);    // set 1 line inversion
    ili9225WriteReg(0x0003, 0x1020);    // set GRAM write direction and BGR=1.
    ili9225WriteReg(0x0008, 0x0808);    // set BP and FP
    ili9225WriteReg(0x000C, 0x0000);    // RGB interface setting   R0Ch=0x0110 for RGB 18Bit and R0Ch=0111for RGB16Bit
    ili9225WriteReg(0x000F, 0x0801);    // Set frame rate

    // Power On sequence

    LL_mDelay(50); // Delay 50ms

    ili9225WriteReg(0x0010, 0x0A00);    // Set SAP,DSTB,STB
    ili9225WriteReg(0x0011, 0x1038);    // Set APON,PON,AON,VCI1EN,VC

    LL_mDelay(50); // Delay 50ms

    ili9225WriteReg(0x0012, 0x6121);    // Internal reference voltage= Vci;
    ili9225WriteReg(0x0013, 0x0062);    // Set GVDD
    ili9225WriteReg(0x0014, 0x5b60);    // Set VCOMH/VCOML voltage

    // Set GRAM area
    ili9225WriteReg(0x0030, 0x0000);
    ili9225WriteReg(0x0031, 0x00DB);
    ili9225WriteReg(0x0032, 0x0000);
    ili9225WriteReg(0x0033, 0x0000);
    ili9225WriteReg(0x0034, 0x00DB);
    ili9225WriteReg(0x0035, 0x0000);
    ili9225WriteReg(0x0036, 0x00AF);
    ili9225WriteReg(0x0037, 0x0000);
    ili9225WriteReg(0x0038, 0x00DB);
    ili9225WriteReg(0x0039, 0x0000);
    ili9225WriteReg(0x0020, 0x0000);    // Set GRAM Address
    ili9225WriteReg(0x0021, 0x0000);    // Set GRAM Address

    // Adjust the Gamma  Curve
    ili9225WriteReg(0x0050, 0x0000);
    ili9225WriteReg(0x0051, 0x000B);
    ili9225WriteReg(0x0052, 0x0a01);
    ili9225WriteReg(0x0053, 0x010c);
    ili9225WriteReg(0x0054, 0x010a);
    ili9225WriteReg(0x0055, 0x0B00);
    ili9225WriteReg(0x0056, 0x0000);
    ili9225WriteReg(0x0057, 0x0c01);
    ili9225WriteReg(0x0058, 0x0E00);
    ili9225WriteReg(0x0059, 0x000E);

    LL_mDelay(50); // Delay 50ms

    ili9225WriteReg(0x0007, 0x1017);  // 65K color and display ON

    SET(DISP_CS);
}

void ili9225Init(DispDriver **driver)
{
    *driver = &drv;
    ili9225InitSeq();
}

void ili9225Sleep(void)
{
    CLR(DISP_CS);

    ili9225WriteReg(0x0007, 0x0000);    // Display OFF
    LL_mDelay(50);
    ili9225WriteReg(0x0010, 0x0A01);    // SAP, BT[3:0], AP, DSTB, SLP, STB

    SET(DISP_CS);
}

void ili9225Wakeup(void)
{
    CLR(DISP_CS);

    ili9225WriteReg(0x0010, 0x0A00);    // SAP, BT[3:0], AP, DSTB, SLP, STB
    LL_mDelay(50);
    ili9225WriteReg(0x0007, 0x1017);    // 65K color and display ON

    SET(DISP_CS);
}

void ili9225DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    CLR(DISP_CS);

    ili9225SetWindow(x, y, 1, 1);
    dispdrvSendData16(color);

    SET(DISP_CS);
}

void ili9225DrawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    CLR(DISP_CS);

    ili9225SetWindow(x, y, w, h);
    dispdrvSendFill(w * h, color);

    SET(DISP_CS);
}

void ili9225DrawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor)
{
    int16_t w = img->width;
    int16_t h = img->height;

    CLR(DISP_CS);

    ili9225SetWindow(x, y, w, h);
    dispdrvSendImage(img, color, bgColor);

    SET(DISP_CS);
}
