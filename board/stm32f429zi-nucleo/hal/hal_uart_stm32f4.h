/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
/**
 ******************************************************************************
 * @file    hal_uart_stm32f4.h
 * @author  MCU China FAE team
 * @version 1.0
 * @date    26/12/2018
 * @brief   aos porting layer
 ******************************************************************************
 *
 * COPYRIGHT(c) 2018 STMicroelectronics
 *
 ******************************************************************************
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

#define PORT_UART3 0 /* USART3 */
#define PORT_UART6 1 /* USART6*/

#define PORT_UART_MAX_NUM 2 /*Currently only use USART3 and USART6*/

/***************************** usart3 configure *******************************/
#define UART3 USART3

#define UART3_BANDRATE 115200
#define UART3_WORD_LENGTH UART_WORDLENGTH_8B
#define UART3_STOP_BITS UART_STOPBITS_1
#define UART3_PARITY UART_PARITY_NONE
#define UART3_MODE UART_MODE_TX_RX
#define UART3_HW_FLOW_CTL UART_HWCONTROL_NONE
#define UART3_OVER_SAMPLING UART_OVERSAMPLING_16
#define UART3_ONE_BIT_SAMPLING UART_ONE_BIT_SAMPLE_DISABLE
#define UART3_ADV_FEATURE_INIT UART_ADVFEATURE_NO_INIT

#define UART3_IRQn USART3_IRQn

#define UART3_CLK_ENABLE() __HAL_RCC_USART3_CLK_ENABLE()
#define UART3_TX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define UART3_RX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()

/***************************** usart6 configure *******************************/
#define UART6 USART6

#define UART6_BANDRATE 115200
#define UART6_WORD_LENGTH UART_WORDLENGTH_8B
#define UART6_STOP_BITS UART_STOPBITS_1
#define UART6_PARITY UART_PARITY_NONE
#define UART6_MODE UART_MODE_TX_RX
#define UART6_HW_FLOW_CTL UART_HWCONTROL_NONE
#define UART6_OVER_SAMPLING UART_OVERSAMPLING_16
#define UART6_ONE_BIT_SAMPLING UART_ONE_BIT_SAMPLE_DISABLE
#define UART6_ADV_FEATURE_INIT UART_ADVFEATURE_NO_INIT

#define UART6_IRQn USART6_IRQn

#define UART6_CLK_ENABLE() __HAL_RCC_USART6_CLK_ENABLE()
#define UART6_TX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define UART6_RX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()

#define MAX_BUF_UART_BYTES (5 * 1024)

#ifdef __cplusplus
}
#endif

#endif /* __HAL_UART_STM32L4_H */
