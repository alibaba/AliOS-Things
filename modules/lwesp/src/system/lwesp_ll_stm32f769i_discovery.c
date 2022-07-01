/**
 * \file            lwesp_ll_stm32f769i_discovery.c
 * \brief           Low-level communication with ESP device for STM32F769I-Discovery using DMA
 */

/*
 * Copyright (c) 2022 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwESP - Lightweight ESP-AT parser library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v1.1.2-dev
 */

/*
 * Default UART configuration is (Connector CN2 on board):
 *
 * UART:                UART5
 * STM32 TX (ESP RX):   GPIOC, GPIO_PIN_12
 * STM32 RX (ESP TX):   GPIOD, GPIO_PIN_2
 * RESET:               GPIOJ, GPIO_PIN_14
 *
 * UART_DMA:            DMA1
 * UART_DMA_STREAM:     DMA_STREAM_0
 * UART_DMA_CHANNEL:    DMA_CHANNEL_4
 */

#if !__DOXYGEN__

#include "stm32f7xx_ll_bus.h"
#include "stm32f7xx_ll_usart.h"
#include "stm32f7xx_ll_gpio.h"
#include "stm32f7xx_ll_dma.h"
#include "stm32f7xx_ll_rcc.h"

/* USART */
#define LWESP_USART                           UART5
#define LWESP_USART_CLK                       LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART5)
#define LWESP_USART_IRQ                       UART5_IRQn
#define LWESP_USART_IRQHANDLER                UART5_IRQHandler

/* DMA settings */
#define LWESP_USART_DMA                       DMA1
#define LWESP_USART_DMA_CLK                   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1)
#define LWESP_USART_DMA_RX_STREAM             LL_DMA_STREAM_0
#define LWESP_USART_DMA_RX_CH                 LL_DMA_CHANNEL_4
#define LWESP_USART_DMA_RX_IRQ                DMA1_Stream0_IRQn
#define LWESP_USART_DMA_RX_IRQHANDLER         DMA1_Stream0_IRQHandler

/* DMA flags management */
#define LWESP_USART_DMA_RX_IS_TC              LL_DMA_IsActiveFlag_TC0(LWESP_USART_DMA)
#define LWESP_USART_DMA_RX_IS_HT              LL_DMA_IsActiveFlag_HT0(LWESP_USART_DMA)
#define LWESP_USART_DMA_RX_CLEAR_TC           LL_DMA_ClearFlag_TC0(LWESP_USART_DMA)
#define LWESP_USART_DMA_RX_CLEAR_HT           LL_DMA_ClearFlag_HT0(LWESP_USART_DMA)

/* USART TX PIN */
#define LWESP_USART_TX_PORT_CLK               LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC)
#define LWESP_USART_TX_PORT                   GPIOC
#define LWESP_USART_TX_PIN                    LL_GPIO_PIN_12
#define LWESP_USART_TX_PIN_AF                 LL_GPIO_AF_8

/* USART RX PIN */
#define LWESP_USART_RX_PORT_CLK               LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD)
#define LWESP_USART_RX_PORT                   GPIOD
#define LWESP_USART_RX_PIN                    LL_GPIO_PIN_2
#define LWESP_USART_RX_PIN_AF                 LL_GPIO_AF_8

/* RESET PIN */
#define LWESP_RESET_PORT_CLK                  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOJ)
#define LWESP_RESET_PORT                      GPIOJ
#define LWESP_RESET_PIN                       LL_GPIO_PIN_14

/* Include STM32 generic driver */
#include "../system/lwesp_ll_stm32.c"

#endif /* !__DOXYGEN__ */
