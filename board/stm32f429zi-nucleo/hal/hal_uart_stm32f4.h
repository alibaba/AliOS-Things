/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __HAL_UART_STM32F4_H
#define __HAL_UART_STM32F4_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "aos/kernel.h"
#include "hal/hal.h"

#define PORT_UART1 1           /* USART1 */
#define PORT_UART2 2           /* USART2 */
#define PORT_UART3 0           /* the port number of stand uart must be 0 */
#define PORT_UART4 3           /* UART4 */
#define PORT_UART5 4           /* UART5 */
#define PORT_UART6 5           /* LPUART1 */

#define PORT_UART_MAX_NUM 6

/***************************** usart1 configure *******************************/
#define UART1                          USART1

#define UART1_BANDRATE                 115200
#define UART1_WORD_LENGTH              UART_WORDLENGTH_8B
#define UART1_STOP_BITS                UART_STOPBITS_1
#define UART1_PARITY                   UART_PARITY_NONE
#define UART1_MODE                     UART_MODE_TX_RX
#define UART1_HW_FLOW_CTL              UART_HWCONTROL_NONE
#define UART1_OVER_SAMPLING            UART_OVERSAMPLING_16


#define UART1_IRQn                     USART1_IRQn
#define UART1_CLK_ENABLE()             __HAL_RCC_USART1_CLK_ENABLE()
#define UART1_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define UART1_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()


/***************************** usart2 configure *******************************/
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

#define UART2_IRQn                     USART2_IRQn

#define UART2_CLK_ENABLE()             __HAL_RCC_USART2_CLK_ENABLE()
#define UART2_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define UART2_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()


/***************************** usart3 configure *******************************/
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

#define UART3_IRQn                     USART3_IRQn

#define UART3_CLK_ENABLE()             __HAL_RCC_USART3_CLK_ENABLE()
#define UART3_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()
#define UART3_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()


#define MAX_BUF_UART_BYTES  1024

#ifdef __cplusplus
}
#endif

#endif /* __HAL_UART_STM32L4_H */
