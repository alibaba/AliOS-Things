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
#include <csi_kernel.h>

extern void dw_usart_irqhandler(int32_t idx);
extern void dw_timer_irqhandler(int32_t idx);
extern void dw_gpio_irqhandler(int32_t idx);
extern void systick_handler(void);
extern void xPortSysTickHandler(void);
extern void OSTimeTick(void);

#define readl(addr) \
    ({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })

#ifndef CONFIG_KERNEL_NONE
#define  CSI_INTRPT_ENTER() csi_kernel_intrpt_enter()
#define  CSI_INTRPT_EXIT()  csi_kernel_intrpt_exit()
#else
#define  CSI_INTRPT_ENTER()
#define  CSI_INTRPT_EXIT()
#endif

ATTRIBUTE_ISR void CORET_IRQHandler(void)
{
    CSI_INTRPT_ENTER();

    readl(0xE000E010);

#if defined(CONFIG_KERNEL_RHINO)
    systick_handler();
#elif defined(CONFIG_KERNEL_FREERTOS)
    xPortSysTickHandler();
#elif defined(CONFIG_KERNEL_UCOS)
    OSTimeTick();
#endif

    CSI_INTRPT_EXIT();
}

#if defined(CONFIG_USART)
ATTRIBUTE_ISR void USART_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_usart_irqhandler(0);
    CSI_INTRPT_EXIT();
}
#endif

#if defined(CONFIG_TIMER)
ATTRIBUTE_ISR void TIM0_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_timer_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void TIM1_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_timer_irqhandler(1);
    CSI_INTRPT_EXIT();
}
ATTRIBUTE_ISR void TIM2_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_timer_irqhandler(2);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void TIM3_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_timer_irqhandler(3);
    CSI_INTRPT_EXIT();
}

#endif

#if defined(CONFIG_GPIO)

ATTRIBUTE_ISR void GPIO0_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_gpio_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void GPIO1_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_gpio_irqhandler(1);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void GPIO2_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_gpio_irqhandler(2);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void GPIO3_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_gpio_irqhandler(3);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void GPIO4_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_gpio_irqhandler(4);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void GPIO5_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_gpio_irqhandler(5);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void GPIO6_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_gpio_irqhandler(6);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void GPIO7_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_gpio_irqhandler(7);
    CSI_INTRPT_EXIT();
}
#endif
