#ifndef PINS_H
#define PINS_H

#include <stm32f1xx_ll_gpio.h>

void pinsInit(void);

#define CONCAT(x,y)             x ## y

#define OUT(p)                  (LL_GPIO_SetPinMode(CONCAT(p, _Port), CONCAT(p, _Pin), LL_GPIO_MODE_OUTPUT))
#define IN_F(p)                 (LL_GPIO_SetPinMode(CONCAT(p, _Port), CONCAT(p, _Pin), LL_GPIO_MODE_FLOATING))
#define IN_P(p)                 (LL_GPIO_SetPinMode(CONCAT(p, _Port), CONCAT(p, _Pin), LL_GPIO_MODE_INPUT))
#define ALT(p)                  (LL_GPIO_SetPinMode(CONCAT(p, _Port), CONCAT(p, _Pin), LL_GPIO_MODE_ALTERNATE))
#define SET(p)                  (LL_GPIO_SetOutputPin(CONCAT(p, _Port), CONCAT(p, _Pin)))
#define CLR(p)                  (LL_GPIO_ResetOutputPin(CONCAT(p, _Port), CONCAT(p, _Pin)))
#define READ(p)                 (LL_GPIO_IsInputPinSet(CONCAT(p, _Port), CONCAT(p, _Pin)))

#define SPEED(p, s)             (LL_GPIO_SetPinSpeed(CONCAT(p, _Port), CONCAT(p, _Pin), s))
#define OTYPE(p, t)             (LL_GPIO_SetPinOutputType(CONCAT(p, _Port), CONCAT(p, _Pin), t))

#define OUT_INIT(p, t, s)   \
    do {                    \
        OUT(p);             \
        SPEED(p, s);        \
        OTYPE(p, t);        \
    } while (0);

#define IN_U(p)             \
    do {                    \
        IN_P(p);            \
        SET(p);             \
    } while (0);

#define IN_D(p)             \
    do {                    \
        IN_P(p);            \
        CLR(p);             \
    } while (0);

#define ALT_INIT(p, t, s)   \
    do {                    \
        ALT(p);             \
        SPEED(p, s);        \
        OTYPE(p, t);        \
    } while (0);

#define LED1_Port               GPIOA
#define LED1_Pin                LL_GPIO_PIN_2
#define LED3_Port               GPIOA
#define LED3_Pin                LL_GPIO_PIN_3

// TFT LCD pins
#define DISP_DATA_HI_Port       GPIOB
#define DISP_DATA_HI_Pin        (LL_GPIO_PIN_8 | LL_GPIO_PIN_9 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | \
                                 LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15)
#define DISP_DATA_LO_Port       GPIOC
#define DISP_DATA_LO_Pin        (LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_2 | LL_GPIO_PIN_3 | \
                                 LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | LL_GPIO_PIN_6 | LL_GPIO_PIN_7)

// TFT parallel (8-bit) wiring
#define DISP_CS_Port            GPIOC
#define DISP_CS_Pin             LL_GPIO_PIN_9
#define DISP_RD_Port            GPIOC
#define DISP_RD_Pin             LL_GPIO_PIN_11
#define DISP_WR_Port            GPIOC
#define DISP_WR_Pin             LL_GPIO_PIN_10
#define DISP_RS_Port            GPIOC
#define DISP_RS_Pin             LL_GPIO_PIN_8
#define DISP_RST_Port           GPIOB
#define DISP_RST_Pin            LL_GPIO_PIN_5
#define DISP_BCKL_Port          GPIOC
#define DISP_BCKL_Pin           LL_GPIO_PIN_12

// TFT serial (SPI 3 lines) wiring
#define DISP_SPI_SCK_Port       GPIOC
#define DISP_SPI_SCK_Pin        LL_GPIO_PIN_6
#define DISP_SPI_DC             DISP_RS
#define DISP_SPI_SDI            DISP_WR
#define DISP_SPI_SDO            DISP_RD

// USART used for debugging
#define USART_DBG               USART1

#endif // PINS_H
