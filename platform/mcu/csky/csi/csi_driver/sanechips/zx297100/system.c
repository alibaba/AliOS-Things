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
 * @file     system.c
 * @brief    CSI Device System Source File for PHOBOS
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <csi_config.h>
#include "soc.h"
#include "csi_core.h"
#include "drv_timer.h"

#ifndef CONFIG_SYSTICK_HZ
#define CONFIG_SYSTICK_HZ          100
#endif

#ifndef CONFIG_LPM_TICKLESS_CNTTIM
#define CONFIG_LPM_TICKLESS_CNTTIM 4
#endif

#ifndef CONFIG_LPM_TICKLESS_SYSTIM
#define CONFIG_LPM_TICKLESS_SYSTIM 3
#endif

#ifdef CONFIG_LPM_TICKLESS_SLEEP
extern timer_handle_t system_timer;
extern timer_handle_t count_timer;
extern void systick_handler(void);
#endif

extern int ckcpu_vsr_table;
extern void csi_intc_init(void);
/*----------------------------------------------------------------------------
  System Core Clock Variable
 *----------------------------------------------------------------------------*/
int SystemCoreClock = 76800000;  /* System Core Clock Frequency:76.8Mhz */

/**
  * @brief  initialize the system
  *         Initialize the psr and vbr.
  * @param  None
  * @return None
  */
void __attribute__((weak)) SystemInit(void)
{
    /* Here we may setting exception vector, MGU, cache, and so on. */
    __set_VBR((uint32_t) & (ckcpu_vsr_table));

    csi_intc_init();

#ifdef CONFIG_KERNEL_NONE
#ifdef CONFIG_SYSTEM_SECURE
    __set_PSR(0xc0000140);
#else
    __set_PSR(0x80000140);
#endif
#endif

#ifdef CONFIG_LPM_TICKLESS_SLEEP
    /* init the system timer*/
    system_timer = csi_timer_initialize(CONFIG_LPM_TICKLESS_SYSTIM, systick_handler);
    /* config the timer */
    csi_timer_config(system_timer, TIMER_MODE_RELOAD);
    //    csi_timer_set_timeout(system_timer, (1000000/CONFIG_SYSTICK_HZ));
    csi_timer_set_timeout(system_timer, 10000000);
    csi_timer_start(system_timer);

    /* init the system timer*/
    count_timer = csi_timer_initialize(CONFIG_LPM_TICKLESS_CNTTIM, NULL);
    /* config the timer */
    csi_timer_config(count_timer, TIMER_MODE_RELOAD);
    csi_timer_set_timeout(count_timer, 0);
    csi_timer_start(count_timer);
#endif

#ifndef CONFIG_LPM_TICKLESS_SLEEP
#ifndef CONFIG_KERNEL_NONE
    csi_coret_config(76800000 / CONFIG_SYSTICK_HZ, PIC_IRQ_CK_ITIMER);    //10ms
    csi_vic_enable_irq(PIC_IRQ_CK_ITIMER);
#endif
#endif

#ifndef CONFIG_KERNEL_RHINO
#ifndef CONFIG_NUTTXMM_NONE
    extern void mm_heap_initialize(void);
    mm_heap_initialize();
#endif
#endif
}
