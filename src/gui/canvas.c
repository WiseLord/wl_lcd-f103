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
    glcdSetFontColor(COLOR_BLACK);
    glcdSetFontBgColor(COLOR_WHITE);

    glcdSetFont(&fontterminus32);
    font7segLoad(font_7seg_10);
}

void canvasShowTimer()
{
    glcdSetXY(20, 20);
    int32_t time = vacGetTimer() / 1000;

    int32_t min = time / 60;
    int32_t sec = time % 60;

    char buf[32];
    snprintf(buf, sizeof(buf), "%02ld:%02ld", min, sec);
//        glcdWriteString(buf);
    font7segWriteString(buf);
}
