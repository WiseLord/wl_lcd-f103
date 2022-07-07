#include "canvas.h"

#include "display/glcd.h"
#include "gui/font7seg.h"
#include "vac.h"

#include <stdio.h>

void canvasInit()
{
    glcdInit(GLCD_LANDSCAPE);
    glcdSetBacklight(true);

    // Graphics
    int16_t w = glcdGet()->rect.w;
    int16_t h = glcdGet()->rect.h;

    glcdDrawRect(0, 0, w, h, COLOR_WHITE);
    glcdSetFontColor(COLOR_WHITE);
    glcdSetFontBgColor(COLOR_BLACK);

    glcdSetFont(&fontterminus32);
}

void canvasShowTimer()
{
    glcdSetFont(&fontterminus12);

    glcdSetXY(0, -2);
    int32_t time = vacGetTimer();

    int32_t msec = time % 1000 / 100;

    int32_t min = time / 1000 / 60;
    int32_t sec = time / 1000 % 60;


    char buf[32];
    snprintf(buf, sizeof(buf), "%02ld:%02ld.%01ld", min, sec, msec);
    glcdWriteString(buf);
}
