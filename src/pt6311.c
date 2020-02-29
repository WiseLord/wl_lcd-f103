#include "pt6311.h"

#include "pins.h"
#include "utils.h"

typedef struct
{
    uint32_t dig[PT6311_DIG_NUM];
    uint8_t ctrl;
} pt6311ctx;

static pt6311ctx ctx;

static void pt6311PinsInit(void)
{
    LL_GPIO_InitTypeDef initDef;

    initDef.Mode = LL_GPIO_MODE_OUTPUT;
    initDef.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    initDef.OutputType = LL_GPIO_OUTPUT_PUSHPULL;

    initDef.Pin = PT6311_STB_Pin;
    LL_GPIO_Init(PT6311_STB_Port, &initDef);
    SET(PT6311_STB);

    initDef.Pin = PT6311_DAT_Pin;
    LL_GPIO_Init(PT6311_DAT_Port, &initDef);
    SET(PT6311_DAT);

    initDef.Pin = PT6311_CLK_Pin;
    LL_GPIO_Init(PT6311_CLK_Port, &initDef);
    SET(PT6311_CLK);
}

static void pt6311SendByte(uint8_t data)
{
    for (int i = 0; i < 8; i++) {
        if (data & 0x01) {
            SET(PT6311_DAT);
        } else {
            CLR(PT6311_DAT);
        }
        CLR(PT6311_CLK);
        utilnDelay(400);
        SET(PT6311_CLK);
        utilnDelay(400);
        data >>= 1;
    }
}

static void pt6311SendCmd(uint8_t cmd)
{
    utilnDelay(400);
    CLR(PT6311_STB);
    pt6311SendByte(cmd);
    utilnDelay(600);
    SET(PT6311_STB);
}

void pt6311Init(void)
{
    pt6311PinsInit();

    pt6311UpdateScreen();

    pt6311SendCmd(PT6314_DISP_MODE_TUNER);
    pt6311SetBrightness(7);
    pt6311EnableDisplay(true);
}

void pt6311EnableDisplay(bool value)
{
    value ? (ctx.ctrl |= PT6311_DISP_ON) : (ctx.ctrl &= ~PT6311_DISP_ON);

    pt6311SendCmd(PT6311_DISP_CTRL | ctx.ctrl);
}

void pt6311SetBrightness(uint8_t value)
{
    if (value > PT6311_DISP_DIM_MASK) {
        value = PT6311_DISP_DIM_MASK;
    }

    ctx.ctrl &= ~PT6311_DISP_DIM_MASK;
    ctx.ctrl |= value;

    pt6311SendCmd(PT6311_DISP_CTRL | ctx.ctrl);
}

void pt6311SetDigit(uint8_t pos, uint32_t value)
{
    if (pos >= PT6311_DIG_NUM) {
        return;
    }

    ctx.dig[pos] = value;
}

uint32_t pt6311GetDigit(uint8_t pos)
{
    if (pos >= PT6311_DIG_NUM) {
        return 0;
    }

    return ctx.dig[pos];
}

void pt6311UpdateScreen(void)
{
    pt6311SendCmd(PT6311_DATA_WRITE_DISPLAY | PT6311_DATA_ADDR_INC);

    utilnDelay(400);
    CLR(PT6311_STB);

    pt6311SendByte(PT6311_ADDR_SET | 0x00);

    for (uint8_t i = 0; i < PT6311_DIG_NUM; i++) {
        uint32_t data = ctx.dig[i];
        pt6311SendByte(data & 0xFF);
        data >>= 8;
        pt6311SendByte(data & 0xFF);
        data >>= 8;
        pt6311SendByte(data & 0x0F);
    }

    utilnDelay(600);
    SET(PT6311_STB);
}
