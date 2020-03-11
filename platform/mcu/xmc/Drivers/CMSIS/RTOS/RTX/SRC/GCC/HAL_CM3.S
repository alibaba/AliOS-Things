/*----------------------------------------------------------------------------
 *      CMSIS-RTOS  -  RTX
 *----------------------------------------------------------------------------
 *      Name:    HAL_CM3.S
 *      Purpose: Hardware Abstraction Layer for Cortex-M3
 *      Rev.:    V4.70
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

        .file   "HAL_CM3.S"
        .syntax unified

        .equ    TCB_TSTACK, 40


/*----------------------------------------------------------------------------
 *      Functions
 *---------------------------------------------------------------------------*/

        .thumb

        .section ".text"
        .align  2


/*--------------------------- rt_set_PSP ------------------------------------*/

#       void rt_set_PSP (U32 stack);

        .thumb_func
        .type   rt_set_PSP, %function
        .global rt_set_PSP
rt_set_PSP:
        .fnstart
        .cantunwind

        MSR     PSP,R0
        BX      LR

        .fnend
        .size   rt_set_PSP, .-rt_set_PSP


/*--------------------------- rt_get_PSP ------------------------------------*/

#       U32 rt_get_PSP (void);

        .thumb_func
        .type   rt_get_PSP, %function
        .global rt_get_PSP
rt_get_PSP:
        .fnstart
        .cantunwind

        MRS     R0,PSP
        BX      LR

        .fnend
        .size   rt_get_PSP, .-rt_get_PSP


/*--------------------------- os_set_env ------------------------------------*/

#       void os_set_env (void);
        /* Switch to Unprivileged/Privileged Thread mode, use PSP. */

        .thumb_func
        .type   os_set_env, %function
        .global os_set_env
os_set_env:
        .fnstart
        .cantunwind

        MOV     R0,SP                   /* PSP = MSP */
        MSR     PSP,R0
        LDR     R0,=os_flags
        LDRB    R0,[R0]
        LSLS    R0,#31
        ITE     NE
        MOVNE   R0,#0x02                /* Privileged Thread mode, use PSP */
        MOVEQ   R0,#0x03                /* Unprivileged Thread mode, use PSP */
        MSR     CONTROL,R0
        BX      LR

        .fnend
        .size   os_set_env, .-os_set_env


/*--------------------------- _alloc_box ------------------------------------*/

#      void *_alloc_box (void *box_mem);
       /* Function wrapper for Unprivileged/Privileged mode. */

        .thumb_func
        .type   _alloc_box, %function
        .global _alloc_box
_alloc_box:
        .fnstart
        .cantunwind

        LDR     R12,=rt_alloc_box
        MRS     R3,IPSR
        LSLS    R3,#24
        IT      NE
        BXNE    R12
        MRS     R3,CONTROL
        LSLS    R3,#31
        IT      EQ
        BXEQ    R12
        SVC     0
        BX      LR

        .fnend
        .size   _alloc_box, .-_alloc_box


/*--------------------------- _free_box -------------------------------------*/

#       U32 _free_box (void *box_mem, void *box);
        /* Function wrapper for Unprivileged/Privileged mode. */

        .thumb_func
        .type   _free_box, %function
        .global _free_box
_free_box:
        .fnstart
        .cantunwind

        LDR     R12,=rt_free_box
        MRS     R3,IPSR
        LSLS    R3,#24
        IT      NE
        BXNE    R12
        MRS     R3,CONTROL
        LSLS    R3,#31
        IT      EQ
        BXEQ    R12
        SVC     0
        BX      LR

        .fnend
        .size   _free_box, .-_free_box


/*-------------------------- SVC_Handler ------------------------------------*/

#       void SVC_Handler (void);

        .thumb_func
        .type   SVC_Handler, %function
        .global SVC_Handler
SVC_Handler:
        .ifdef  IFX_XMC4XXX
        .global SVC_Handler_Veneer
