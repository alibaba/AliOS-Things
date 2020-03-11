/*----------------------------------------------------------------------------
 *      CMSIS-RTOS  -  RTX
 *----------------------------------------------------------------------------
 *      Name:    HAL_CM0.S
 *      Purpose: Hardware Abstraction Layer for Cortex-M0
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

        NAME    HAL_CM0.S

        #define TCB_TSTACK 40

        EXTERN  os_flags
        EXTERN  os_tsk
        EXTERN  rt_alloc_box
        EXTERN  rt_free_box
        EXTERN  rt_stk_check
        EXTERN  rt_pop_req
        EXTERN  rt_systick
        EXTERN  os_tick_irqack
        EXTERN  SVC_Table
        EXTERN  SVC_Count

/*----------------------------------------------------------------------------
 *      Functions
 *---------------------------------------------------------------------------*/

        SECTION .text:CODE:NOROOT(2)
        THUMB

/*--------------------------- rt_set_PSP ------------------------------------*/

;       void rt_set_PSP (U32 stack);

        PUBLIC  rt_set_PSP
rt_set_PSP:

        MSR     PSP,R0
        BX      LR


/*--------------------------- rt_get_PSP ------------------------------------*/

;       U32 rt_get_PSP (void);

        PUBLIC  rt_get_PSP
rt_get_PSP:

        MRS     R0,PSP
        BX      LR


/*--------------------------- os_set_env ------------------------------------*/

;       void os_set_env (void);
        /* Switch to Unprivileged/Privileged Thread mode, use PSP. */

        PUBLIC  os_set_env
os_set_env:

        MOV     R0,SP                   /* PSP = MSP */
        MSR     PSP,R0
        LDR     R0,=os_flags
        LDRB    R0,[R0]
        LSLS    R0,#31
        BNE     PrivilegedE
        MOVS    R0,#0x03                /* Unprivileged Thread mode, use PSP */
        MSR     CONTROL,R0
        BX      LR
PrivilegedE:
        MOVS    R0,#0x02                /* Privileged Thread mode, use PSP */
        MSR     CONTROL,R0
        BX      LR


/*--------------------------- _alloc_box ------------------------------------*/

;      void *_alloc_box (void *box_mem);
       /* Function wrapper for Unprivileged/Privileged mode. */

        PUBLIC  _alloc_box
_alloc_box:

        LDR     R3,=rt_alloc_box
        MOV     R12,R3
        MRS     R3,IPSR
        LSLS    R3,#24
        BNE     PrivilegedA
        MRS     R3,CONTROL
        LSLS    R3,#31
        BEQ     PrivilegedA
        SVC     0
        BX      LR
PrivilegedA:
        BX      R12


/*--------------------------- _free_box -------------------------------------*/

;       U32 _free_box (void *box_mem, void *box);
        /* Function wrapper for Unprivileged/Privileged mode. */

        PUBLIC  _free_box
_free_box:

        LDR     R3,=rt_free_box
        MOV     R12,R3
        MRS     R3,IPSR
        LSLS    R3,#24
        BNE     PrivilegedF
        MRS     R3,CONTROL
        LSLS    R3,#31
        BEQ     PrivilegedF
        SVC     0
        BX      LR
PrivilegedF:
        BX      R12


/*-------------------------- SVC_Handler ------------------------------------*/

;       void SVC_Handler (void);

        PUBLIC  SVC_Handler
