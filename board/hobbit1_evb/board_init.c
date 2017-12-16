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
#include "soc.h"
#include <config.h>
#include <csi_core.h>
#include <csi_reg.h>
#include "pin.h"

extern void hobbit_ioreuse_initial(void);

void __attribute__ ((weak)) board_init(void)
{
    hobbit_ioreuse_initial();

    drv_nvic_init(2);

    drv_coret_config(SYSTEM_CLOCK / CONFIG_SYSTICK_HZ, CORET_IRQn);
#ifndef CONFIG_KERNEL_NONE
    drv_coret_config(SYSTEM_CLOCK / CONFIG_SYSTICK_HZ, CORET_IRQn);
#endif

#if defined(CONFIG_USART)
    extern usart_handle_t console_handle;

    /* init the console*/
    console_handle = csi_usart_initialize(CONSOLE_TXD, CONSOLE_RXD, NULL, NULL);
    /* config the UART */
    csi_usart_config(console_handle, SYSTEM_CLOCK, 115200, USART_MODE_ASYNCHRONOUS, USART_PARITY_NONE, USART_STOP_BITS_1, USART_DATA_BITS_8);
#endif
}
