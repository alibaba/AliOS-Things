/**
 * \file            lwesp_ll_stm32l496g_discovery.c
 * \brief           Low-level communication with ESP device for STM32L496G-Discovery using DMA
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
 * Default UART configuration is:
 *
 * UART:                USART1
 * STM32 TX (ESP RX):   GPIOB, GPIO_PIN_6
 * STM32 RX (ESP TX):   GPIOG, GPIO_PIN_10; Note: VDDIO2 must be enabled in PWR register
 * RESET:               GPIOB, GPIO_PIN_2
 * GPIO0:               GPIOH, GPIO_PIN_2
 * GPIO2:               GPIOA, GPIO_PIN_0
 * CH_PD:               GPIOA, GPIO_PIN_4
 *
 * USART_DMA:           DMA1
 * USART_DMA_CHANNEL:   DMA_CHANNEL_5
 * USART_DMA_REQ_NUM:   2
 */

#if !__DOXYGEN__

#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_usart.h"
#include "stm32l4xx_ll_gpio.h"
#include "stm32l4xx_ll_dma.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_pwr.h"

/* USART */
#define LWESP_USART                           USART1
#define LWESP_USART_CLK                       LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1)
#define LWESP_USART_IRQ                       USART1_IRQn
#define LWESP_USART_IRQHANDLER                USART1_IRQHandler

/* DMA settings */
#define LWESP_USART_DMA                       DMA1
#define LWESP_USART_DMA_CLK                   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1)
#define LWESP_USART_DMA_RX_CH                 LL_DMA_CHANNEL_5
#define LWESP_USART_DMA_RX_REQ_NUM            LL_DMA_REQUEST_2
#define LWESP_USART_DMA_RX_IRQ                DMA1_Channel5_IRQn
#define LWESP_USART_DMA_RX_IRQHANDLER         DMA1_Channel5_IRQHandler

/* DMA flags management */
#define LWESP_USART_DMA_RX_IS_TC              LL_DMA_IsActiveFlag_TC5(LWESP_USART_DMA)
#define LWESP_USART_DMA_RX_IS_HT              LL_DMA_IsActiveFlag_HT5(LWESP_USART_DMA)
#define LWESP_USART_DMA_RX_CLEAR_TC           LL_DMA_ClearFlag_TC5(LWESP_USART_DMA)
#define LWESP_USART_DMA_RX_CLEAR_HT           LL_DMA_ClearFlag_HT5(LWESP_USART_DMA)

/* USART TX PIN */
#define LWESP_USART_TX_PORT_CLK               LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB)
#define LWESP_USART_TX_PORT                   GPIOB
#define LWESP_USART_TX_PIN                    LL_GPIO_PIN_6
#define LWESP_USART_TX_PIN_AF                 LL_GPIO_AF_7

/* USART RX PIN */
/* Since GPIOG.10 is on VDDIO2, we have to enable VDDIO2 in power management */
#define LWESP_USART_RX_PORT_CLK               do { LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOG); LL_PWR_EnableVddIO2(); } while (0)
#define LWESP_USART_RX_PORT                   GPIOG
#define LWESP_USART_RX_PIN                    LL_GPIO_PIN_10
#define LWESP_USART_RX_PIN_AF                 LL_GPIO_AF_7

/* RESET PIN */
#define LWESP_RESET_PORT_CLK                  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB)
#define LWESP_RESET_PORT                      GPIOB
#define LWESP_RESET_PIN                       LL_GPIO_PIN_2

/* GPIO0 PIN */
#define LWESP_GPIO0_PORT_CLK                  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA)
#define LWESP_GPIO0_PORT                      GPIOA
#define LWESP_GPIO0_PIN                       LL_GPIO_PIN_0

/* GPIO2 PIN */
#define LWESP_GPIO2_PORT_CLK                  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOH)
#define LWESP_GPIO2_PORT                      GPIOH
#define LWESP_GPIO2_PIN                       LL_GPIO_PIN_2

/* CH_PD PIN */
#define LWESP_CH_PD_PORT_CLK                  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA)
#define LWESP_CH_PD_PORT                      GPIOA
#define LWESP_CH_PD_PIN                       LL_GPIO_PIN_4

/* Include STM32 generic driver */
#include "../system/lwesp_ll_stm32.c"

#endif /* !__DOXYGEN__ */