SVC_Handler:

        MRS     R0,PSP                  /* Read PSP */
        LDR     R1,[R0,#24]             /* Read Saved PC from Stack */
        SUBS    R1,R1,#2                /* Point to SVC Instruction */
        LDRB    R1,[R1]                 /* Load SVC Number */
        CMP     R1,#0
        BNE     SVC_User                /* User SVC Number > 0 */

        MOV     LR,R4
        LDMIA   R0,{R0-R3,R4}           /* Read R0-R3,R12 from stack */
        MOV     R12,R4
        MOV     R4,LR
        BLX     R12                     /* Call SVC Function */

        MRS     R3,PSP                  /* Read PSP */
        STMIA   R3!,{R0-R2}             /* Store return values */

        LDR     R3,=os_tsk
        LDMIA   R3!,{R1,R2}             /* os_tsk.run, os_tsk.next */
        CMP     R1,R2
        BEQ     SVC_Exit                /* no task switch */

        SUBS    R3,#8
        CMP     R1,#0                   /* Runtask deleted? */
        BEQ     SVC_Next

        MRS     R0,PSP                  /* Read PSP */
        SUBS    R0,R0,#32               /* Adjust Start Address */
        STR     R0,[R1,#TCB_TSTACK]     /* Update os_tsk.run->tsk_stack */       
        STMIA   R0!,{R4-R7}             /* Save old context (R4-R7) */
        MOV     R4,R8
        MOV     R5,R9
        MOV     R6,R10
        MOV     R7,R11
        STMIA   R0!,{R4-R7}             /* Save old context (R8-R11) */

        PUSH    {R2,R3}
        BL      rt_stk_check            /* Check for Stack overflow */
        POP     {R2,R3}

SVC_Next:
        STR     R2,[R3]                 /* os_tsk.run = os_tsk.next */

        LDR     R0,[R2,#TCB_TSTACK]     /* os_tsk.next->tsk_stack */
        ADDS    R0,R0,#16               /* Adjust Start Address */
        LDMIA   R0!,{R4-R7}             /* Restore new Context (R8-R11) */
        MOV     R8,R4
        MOV     R9,R5
        MOV     R10,R6
        MOV     R11,R7
        MSR     PSP,R0                  /* Write PSP */
        SUBS    R0,R0,#32               /* Adjust Start Address */
        LDMIA   R0!,{R4-R7}             /* Restore new Context (R4-R7) */

SVC_Exit:
        MOVS    R0,#~0xFFFFFFFD         /* Set EXC_RETURN value */
        MVNS    R0,R0
        BX      R0                      /* RETI to Thread Mode, use PSP */

        /*------------------- User SVC ------------------------------*/

SVC_User:
        PUSH    {R4,LR}                 /* Save Registers */
        LDR     R2,=SVC_Count
        LDR     R2,[R2]
        CMP     R1,R2
        BHI     SVC_Done                /* Overflow */

        LDR     R4,=SVC_Table-4
        LSLS    R1,R1,#2
        LDR     R4,[R4,R1]              /* Load SVC Function Address */
        MOV     LR,R4

        LDMIA   R0,{R0-R3,R4}           /* Read R0-R3,R12 from stack */
        MOV     R12,R4
        BLX     LR                      /* Call SVC Function */

        MRS     R4,PSP                  /* Read PSP */
        STMIA   R4!,{R0-R3}             /* Function return values */
SVC_Done:
        POP     {R4,PC}                 /* RETI */
        

/*-------------------------- PendSV_Handler ---------------------------------*/

;       void PendSV_Handler (void);

        PUBLIC  PendSV_Handler
PendSV_Handler:

        BL      rt_pop_req

Sys_Switch:
        LDR     R3,=os_tsk
        LDMIA   R3!,{R1,R2}             /* os_tsk.run, os_tsk.next */
        CMP     R1,R2
        BEQ     Sys_Exit                /* no task switch */

        SUBS    R3,#8

        MRS     R0,PSP                  /* Read PSP */
        SUBS    R0,R0,#32               /* Adjust Start Address */
        STR     R0,[R1,#TCB_TSTACK]     /* Update os_tsk.run->tsk_stack */
        STMIA   R0!,{R4-R7}             /* Save old context (R4-R7) */
        MOV     R4,R8
        MOV     R5,R9
        MOV     R6,R10
        MOV     R7,R11
        STMIA   R0!,{R4-R7}             /* Save old context (R8-R11) */

        PUSH    {R2,R3}
        BL      rt_stk_check            /* Check for Stack overflow */
        POP     {R2,R3}

        STR     R2,[R3]                 /* os_tsk.run = os_tsk.next */

        LDR     R0,[R2,#TCB_TSTACK]     /* os_tsk.next->tsk_stack */
        ADDS    R0,R0,#16               /* Adjust Start Address */
        LDMIA   R0!,{R4-R7}             /* Restore new Context (R8-R11) */
        MOV     R8,R4
        MOV     R9,R5
        MOV     R10,R6
        MOV     R11,R7
        MSR     PSP,R0                  /* Write PSP */
        SUBS    R0,R0,#32               /* Adjust Start Address */
        LDMIA   R0!,{R4-R7}             /* Restore new Context (R4-R7) */

Sys_Exit:
        MOVS    R0,#~0xFFFFFFFD         /* Set EXC_RETURN value */
        MVNS    R0,R0
        BX      R0                      /* RETI to Thread Mode, use PSP */


/*-------------------------- SysTick_Handler --------------------------------*/

;       void SysTick_Handler (void);

        PUBLIC  SysTick_Handler
SysTick_Handler:

        BL      rt_systick
        B       Sys_Switch


/*-------------------------- OS_Tick_Handler --------------------------------*/

;       void OS_Tick_Handler (void);

        PUBLIC  OS_Tick_Handler
OS_Tick_Handler:

        BL      os_tick_irqack
        BL      rt_systick
        B       Sys_Switch


        END

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
