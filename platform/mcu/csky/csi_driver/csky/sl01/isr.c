/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
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

/******************************************************************************
 * @file     isr.c
 * @brief    source file for the interrupt server route
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include <drv_common.h>
#include "config.h"
#include "soc.h"

extern void dw_usart_irqhandler(int32_t idx);
extern void dw_timer_irqhandler(int32_t idx);
extern void dw_gpio_irqhandler(int32_t idx);
extern void systick_handler(void);
extern void xPortSysTickHandler(void);
extern void OSTimeTick(void);

#define readl(addr) \
    ({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })

__attribute__((isr)) void CORET_IRQHandler(void)
{
    readl(0xE000E010);

#if defined(CONFIG_KERNEL_RHINO)
    systick_handler();
#elif defined(CONFIG_KERNEL_FREERTOS)
    xPortSysTickHandler();
#elif defined(CONFIG_KERNEL_UCOS)
    OSTimeTick();
#endif
}

#if defined(CONFIG_USART)
__attribute__((isr)) void USART_IRQHandler(void)
{
    dw_usart_irqhandler(0);
}
#endif

#if defined(CONFIG_TIMER)
__attribute__((isr)) void TIM0_IRQHandler(void)
{
    dw_timer_irqhandler(0);
}

__attribute__((isr)) void TIM1_IRQHandler(void)
{
    dw_timer_irqhandler(1);
}
__attribute__((isr)) void TIM2_IRQHandler(void)
{
    dw_timer_irqhandler(2);
}

__attribute__((isr)) void TIM3_IRQHandler(void)
{
    dw_timer_irqhandler(3);
}

#endif

#if defined(CONFIG_GPIO)

__attribute__((isr)) void GPIO0_IRQHandler(void)
{
    dw_gpio_irqhandler(0);
}

__attribute__((isr)) void GPIO1_IRQHandler(void)
{
    dw_gpio_irqhandler(1);
}

__attribute__((isr)) void GPIO2_IRQHandler(void)
{
    dw_gpio_irqhandler(2);
}

__attribute__((isr)) void GPIO3_IRQHandler(void)
{
    dw_gpio_irqhandler(3);
}

__attribute__((isr)) void GPIO4_IRQHandler(void)
{
    dw_gpio_irqhandler(4);
}

__attribute__((isr)) void GPIO5_IRQHandler(void)
{
    dw_gpio_irqhandler(5);
}

__attribute__((isr)) void GPIO6_IRQHandler(void)
{
    dw_gpio_irqhandler(6);
}

__attribute__((isr)) void GPIO7_IRQHandler(void)
{
    dw_gpio_irqhandler(7);
}
#endif
