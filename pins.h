#ifndef PINS_H
#define PINS_H

#include "hwlibs.h"

void pinsInit(void);

#ifdef _STM32F1
#define IS_GPIO_HI(x)           ((x ## _Pin) & 0x00FF0000U)
#define IS_GPIO_LO(x)           ((x ## _Pin) & 0x0000FF00U)
#endif
#ifdef _STM32F3
#define IS_GPIO_HI(x)           ((x ## _Pin) & 0x0000FF00U)
#define IS_GPIO_LO(x)           ((x ## _Pin) & 0x000000FFU)
#endif

#define CONCAT(x,y)             x ## y

#define SET(p)                  (LL_GPIO_SetOutputPin(CONCAT(p, _Port), CONCAT(p, _Pin)))
#define CLR(p)                  (LL_GPIO_ResetOutputPin(CONCAT(p, _Port), CONCAT(p, _Pin)))
#ifdef _STM32F1
#define READ(p)                 (LL_GPIO_ReadInputPort(CONCAT(p, _Port)) & (CONCAT(p, _Pin) >> GPIO_PIN_MASK_POS) & 0x0000FFFFU)
#else
#define READ(p)                 (LL_GPIO_ReadInputPort(CONCAT(p, _Port)) & (CONCAT(p, _Pin)) & 0x0000FFFFU)
#endif

#define READ_BYTE(p)            (IS_GPIO_LO(p) ? (READ(p) & 0x00FF) : (READ(p) & 0xFF00) >> 8)

#define WRITE_BYTE(p, data)     (CONCAT(p, _Port)->BSRR = (IS_GPIO_LO(p) ? (0x00FF0000U | (uint32_t)data) : (0xFF000000U | (uint32_t)(data << 8))))

#define LED1_Port               GPIOA
#define LED1_Pin                LL_GPIO_PIN_2
#define LED3_Port               GPIOA
#define LED3_Pin                LL_GPIO_PIN_3

// TFT LCD pins
#define DISP_DATA_Port          GPIOB
#define DISP_DATA_Pin           (LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3 | \
                                 LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | LL_GPIO_PIN_6 | LL_GPIO_PIN_7)

// TFT parallel (8-bit) wiring
#define DISP_CS_Port            GPIOB
#define DISP_CS_Pin             LL_GPIO_PIN_12
#define DISP_RD_Port            GPIOB
#define DISP_RD_Pin             LL_GPIO_PIN_10
#define DISP_WR_Port            GPIOB
#define DISP_WR_Pin             LL_GPIO_PIN_15
#define DISP_RS_Port            GPIOB
#define DISP_RS_Pin             LL_GPIO_PIN_14  // SPI DC
#define DISP_RST_Port           GPIOB
#define DISP_RST_Pin            LL_GPIO_PIN_11
#define DISP_BCKL_Port          GPIOC
#define DISP_BCKL_Pin           LL_GPIO_PIN_13

#endif // PINS_H
