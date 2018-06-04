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
 * @date     25. August 2017
 ******************************************************************************/
#include <drv_common.h>
#include <stdio.h>
#include "config.h"
#include "soc.h"
#include <csi_kernel.h>
#include "drv_intc.h"

#define readl(addr) \
    ({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })

#ifndef CONFIG_KERNEL_NONE
#define  CSI_INTRPT_ENTER() csi_kernel_intrpt_enter()
#define  CSI_INTRPT_EXIT()  csi_kernel_intrpt_exit()
#else
#define  CSI_INTRPT_ENTER()
#define  CSI_INTRPT_EXIT()
#endif

//extern uint64_t g_sys_tick_count;
//extern void dw_timer_irqhandler(void);
//extern void dw_usart_irqhandler(int32_t idx);
//extern unsigned int ADC_DRV_irqhandler(void);
extern void systick_handler(void);
extern void dw_timer0_irqhandler(void);


void yunos_tick_proc(void);

#if defined(CONFIG_USART)
/*ATTRIBUTE_ISR void USART0_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_usart_irqhandler(0);
    CSI_INTRPT_EXIT();
}*/
#endif

#if defined(CONFIG_IIC)
ATTRIBUTE_ISR void I2C0_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    //dw_iic_irqhandler(0);
    printf("i2c irq happen!\n");
    CSI_INTRPT_EXIT();
}
#endif

ATTRIBUTE_ISR void ADC_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
   // ADC_DRV_irqhandler();
    CSI_INTRPT_EXIT();
}

void ATTRIBUTE_ISR TIMER0_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
#if defined(CONFIG_KERNEL_RHINO)
    systick_handler();
#elif defined(CONFIG_KERNEL_FREERTOS)
    xPortSysTickHandler();
#elif defined(CONFIG_KERNEL_UCOS)
    OSTimeTick();
#endif

    dw_timer0_irqhandler();
    CSI_INTRPT_EXIT();
}
