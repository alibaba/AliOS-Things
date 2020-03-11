/*----------------------------------------------------------------------------
 *      CMSIS-RTOS  -  RTX
 *----------------------------------------------------------------------------
 *      Name:    RTX_CONFIG.H
 *      Purpose: Exported functions of RTX_Config.c
 *      Rev.:    V4.81
 *----------------------------------------------------------------------------
 *
 * Copyright (c) 1999-2009 KEIL, 2009-2017 ARM Germany GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *---------------------------------------------------------------------------*/

#include <stdint.h>

/* Error Codes */
#define OS_ERR_STK_OVF          1U
#define OS_ERR_FIFO_OVF         2U
#define OS_ERR_MBX_OVF          3U
#define OS_ERR_TIMER_OVF        4U

/* Definitions */
#define BOX_ALIGN_8                   0x80000000U
#define _declare_box(pool,size,cnt)   U32 pool[(((size)+3)/4)*(cnt) + 3]
#define _declare_box8(pool,size,cnt)  U64 pool[(((size)+7)/8)*(cnt) + 2]
#define _init_box8(pool,size,bsize)   _init_box (pool,size,(bsize) | BOX_ALIGN_8)

/* Variables */
extern U32 mp_tcb[];
extern U64 mp_stk[];
extern U32 os_fifo[];
extern void *os_active_TCB[];

/* Constants */
extern U16 const os_maxtaskrun;
extern U32 const os_trv;
extern U8  const os_flags;
extern U32 const os_stackinfo;
extern U32 const os_rrobin;
extern U32 const os_clockrate;
extern U32 const os_timernum;
extern U16 const mp_tcb_size;
extern U32 const mp_stk_size;
extern U32 const *m_tmr;
extern U16 const mp_tmr_size;
extern U8  const os_fifo_size;

/* Functions */
extern void os_idle_demon   (void);
extern S32  os_tick_init    (void);
extern U32  os_tick_val     (void);
extern U32  os_tick_ovf     (void);
extern void os_tick_irqack  (void);
extern void os_tmr_call     (U16 info);
extern void os_error        (uint32_t err_code);

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
