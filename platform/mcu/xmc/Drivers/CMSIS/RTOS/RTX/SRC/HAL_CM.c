/*----------------------------------------------------------------------------
 *      CMSIS-RTOS  -  RTX
 *----------------------------------------------------------------------------
 *      Name:    HAL_CM.C
 *      Purpose: Hardware Abstraction Layer for Cortex-M
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

#include "rt_TypeDef.h"
#include "RTX_Config.h"
#include "rt_HAL_CM.h"


/*----------------------------------------------------------------------------
 *      Global Variables
 *---------------------------------------------------------------------------*/

#ifdef DBG_MSG
BIT dbg_msg;
#endif

/*----------------------------------------------------------------------------
 *      Functions
 *---------------------------------------------------------------------------*/


/*--------------------------- rt_init_stack ---------------------------------*/

void rt_init_stack (P_TCB p_TCB, FUNCP task_body) {
  /* Prepare TCB and saved context for a first time start of a task. */
  U32 *stk,i,size;

  /* Prepare a complete interrupt frame for first task start */
  size = p_TCB->priv_stack >> 2;
  if (size == 0U) {
    size = (U16)os_stackinfo >> 2;
  }

  /* Write to the top of stack. */
  stk = &p_TCB->stack[size];

  /* Auto correct to 8-byte ARM stack alignment. */
  if ((U32)stk & 0x04U) {
    stk--;
  }

  stk -= 16;

  /* Default xPSR and initial PC */
  stk[15] = INITIAL_xPSR;
  stk[14] = (U32)task_body;

  /* Clear R4-R11,R0-R3,R12,LR registers. */
  for (i = 0U; i < 14U; i++) {
    stk[i] = 0U;
  }

  /* Assign a void pointer to R0. */
  stk[8] = (U32)p_TCB->msg;

  /* Initial Task stack pointer. */
  p_TCB->tsk_stack = (U32)stk;

  /* Task entry point. */
  p_TCB->ptask = task_body;

  /* Initialize stack with magic pattern. */
  if (os_stackinfo & 0x10000000U) {
    if (size > (16U+1U)) {
      for (i = ((size - 16U)/2U) - 1U; i; i--) {
        stk -= 2U;
        stk[1] = MAGIC_PATTERN;
        stk[0] = MAGIC_PATTERN;
      }
      if (--stk > p_TCB->stack) {
        *stk = MAGIC_PATTERN;
      }
    }
  }

  /* Set a magic word for checking of stack overflow. */
  p_TCB->stack[0] = MAGIC_WORD;
}


/*--------------------------- rt_ret_val ----------------------------------*/

static __inline U32 *rt_ret_regs (P_TCB p_TCB) {
  /* Get pointer to task return value registers (R0..R3) in Stack */
#if defined(__TARGET_FPU_VFP)
  if (p_TCB->stack_frame) {
    /* Extended Stack Frame: R4-R11,S16-S31,R0-R3,R12,LR,PC,xPSR,S0-S15,FPSCR */
    return (U32 *)(p_TCB->tsk_stack + (8U*4U) + (16U*4U));
  } else {
    /* Basic Stack Frame: R4-R11,R0-R3,R12,LR,PC,xPSR */
    return (U32 *)(p_TCB->tsk_stack + (8U*4U));
  }
#else
  /* Stack Frame: R4-R11,R0-R3,R12,LR,PC,xPSR */
  return (U32 *)(p_TCB->tsk_stack + (8U*4U));
#endif
}

void rt_ret_val (P_TCB p_TCB, U32 v0) {
  U32 *ret;

  ret = rt_ret_regs(p_TCB);
  ret[0] = v0;
}

void rt_ret_val2(P_TCB p_TCB, U32 v0, U32 v1) {
  U32 *ret;

  ret = rt_ret_regs(p_TCB);
  ret[0] = v0;
  ret[1] = v1;
}


/*--------------------------- dbg_init --------------------------------------*/

#ifdef DBG_MSG
void dbg_init (void) {
  if (((DEMCR & DEMCR_TRCENA) != 0U)     && 
      ((ITM_CONTROL & ITM_ITMENA) != 0U) &&
      ((ITM_ENABLE & (1UL << 31)) != 0U)) {
    dbg_msg = __TRUE;
  }
}
#endif

/*--------------------------- dbg_task_notify -------------------------------*/

#ifdef DBG_MSG
void dbg_task_notify (P_TCB p_tcb, BOOL create) {
  while (ITM_PORT31_U32 == 0U);
  ITM_PORT31_U32 = (U32)p_tcb->ptask;
  while (ITM_PORT31_U32 == 0U);
  ITM_PORT31_U16 = (U16)((create << 8) | p_tcb->task_id);
}
#endif

/*--------------------------- dbg_task_switch -------------------------------*/

#ifdef DBG_MSG
void dbg_task_switch (U32 task_id) {
  while (ITM_PORT31_U32 == 0U);
  ITM_PORT31_U8 = (U8)task_id;
}
#endif

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
