/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <config.h>

extern void NOVIC_IRQ_Default_Handler(void);
extern void CORET_IRQHandler(void);

#ifdef CONFIG_TIMER
extern void TIMA0_IRQHandler(void);
extern void TIMA1_IRQHandler(void);
extern void TIMB0_IRQHandler(void);
extern void TIMB1_IRQHandler(void);
#endif
#ifdef CONFIG_WDT
extern void WDT_IRQHandler(void);
#endif
#ifdef CONFIG_USART
extern void USART0_IRQHandler(void);
extern void USART1_IRQHandler(void);
extern void USART2_IRQHandler(void);
extern void USART3_IRQHandler(void);
#endif
#ifdef CONFIG_IIC
extern void I2C0_IRQHandler(void);
extern void I2C1_IRQHandler(void);
#endif
#if defined(CONFIG_SPI) && defined(CONFIG_GPIO)
extern void SPI0_IRQHandler(void);
extern void SPI1_IRQHandler(void);
#endif
#ifdef CONFIG_GPIO
extern void GPIO0_IRQHandler(void);
extern void GPIO1_IRQHandler(void);
#endif
#ifdef CONFIG_RTC
extern void RTC_IRQHandler(void);
extern void RTC1_IRQHandler(void);
#endif
#ifdef CONFIG_AES
extern void AES_IRQHandler(void);
#endif
#ifdef CONFIG_SHA
extern void SHA_IRQHandler(void);
#endif

void (*g_irqvector[])(void) = {
#ifdef CONFIG_GPIO
    GPIO0_IRQHandler, /* 0, default interrupt entry */
#else
    NOVIC_IRQ_Default_Handler, /* 0, default interrupt entry */
#endif
    CORET_IRQHandler,  /* 1, default interrupt entry */
#ifdef CONFIG_TIMER
    TIMA0_IRQHandler, /* 2, default interrupt entry */
    TIMA1_IRQHandler, /* 3, default interrupt entry */
#else
    NOVIC_IRQ_Default_Handler,  /* 2, default interrupt entry */
    NOVIC_IRQ_Default_Handler,  /* 3, default interrupt entry */
#endif
    NOVIC_IRQ_Default_Handler, /* 4, default interrupt entry */
#ifdef CONFIG_WDT
    WDT_IRQHandler, /* 5, default interrupt entry */
#else
    NOVIC_IRQ_Default_Handler, /* 5, default interrupt entry */
#endif
#ifdef CONFIG_USART
    USART0_IRQHandler, /* 6, default interrupt entry */
    USART1_IRQHandler, /* 7, default interrupt entry */
    USART2_IRQHandler, /* 8, default interrupt entry */
#else
    NOVIC_IRQ_Default_Handler, /* 6, default interrupt entry */
    NOVIC_IRQ_Default_Handler, /* 7, default interrupt entry */
    NOVIC_IRQ_Default_Handler, /* 8, default interrupt entry */
#endif
#ifdef CONFIG_IIC
    I2C0_IRQHandler, /* 9, default interrupt entry */
    I2C1_IRQHandler, /* 10, default interrupt entry */
#else
    NOVIC_IRQ_Default_Handler, /* 9, default interrupt entry */
    NOVIC_IRQ_Default_Handler, /* 10, default interrupt entry */
#endif
#if defined(CONFIG_SPI) && defined(CONFIG_GPIO)
    SPI1_IRQHandler, /* 11, default interrupt entry */
    SPI0_IRQHandler, /* 12, default interrupt entry */
#else
    NOVIC_IRQ_Default_Handler, /* 11, default interrupt entry */
    NOVIC_IRQ_Default_Handler, /* 12, default interrupt entry */
#endif
#ifdef CONFIG_RTC
    RTC_IRQHandler, /* 13, default interrupt entry */
#else
    NOVIC_IRQ_Default_Handler, /* 13, default interrupt entry */
#endif
    NOVIC_IRQ_Default_Handler, /* 14, default interrupt entry */
    NOVIC_IRQ_Default_Handler, /* 15, default interrupt entry */
    NOVIC_IRQ_Default_Handler, /* 16, default interrupt entry */
    NOVIC_IRQ_Default_Handler, /* 17, default interrupt entry */
    NOVIC_IRQ_Default_Handler, /* 18, default interrupt entry */
    NOVIC_IRQ_Default_Handler, /* 19, default interrupt entry */
    NOVIC_IRQ_Default_Handler, /* 20, default interrupt entry */
#ifdef CONFIG_USART
    USART3_IRQHandler, /* 21, default interrupt entry */
#else
    NOVIC_IRQ_Default_Handler, /* 21, default interrupt entry */
#endif
    NOVIC_IRQ_Default_Handler, /* 22, default interrupt entry */
#ifdef CONFIG_TIMER
    TIMB0_IRQHandler, /* 23, default interrupt entry */
    TIMB1_IRQHandler, /* 24, default interrupt entry */
#else
    NOVIC_IRQ_Default_Handler, /* 23, default interrupt entry */
    NOVIC_IRQ_Default_Handler, /* 24, default interrupt entry */
#endif
    NOVIC_IRQ_Default_Handler, /* 25, default interrupt entry */
#ifdef CONFIG_AES
    AES_IRQHandler, /* 26, default interrupt entry */
#else
    NOVIC_IRQ_Default_Handler, /* 26, default interrupt entry */
#endif
#ifdef CONFIG_GPIO
    GPIO1_IRQHandler, /* 27, default interrupt entry */
#else
    NOVIC_IRQ_Default_Handler, /* 27, default interrupt entry */
#endif
    NOVIC_IRQ_Default_Handler, /* 28, default interrupt entry */
#ifdef CONFIG_SHA
    SHA_IRQHandler, /* 29, default interrupt entry */
#else
    NOVIC_IRQ_Default_Handler, /* 29, default interrupt entry */
#endif
};
