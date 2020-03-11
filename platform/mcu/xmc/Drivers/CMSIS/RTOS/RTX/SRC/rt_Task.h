/*----------------------------------------------------------------------------
 *      CMSIS-RTOS  -  RTX
 *----------------------------------------------------------------------------
 *      Name:    RT_TASK.H
 *      Purpose: Task functions and system start up.
 *      Rev.:    V4.79
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

/* Definitions */

/* Values for 'state'   */
#define INACTIVE        0U
#define READY           1U
#define RUNNING         2U
#define WAIT_DLY        3U
#define WAIT_ITV        4U
#define WAIT_OR         5U
#define WAIT_AND        6U
#define WAIT_SEM        7U
#define WAIT_MBX        8U
#define WAIT_MUT        9U

/* Return codes */
#define OS_R_TMO        0x01U
#define OS_R_EVT        0x02U
#define OS_R_SEM        0x03U
#define OS_R_MBX        0x04U
#define OS_R_MUT        0x05U

#define OS_R_OK         0x00U
#define OS_R_NOK        0xFFU

/* Variables */
extern struct OS_TSK os_tsk;
extern struct OS_TCB os_idle_TCB;

/* Functions */
extern void      rt_switch_req (P_TCB p_next);
extern void      rt_dispatch   (P_TCB next_TCB);
extern void      rt_block      (U16 timeout, U8 block_state);
extern void      rt_tsk_pass   (void);
extern OS_TID    rt_tsk_self   (void);
extern OS_RESULT rt_tsk_prio   (OS_TID task_id, U8 new_prio);
extern OS_TID    rt_tsk_create (FUNCP task, U32 prio_stksz, void *stk, void *argv);
extern OS_RESULT rt_tsk_delete (OS_TID task_id);
#ifdef __CMSIS_RTOS
extern void      rt_sys_init   (void);
extern void      rt_sys_start  (void);
#else
extern void      rt_sys_init   (FUNCP first_task, U32 prio_stksz, void *stk);
#endif

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
