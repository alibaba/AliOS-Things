/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_UART_STM32L4_H
#define __HAL_UART_STM32L4_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32l4xx_hal.h"  
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "aos/kernel.h"

#define PORT_UART1         1
#define PORT_UART2         2
#define PORT_UART3         3
#define PORT_UART4         4
#define PORT_LPUART1       0x81
#define UART_IRQ_PRIORITY  2

/***************************** lpuart1 configure *******************************/
//#define LPUART1                        LPUART1

#define LPUART1_BANDRATE               115200
#define LPUART1_WORD_LENGTH            UART_WORDLENGTH_8B
#define LPUART1_STOP_BITS              UART_STOPBITS_1
#define LPUART1_PARITY                 UART_PARITY_NONE
#define LPUART1_MODE                   UART_MODE_TX_RX
#define LPUART1_HW_FLOW_CTL            UART_HWCONTROL_NONE
#define LPUART1_OVER_SAMPLING          
#define LPUART1_ONE_BIT_SAMPLING       UART_ONE_BIT_SAMPLE_DISABLE
#define LPUART1_ADV_FEATURE_INIT       UART_ADVFEATURE_NO_INIT

#define LPUART1_CLK_ENABLE()           __HAL_RCC_LPUART1_CLK_ENABLE()
#define LPUART1_CLK_DISABLE()          __HAL_RCC_LPUART1_CLK_DISABLE()
#define LPUART1_TX_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()
#define LPUART1_TX_GPIO_CLK_DISABLE()  __HAL_RCC_GPIOB_CLK_DISABLE()
#define LPUART1_RX_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()
#define LPUART1_RX_GPIO_CLK_DISABLE()  __HAL_RCC_GPIOB_CLK_DISABLE()

#define LPUART1_TX_PIN                 GPIO_PIN_11
#define LPUART1_TX_MODE                GPIO_MODE_AF_PP
#define LPUART1_TX_SPEED               GPIO_SPEED_FREQ_VERY_HIGH
#define LPUART1_TX_PULL                GPIO_PULLUP
#define LPUART1_TX_ALTERNATE           GPIO_AF8_LPUART1
#define LPUART1_TX_GPIO_PORT           GPIOB

#define LPUART1_RX_PIN                 GPIO_PIN_10
#define LPUART1_RX_MODE                GPIO_MODE_AF_PP
#define LPUART1_RX_ALTERNATE           GPIO_AF8_LPUART1
#define LPUART1_RX_GPIO_PORT           GPIOB

//#define LPUART1_IRQn                   LPUART1_IRQn


/***************************** uart2 configure *******************************/
#define UART2                          USART2

#define UART2_BANDRATE                 115200
#define UART2_WORD_LENGTH              UART_WORDLENGTH_8B
#define UART2_STOP_BITS                UART_STOPBITS_1
#define UART2_PARITY                   UART_PARITY_NONE
#define UART2_MODE                     UART_MODE_TX_RX
#define UART2_HW_FLOW_CTL              UART_HWCONTROL_NONE
#define UART2_OVER_SAMPLING            UART_OVERSAMPLING_16
#define UART2_ONE_BIT_SAMPLING         UART_ONE_BIT_SAMPLE_DISABLE
#define UART2_ADV_FEATURE_INIT         UART_ADVFEATURE_NO_INIT

#define UART2_CLK_ENABLE()             __HAL_RCC_USART2_CLK_ENABLE()
#define UART2_CLK_DISABLE()            __HAL_RCC_USART2_CLK_DISABLE()
#define UART2_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define UART2_TX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOA_CLK_DISABLE()
#define UART2_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define UART2_RX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOA_CLK_DISABLE()

#define UART2_TX_PIN                   GPIO_PIN_2
#define UART2_TX_MODE                  GPIO_MODE_AF_PP
#define UART2_TX_SPEED                 GPIO_SPEED_FREQ_VERY_HIGH
#define UART2_TX_PULL                  GPIO_NOPULL
#define UART2_TX_ALTERNATE             GPIO_AF7_USART2
#define UART2_TX_GPIO_PORT             GPIOA

#define UART2_RX_PIN                   GPIO_PIN_3
#define UART2_RX_MODE                  GPIO_MODE_AF_PP
#define UART2_RX_ALTERNATE             GPIO_AF7_USART2
#define UART2_RX_GPIO_PORT             GPIOA

#define UART2_IRQn                     USART2_IRQn

/***************************** uart3 configure *******************************/
#define UART3                          USART3

#define UART3_BANDRATE                 115200
#define UART3_WORD_LENGTH              UART_WORDLENGTH_8B
#define UART3_STOP_BITS                UART_STOPBITS_1
#define UART3_PARITY                   UART_PARITY_NONE
#define UART3_MODE                     UART_MODE_TX_RX
#define UART3_HW_FLOW_CTL              UART_HWCONTROL_NONE
#define UART3_OVER_SAMPLING            UART_OVERSAMPLING_16
#define UART3_ONE_BIT_SAMPLING         UART_ONE_BIT_SAMPLE_DISABLE
#define UART3_ADV_FEATURE_INIT         UART_ADVFEATURE_NO_INIT

#define UART3_CLK_ENABLE()             __HAL_RCC_USART3_CLK_ENABLE()
#define UART3_CLK_DISABLE()            __HAL_RCC_USART3_CLK_DISABLE()
#define UART3_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
#define UART3_TX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOC_CLK_DISABLE()
#define UART3_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
#define UART3_RX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOC_CLK_DISABLE()

#define UART3_TX_PIN                   GPIO_PIN_4
#define UART3_TX_MODE                  GPIO_MODE_AF_PP
#define UART3_TX_SPEED                 GPIO_SPEED_FREQ_VERY_HIGH
#define UART3_TX_PULL                  GPIO_NOPULL
#define UART3_TX_ALTERNATE             GPIO_AF7_USART3
#define UART3_TX_GPIO_PORT             GPIOC

#define UART3_RX_PIN                   GPIO_PIN_5
#define UART3_RX_MODE                  GPIO_MODE_AF_PP
#define UART3_RX_ALTERNATE             GPIO_AF7_USART3
#define UART3_RX_GPIO_PORT             GPIOC

#define UART3_IRQn                     USART3_IRQn

#define MAX_BUF_UART_BYTES  4096

#ifdef __cplusplus
}
#endif

#endif /* __HAL_UART_STM32L4_H */
