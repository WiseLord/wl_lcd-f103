#ifndef HWLIBS_H
#define HWLIBS_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(STM32F103xB)

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_cortex.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_i2c.h>
#include <stm32f1xx_ll_rcc.h>
#include <stm32f1xx_ll_spi.h>
#include <stm32f1xx_ll_system.h>
#include <stm32f1xx_ll_tim.h>
#include <stm32f1xx_ll_usart.h>
#include <stm32f1xx_ll_utils.h>

#elif defined (STM32F303xC)

#include <stm32f3xx_ll_bus.h>
#include <stm32f3xx_ll_cortex.h>
#include <stm32f3xx_ll_gpio.h>
#include <stm32f3xx_ll_i2c.h>
#include <stm32f3xx_ll_rcc.h>
#include <stm32f3xx_ll_spi.h>
#include <stm32f3xx_ll_system.h>
#include <stm32f3xx_ll_tim.h>
#include <stm32f3xx_ll_usart.h>
#include <stm32f3xx_ll_utils.h>

#endif

// Pins helper definitions
#define CONCAT(x,y)             x ## y

#define SET(p)                  (LL_GPIO_SetOutputPin(CONCAT(p, _Port), CONCAT(p, _Pin)))
#define CLR(p)                  (LL_GPIO_ResetOutputPin(CONCAT(p, _Port), CONCAT(p, _Pin)))
#define TOG(p)                  (LL_GPIO_TogglePin(CONCAT(p, _Port), CONCAT(p, _Pin)))
#ifdef STM32F1
#define READ(p)                 (LL_GPIO_ReadInputPort(CONCAT(p, _Port)) & (CONCAT(p, _Pin) >> GPIO_PIN_MASK_POS) & 0x0000FFFFU)
#else
#define READ(p)                 (LL_GPIO_ReadInputPort(CONCAT(p, _Port)) & (CONCAT(p, _Pin)) & 0x0000FFFFU)
#endif

#define OUT(p)                  (LL_GPIO_SetPinMode(CONCAT(p, _Port), CONCAT(p, _Pin), LL_GPIO_MODE_OUTPUT))
#define IN(p)                   (LL_GPIO_SetPinMode(CONCAT(p, _Port), CONCAT(p, _Pin), LL_GPIO_MODE_INPUT))

// Project-specific definitions
#define USART_DBG               USART1

#define LL_I2C1_SCL_PIN         LL_GPIO_PIN_8
#define LL_I2C1_SDA_PIN         LL_GPIO_PIN_9

#define I2C_MASTER              I2C1
#define I2C_SLAVE               I2C2

#define I2C_KS0066              I2C1

#define TIM_DISPSCAN            TIM2
#define TIM_DISPSCAN_HANDLER    TIM2_IRQHandler

#ifdef __cplusplus
}
#endif

#endif // HWLIBS_H
