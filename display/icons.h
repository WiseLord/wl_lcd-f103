#ifndef ICONS_H
#define ICONS_H

#include <stdint.h>

enum {
    ICON24_VOLUME = 0,
    ICON24_BASS,
    ICON24_MIDDLE,
    ICON24_TREBLE,
    ICON24_PREAMP,
    ICON24_FRONTREAR,
    ICON24_BALANCE,
    ICON24_CENTER,
    ICON24_SUBWOOFER,

    ICON24_TUNER,
    ICON24_PC,
    ICON24_TV,
    ICON24_BLUETOOTH,
    ICON24_DVD,

    ICON24_USB,
    ICON24_CH51,
    ICON24_GUITAR,
    ICON24_TURNTABLES,
    ICON24_MICROPHONE,
    ICON24_PROJECTOR,
    ICON24_CASSETTE,
    ICON24_MIXER,
    ICON24_SATELLITE,

    ICON24_BRIGHTNESS,
    ICON24_THRESHOLD,

    ICON24_END
};

enum {
    ICON32_MUTE_ON = 0,
    ICON32_MUTE_OFF,
    ICON32_LOUDNESS_ON,
    ICON32_LOUDNESS_OFF,
    ICON32_SURROUND_ON,
    ICON32_SURROUND_OFF,
    ICON32_EFFECT_3D_ON,
    ICON32_EFFECT_3D_OFF,
    ICON32_TONE_BYPASS_ON,
    ICON32_TONE_BYPASS_OFF,

    ICON32_END
};

extern const uint8_t icons_24[];
extern const uint8_t icons_32[];

#endif //ICONS_H
