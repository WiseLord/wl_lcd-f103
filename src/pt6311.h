#ifndef PT6311_H
#define PT6311_H

#include <stdbool.h>
#include <stdint.h>

#define PT6311_DIG_NUM                  16
#define PT6311_SEG_NUM                  20

// 1. Display mode setting commands
#define PT6311_DISP_MODE_08DIG_20SEG    0x00
#define PT6311_DISP_MODE_09DIG_19SEG    0x08
#define PT6311_DISP_MODE_10DIG_18SEG    0x09    // TUNER
#define PT6311_DISP_MODE_11DIG_17SEG    0x0A
#define PT6311_DISP_MODE_12DIG_16SEG    0x0B    // DVD
#define PT6311_DISP_MODE_13DIG_15SEG    0x0C
#define PT6311_DISP_MODE_14DIG_14SEG    0x0D
#define PT6311_DISP_MODE_15DIG_13SEG    0x0E
#define PT6311_DISP_MODE_16DIG_12SEG    0x0F

// 2. Data setting commands
#define PT6311_DATA_WRITE_DISPLAY       0x40
#define PT6311_DATA_WRITE_LED           0x41
#define PT6311_DATA_READ_KEY            0x42
#define PT6311_DATA_READ_SW             0x43

#define PT6311_DATA_ADDR_INC            0x00
#define PT6311_DATA_ADDR_FIXED          0x04

#define PT6311_DATA_NORMAL              0x00
#define PT6311_DATA_TEST                0x08

// 3. Address setting commands
#define PT6311_ADDR_SET                 0xC0
#define PT6311_ADDR_MASK                0x3F

// 4. Display control
#define PT6311_DISP_CTRL                0x80

#define PT6311_DISP_OFF                 0x00
#define PT6311_DISP_ON                  0x08

#define PT6311_DISP_DIM_MASK            0x07

#define PT6311_DATA_WRITE_DISPLAY       0x40

void pt6311Init(void);

void pt6311EnableDisplay(bool value);
void pt6311SetBrightness(uint8_t value);

void pt6311SetDigit(uint8_t pos, uint32_t value);
uint32_t pt6311GetDigit(uint8_t pos);

void pt6311UpdateScreen(void);

#define PT6314_DISP_MODE_TUNER          PT6311_DISP_MODE_10DIG_18SEG

/*
Display from tuner module (VFD-1204TUNED):

Digit 0     1          2          3            4          5            6            7            8          9

              DAB        FM       STEREO                   RDS          TUNED        MEM

 AAAAAAA    AAAAAAA    AAAAAAA    AAAAAAA      AAAAAAA    AAAAAAA      AAAAAAA      AAAAAAA      AAAAAAA    AAAAAAA
F H I J B  F H I J B  F H I J B  F H I J B    F H I J B  F H I J B    F H I J B    F H I J B    F H I J B  F H I J B
F H I J B  F H I J B  F H I J B  F H I J B O  F H I J B  F H I J B O  F H I J B    F H I J B O  F H I J B  F H I J B  KHz
F  H J  B  F  H J  B  F  H J  B  F  H J  B    F  H J  B  F  H J  B    F  H J  B    F  H J  B    F  H J  B  F  H J  B
 G1   G2    G1   G2    G1   G2    G1   G2      G1   G2    G1   G2      G1   G2      G1   G2      G1   G2    G1   G2
E  M K  C  E  M K  C  E  M K  C  E  M K  C    E  M K  C  E  M K  C    E  M K  C    E  M K  C    E  M K  C  E  M K  C
E M L K C  E M L K C  E M L K C  E M L K C O  E M L K C  E M L K C O  E M L K C    E M L K C O  E M L K C  E M L K C  MHz
E M L K C  E M L K C  E M L K C  E M L K C    E M L K C  E M L K C    E M L K C    E M L K C    E M L K C  E M L K C
 DDDDDDD    DDDDDDD    DDDDDDD    DDDDDDD      DDDDDDD    DDDDDDD      DDDDDDD  P   DDDDDDD      DDDDDDD    DDDDDDD

Bit  0      Segment F
Bit  1      Segment A
Bit  2
Bit  3      Segment H
Bit  4      Segment I
Bit  5      Segment J
Bit  6      Segment B
Bit  7      Segment G1
Bit  8      Colon (O), Dot (P), Label: MHz
Bit  9      Segment D
Bit 10      Segment C
Bit 11      Segment K
Bit 12      Segment G2
Bit 13      Segment E
Bit 14      Segment M
Bit 15      Segment L
Bit 16      Labels: DAB, FM, STEREO, RDS, TUNED, MEM, KHz
Bit 17
*/

#define PT6314_DISP_MODE_DVD            PT6311_DISP_MODE_12DIG_1688SEG

/*
Display from DVD module (VFD-1204DVD):

Digit  11        10         9          8          7            6          5            4            3            2          1         0

SLEEP ||  |>     REP        DIR        ALL       INTRO         PBC       dts           DDD         DDPL         PROG        RAN     POPS  JAZZ

               AAAAAAA    AAAAAAA    AAAAAAA    AAAAAAA      AAAAAAA    AAAAAAA      AAAAAAA      AAAAAAA      AAAAAAA    AAAAAAA
STEREO  V CD  F H I J B  F H I J B  F H I J B  F H I J B    F H I J B  F H I J B    F H I J B    F H I J B    F H I J B  F H I J B  ROCK  CLAS
              F H I J B  F H I J B  F H I J B  F H I J B O  F H I J B  F H I J B O  F H I J B    F H I J B O  F H I J B  F H I J B
              F  H J  B  F  H J  B  F  H J  B  F  H J  B    F  H J  B  F  H J  B    F  H J  B    F  H J  B    F  H J  B  F  H J  B
               G1   G2    G1   G2    G1   G2    G1   G2      G1   G2    G1   G2      G1   G2      G1   G2      G1   G2    G1   G2
SUR5.1  MP3   E  M K  C  E  M K  C  E  M K  C  E  M K  C    E  M K  C  E  M K  C    E  M K  C    E  M K  C    E  M K  C  E  M K  C  LOUDNESS
              E M L K C  E M L K C  E M L K C  E M L K C O  E M L K C  E M L K C O  E M L K C    E M L K C O  E M L K C  E M L K C
              E M L K C  E M L K C  E M L K C  E M L K C    E M L K C  E M L K C    E M L K C    E M L K C    E M L K C  E M L K C
DVD5.1  DVD    DDDDDDD    DDDDDDD    DDDDDDD    DDDDDDD      DDDDDDD    DDDDDDD      DDDDDDD  P   DDDDDDD      DDDDDDD    DDDDDDD   BASS   TM

Bit  0      Segment H, CD
Bit  1      Segment F, SLEEP
Bit  2      Segment A, |>
Bit  3      ||, REP, DIR, ALL, INTRO, PBG, dts, DDD, DPL, PROG, RAN
Bit  4      Segment I, STEREO
Bit  5      Segment J, V
Bit  6      Segment M, DVD5.1
Bit  7      Segment E, DVD
Bit  8      Segment G2, POPS
Bit  9      Segment L, CLAS
Bit 10      Segment K, LOUDNESS
Bit 11      Segment C, BASS
Bit 12      Segment B, SUR5.1
Bit 13      Segment G1, MP3
Bit 14      Segment D, TM
Bit 15      Colon (O), Dot (P)
*/

#endif // PT6311_H
