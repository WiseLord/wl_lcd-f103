#include "usart.h"

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_usart.h>

#define TX_Pin LL_GPIO_PIN_9
#define TX_GPIO_Port GPIOA

#define RX_Pin LL_GPIO_PIN_10
#define RX_GPIO_Port GPIOA

static void pinsInitUsart(USART_TypeDef *USARTx)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_TypeDef *gpio = GPIOA;
    uint32_t txPin =  LL_GPIO_PIN_9;
    uint32_t rxPin =  LL_GPIO_PIN_10;

    if (USARTx == USART2) {
        txPin = LL_GPIO_PIN_2;
        rxPin = LL_GPIO_PIN_3;
    } else if (USARTx == USART3) {
        gpio = GPIOB;
        txPin = LL_GPIO_PIN_11;
        rxPin = LL_GPIO_PIN_10;
    }

    GPIO_InitStruct.Pin = txPin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(gpio, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = rxPin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
    LL_GPIO_Init(gpio, &GPIO_InitStruct);
}


void usartInit(USART_TypeDef *USARTx, uint32_t baudRate)
{
    pinsInitUsart(USARTx);

    LL_USART_InitTypeDef USART_InitStruct = {0};

    // Peripheral clock enable and interrupt init
    if (USARTx == USART1) {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
        NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
        NVIC_EnableIRQ(USART1_IRQn);
    } else if (USARTx == USART2) {
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
        NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
        NVIC_EnableIRQ(USART2_IRQn);
    } else if (USARTx == USART3) {
        LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);
        NVIC_SetPriority(USART3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
        NVIC_EnableIRQ(USART3_IRQn);
    }

    USART_InitStruct.BaudRate = baudRate;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;

    LL_USART_Init(USARTx, &USART_InitStruct);

    LL_USART_ConfigAsyncMode(USARTx);

    LL_USART_Enable(USARTx);
}

void usartSendChar(USART_TypeDef *USARTx, char ch)
{
    while (!LL_USART_IsActiveFlag_TXE(USARTx)) {
        // TODO: timeout
    }

    LL_USART_TransmitData8(USARTx, ch);
}

void usartSendString(USART_TypeDef *USARTx, char *str)
{
    while (*str) {
        usartSendChar(USARTx, *str++);
    }
}