SVC_Handler_Veneer:
        .endif
        .fnstart
        .cantunwind

        MRS     R0,PSP                  /* Read PSP */
        LDR     R1,[R0,#24]             /* Read Saved PC from Stack */
        LDRB    R1,[R1,#-2]             /* Load SVC Number */
        CBNZ    R1,SVC_User

        LDM     R0,{R0-R3,R12}          /* Read R0-R3,R12 from stack */
        BLX     R12                     /* Call SVC Function */

        MRS     R12,PSP                 /* Read PSP */
        STM     R12,{R0-R2}             /* Store return values */

        LDR     R3,=os_tsk
        LDM     R3,{R1,R2}              /* os_tsk.run, os_tsk.next */
        CMP     R1,R2
        BEQ     SVC_Exit                /* no task switch */

        CBZ     R1,SVC_Next             /* Runtask deleted? */
        STMDB   R12!,{R4-R11}           /* Save Old context */
        STR     R12,[R1,#TCB_TSTACK]    /* Update os_tsk.run->tsk_stack */

        PUSH    {R2,R3}
        BL      rt_stk_check            /* Check for Stack overflow */
        POP     {R2,R3}

SVC_Next:
        STR     R2,[R3]                 /* os_tsk.run = os_tsk.next */

        LDR     R12,[R2,#TCB_TSTACK]    /* os_tsk.next->tsk_stack */
        LDMIA   R12!,{R4-R11}           /* Restore New Context */
        MSR     PSP,R12                 /* Write PSP */

SVC_Exit:
        MVN     LR,#~0xFFFFFFFD         /* set EXC_RETURN value */
        .ifdef  IFX_XMC4XXX
        PUSH    {LR}
        POP     {PC}
        .else
        BX      LR
        .endif

        /*------------------- User SVC ------------------------------*/

SVC_User:
        PUSH    {R4,LR}                 /* Save Registers */
        LDR     R2,=SVC_Count
        LDR     R2,[R2]
        CMP     R1,R2
        BHI     SVC_Done                /* Overflow */

        LDR     R4,=SVC_Table-4
        LDR     R4,[R4,R1,LSL #2]       /* Load SVC Function Address */

        LDM     R0,{R0-R3,R12}          /* Read R0-R3,R12 from stack */
        BLX     R4                      /* Call SVC Function */

        MRS     R12,PSP
        STM     R12,{R0-R3}             /* Function return values */
SVC_Done:
        POP     {R4,PC}                 /* RETI */

        .fnend
        .size   SVC_Handler, .-SVC_Handler
        

/*-------------------------- PendSV_Handler ---------------------------------*/

#       void PendSV_Handler (void);

        .thumb_func
        .type   PendSV_Handler, %function
        .global PendSV_Handler
        .global Sys_Switch
PendSV_Handler:
        .ifdef  IFX_XMC4XXX
        .global PendSV_Handler_Veneer
PendSV_Handler_Veneer:
        .endif
        .fnstart
        .cantunwind

        BL      rt_pop_req

Sys_Switch:
        LDR     R3,=os_tsk
        LDM     R3,{R1,R2}              /* os_tsk.run, os_tsk.next */
        CMP     R1,R2
        BEQ     Sys_Exit

        MRS     R12,PSP                 /* Read PSP */
        STMDB   R12!,{R4-R11}           /* Save Old context */
        STR     R12,[R1,#TCB_TSTACK]    /* Update os_tsk.run->tsk_stack */

        PUSH    {R2,R3}
        BL      rt_stk_check            /* Check for Stack overflow */
        POP     {R2,R3}

        STR     R2,[R3]                 /* os_tsk.run = os_tsk.next */

        LDR     R12,[R2,#TCB_TSTACK]    /* os_tsk.next->tsk_stack */
        LDMIA   R12!,{R4-R11}           /* Restore New Context */
        MSR     PSP,R12                 /* Write PSP */

Sys_Exit:
        MVN     LR,#~0xFFFFFFFD         /* set EXC_RETURN value */
        .ifdef  IFX_XMC4XXX
        PUSH    {LR}
        POP     {PC}
        .else
        BX      LR                      /* Return to Thread Mode */
        .endif

        .fnend
        .size   PendSV_Handler, .-PendSV_Handler


/*-------------------------- SysTick_Handler --------------------------------*/

#       void SysTick_Handler (void);

        .thumb_func
        .type   SysTick_Handler, %function
        .global SysTick_Handler
SysTick_Handler:
        .ifdef  IFX_XMC4XXX
        .global SysTick_Handler_Veneer
SysTick_Handler_Veneer:
        .endif
        .fnstart
        .cantunwind

        BL      rt_systick
        B       Sys_Switch

        .fnend
        .size   SysTick_Handler, .-SysTick_Handler


/*-------------------------- OS_Tick_Handler --------------------------------*/

#       void OS_Tick_Handler (void);

        .thumb_func
        .type   OS_Tick_Handler, %function
        .global OS_Tick_Handler
OS_Tick_Handler:
        .fnstart
        .cantunwind

        BL      os_tick_irqack
        BL      rt_systick
        B       Sys_Switch

        .fnend
        .size   OS_Tick_Handler, .-OS_Tick_Handler


        .end

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
