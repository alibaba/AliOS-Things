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
 * @file     board_init.c
 * @brief    CSI Source File for board init
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include "drv_usart.h"
#include "drv_timer.h"
#include "soc.h"
#include <config.h>
#include <csi_core.h>
#include "pin.h"
#include "drv_intc.h"
#include "interrupt.h"

#ifndef CONFIG_CLI_USART_BITS
#define CONFIG_CLI_USART_BITS 8
#endif

#ifndef CONFIG_CLI_USART_PARITY
#define CONFIG_CLI_USART_PARITY 0
#endif

#ifndef CONFIG_CLI_USART_2STOP
#define CONFIG_CLI_USART_2STOP 0
#endif

extern void dh5021a_ioreuse_initial(void);

extern void TIMER0_IRQHandler(void);

void __attribute__ ((weak)) board_init(void)
{
    dh5021a_ioreuse_initial();

    //timer_handle_t  timer_handle = csi_timer_initialize(0, NULL, (void *)NULL);
    //csi_timer_set_timeout(timer_handle, 10000);
    //csi_timer_start(timer_handle, SYSTEM_CLOCK);
    TIMER_DRV_Stop(0);
    TIMER_DRV_ConfigMode(0, TIMER_MODE_RELOAD);
    TIMER_DRV_Start(0, 10000);
    request_irq(TIM0_IRQn, (irq_handler_t)TIMER0_IRQHandler, 0, 0, 0);

    extern usart_handle_t console_handle;
    *(volatile unsigned int *)(0x11002000) = (unsigned int)(0);

    console_handle = csi_usart_initialize(CONSOLE_TXD,CONSOLE_RXD, NULL,NULL);
    csi_usart_config(console_handle, SYSTEM_CLOCK, CONFIG_CLI_USART_BAUD, USART_MODE_ASYNCHRONOUS, CONFIG_CLI_USART_PARITY, CONFIG_CLI_USART_2STOP, USART_DATA_BITS_8);
}
