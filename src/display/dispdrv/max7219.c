#include "../dispdrv.h"

#define DISP_WIDTH      8
#define DISP_HEIGHT     8

// Registers
#define MAX7219_NO_OP           0x00
#define MAX7219_DIGIT_0         0x01
#define MAX7219_DIGIT_1         0x02
#define MAX7219_DIGIT_2         0x03
#define MAX7219_DIGIT_3         0x04
#define MAX7219_DIGIT_4         0x05
#define MAX7219_DIGIT_5         0x06
#define MAX7219_DIGIT_6         0x07
#define MAX7219_DIGIT_7         0x08
#define MAX7219_DEC_MODE        0x09
#define MAX7219_INTENSITY       0x0A
#define MAX7219_SCAN_LIMIT      0x0B
#define MAX7219_SHUTDOWN        0x0C
#define MAX7219_DISP_TEST       0x0F

#define MATRIX_CNT              4

void max7219SendCmd(uint8_t reg, uint8_t data);
void max7219SendDataBuf(uint8_t *buf);
void max7219Init(void);

static uint8_t fb[MATRIX_CNT * 8];

static void max7219SendByte(uint8_t data)
{
    DISP_SPI_SEND_BYTE(data);
}

void max7219SendCmd(uint8_t reg, uint8_t data)
{
    uint8_t j;

    CLR(DISP_CS);
    for (j = 0; j < MATRIX_CNT; j++) {
        max7219SendByte(reg);
        max7219SendByte(data);
    }
    DISP_WAIT_BUSY();
    SET(DISP_CS);
}

void max7219Init(void)
{
    OUT_OD(DISP_CS);

    max7219SendCmd(MAX7219_SHUTDOWN, 1);    // Power on
    max7219SendCmd(MAX7219_DISP_TEST, 0);   // Test mode off
    max7219SendCmd(MAX7219_DEC_MODE, 0);    // Use led matrix
    max7219SendCmd(MAX7219_SCAN_LIMIT, 7);  // Scan all 8 digits (cols)
}

void max7219FbSync(void)
{
    int8_t i, j;

    for (i = 0; i < 8; i++) {
        CLR(DISP_CS);
        for (j = MATRIX_CNT - 1; j >= 0; j--) {
            max7219SendByte(MAX7219_DIGIT_0 + i);
            max7219SendByte(fb[8 * j + i]);
        }
        DISP_WAIT_BUSY();
        SET(DISP_CS);
    }
}

void max7219SetBrightness(uint8_t value)
{
    // value = 0..15
    max7219SendCmd(MAX7219_INTENSITY, value);
}

void max7219Sleep(bool value)
{
    max7219SendCmd(MAX7219_SHUTDOWN, value);
}

void max7219FbSetPixel(int16_t x, int16_t y, color_t color)
{
    if (color) {
        fb[x] |= (1 << y);
    } else {
        fb[x] &= ~(1 << y);
    }
}

const DispDriver dispdrv = {
    .width = DISP_WIDTH * MATRIX_CNT,
    .height = DISP_HEIGHT,
    .init = max7219Init,
    .sleep = max7219Sleep,
    .setBrightness = max7219SetBrightness,

    .fbSync = max7219FbSync,
    .fbSetPixel = max7219FbSetPixel,
};
