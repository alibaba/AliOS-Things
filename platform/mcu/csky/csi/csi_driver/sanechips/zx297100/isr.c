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
#include <csi_config.h>
#include "soc.h"
#include <csi_kernel.h>

void zx29_gpio_irqhandler(int idx);
extern void zx29_rtc_alarm_handler(void);
extern void zx29_rtc_timer_handler(void);
extern void zx29_rtc_stamp_handler(void);
extern void zx29_eflash_irqhandler(uint8_t id);
extern void zx29_usart_irqhandler(int32_t idx);
extern void zx29_wdt_irqhandler(int32_t idx);
extern void zx29_timer_irqhandler(int32_t idx);
extern void zx29_iic_irqhandler(int32_t idx);
extern void systick_handler(void);
extern void zx29_eflash_irqhandler(uint8_t id);
extern void zx29_rtc_stamp_handler(void);
extern void zx29_rtc_timer_handler(void);
extern void zx29_rtc_alarm_handler(void);
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

void CORET_IRQHandler(void)
{
#ifndef CONFIG_KERNEL_FREERTOS
    CSI_INTRPT_ENTER();
#endif

    readl(0xE000E010);

#if defined(CONFIG_KERNEL_RHINO)
    systick_handler();
#elif defined(CONFIG_KERNEL_FREERTOS)
    xPortSysTickHandler();
#elif defined(CONFIG_KERNEL_UCOS)
    OSTimeTick();
#endif

#ifndef CONFIG_KERNEL_FREERTOS
    CSI_INTRPT_EXIT();
#endif
}

void WDT_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    zx29_wdt_irqhandler(0);
    CSI_INTRPT_EXIT();
}

void USART0_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    zx29_usart_irqhandler(0);
    CSI_INTRPT_EXIT();
}

void USART1_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    zx29_usart_irqhandler(1);
    CSI_INTRPT_EXIT();
}
void USART2_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    zx29_usart_irqhandler(2);
    CSI_INTRPT_EXIT();
}

void USART3_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    zx29_usart_irqhandler(3);
    CSI_INTRPT_EXIT();
}

void i2c0_irqhandler(void)
{
    CSI_INTRPT_ENTER();
    zx29_iic_irqhandler(0);
    CSI_INTRPT_EXIT();
}

extern void  ITIM_IRQHandler(void)
{
    ;
}

void TIM0_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    zx29_timer_irqhandler(0);
    CSI_INTRPT_EXIT();
}

void TIM3_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    zx29_timer_irqhandler(3);
    CSI_INTRPT_EXIT();
}

void TIM4_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    zx29_timer_irqhandler(4);
    CSI_INTRPT_EXIT();
}

void TIM2_IRQHandler(void)
{
    ;
}

void SPI0_IRQHandler(void)
{
    ;
}

void gpio6_irqhandler(void)
{
    CSI_INTRPT_ENTER();
    zx29_gpio_irqhandler(6);
    CSI_INTRPT_EXIT();
}

void gpio13_irqhandler(void)
{
    CSI_INTRPT_ENTER();
    zx29_gpio_irqhandler(13);
    CSI_INTRPT_EXIT();
}

void eflash0_irqhandler(void)
{
    CSI_INTRPT_ENTER();
    zx29_eflash_irqhandler(0);
    CSI_INTRPT_EXIT();
}

void eflash1_irqhandler(void)
{
    CSI_INTRPT_ENTER();
    zx29_eflash_irqhandler(1);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void rtc_stamp_handler(void)
{
    CSI_INTRPT_ENTER();
    zx29_rtc_stamp_handler();
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void rtc_timer_handler(void)
{
    CSI_INTRPT_ENTER();
    zx29_rtc_timer_handler();
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void rtc_alarm_handler(void)
{
    CSI_INTRPT_ENTER();
    zx29_rtc_alarm_handler();
    CSI_INTRPT_EXIT();
}
