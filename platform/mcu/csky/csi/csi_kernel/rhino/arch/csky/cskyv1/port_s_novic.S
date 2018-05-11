/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
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

#include <k_config.h>
#include <csi_config.h>

.extern g_active_task
.extern g_preferred_ready_task

/******************************************************************************
 *                                 EXPORT FUNCTIONS
 ******************************************************************************/

.global cpu_intrpt_save
.global cpu_intrpt_restore
.global cpu_task_switch
.global cpu_intrpt_switch
.global cpu_first_task_start

/******************************************************************************
 *                                 EQUATES
 ******************************************************************************/

/******************************************************************************
 *                                 CODE GENERATION DIRECTIVES
 ******************************************************************************/

.text
.align 2

/******************************************************************************
 * Functions:
 *     size_t cpu_intrpt_save(void);
 *     void   cpu_intrpt_restore(size_t psr);
 ******************************************************************************/

.type cpu_intrpt_save, %function
cpu_intrpt_save:
    mfcr    r2, psr
    psrclr  ie
    rts

.type cpu_intrpt_restore, %function
cpu_intrpt_restore:
    mtcr    r2, psr
    rts

/******************************************************************************
 * Functions:
 *     void cpu_intrpt_switch(void);
 *     void cpu_task_switch(void);
 ******************************************************************************/

.type cpu_task_switch, %function
cpu_task_switch:
    lrw     r2, g_intrpt_nested_level
    ldb     r2, (r2)
    cmpnei  r2, 0
    jbf     __task_switch

    lrw     r2, g_active_task
    lrw     r3, g_preferred_ready_task
    ldw     r4, (r3)
    stw     r4, (r2)

    rts


.type cpu_intrpt_switch, %function
cpu_intrpt_switch:
    lrw     r2, g_active_task
    lrw     r3, g_preferred_ready_task
    ldw     r4, (r3)
    stw     r4, (r2)

    rts

/******************************************************************************
 * Functions:
 *     void cpu_first_task_start(void);
 ******************************************************************************/

.type cpu_first_task_start, %function
cpu_first_task_start:
    psrclr  ie
    jbr     __task_switch_nosave

/******************************************************************************
 * Functions:
 *     void __task_switch(void);
 ******************************************************************************/

.type __task_switch, %function
__task_switch:
    subi     sp, 32
    subi     sp, 32
    subi     sp, 12
    stw      r1, (sp, 8)
    mfhi     r1
    stw      r1, (sp, 4)
    mflo     r1
    stw      r1, (sp, 0)
    addi     sp, 12
    stm      r2-r15, (sp)
    subi     sp, 12
    mov      r13, sp
    addi     r13, 32
    addi     r13, 32
    addi     r13, 4
    mfcr     r14, epsr
    stw      r14, (r13)
    stw      r15, (r13, 4)
#ifdef __CSKY_HARD_FLOAT__
    /* Save FPU general regs task struct */
    subi     sp, 32
    subi     sp, 32
    subi     sp, 32
    subi     sp, 32
    fmfs     a1, fr0
    fmfs     a2, fr1
    fmfs     a3, fr2
    fmfs     a4, fr3
    stw      a1, (sp, 0)
    stw      a2, (sp, 4)
    stw      a3, (sp, 8)
    stw      a4, (sp, 12)
    fmfs     a1, fr4
    fmfs     a2, fr5
    fmfs     a3, fr6
    fmfs     a4, fr7
    stw      a1, (sp, 16)
    stw      a2, (sp, 20)
    stw      a3, (sp, 24)
    stw      a4, (sp, 28)
    addi     sp, 32
    fmfs     a1, fr8
    fmfs     a2, fr9
    fmfs     a3, fr10
    fmfs     a4, fr11
    stw      a1, (sp, 0)
    stw      a2, (sp, 4)
    stw      a3, (sp, 8)
    stw      a4, (sp, 12)
    fmfs     a1, fr12
    fmfs     a2, fr13
    fmfs     a3, fr14
    fmfs     a4, fr15
    stw      a1, (sp, 16)
    stw      a2, (sp, 20)
    stw      a3, (sp, 24)
    stw      a4, (sp, 28)
    addi     sp, 32
    fmfs     a1, fr16
    fmfs     a2, fr17
    fmfs     a3, fr18
    fmfs     a4, fr19
    stw      a1, (sp, 0)
    stw      a2, (sp, 4)
    stw      a3, (sp, 8)
    stw      a4, (sp, 12)
    fmfs     a1, fr20
    fmfs     a2, fr21
    fmfs     a3, fr22
    fmfs     a4, fr23
    stw      a1, (sp, 16)
    stw      a2, (sp, 20)
    stw      a3, (sp, 24)
    stw      a4, (sp, 28)
    fmfs     a1, fr24
    fmfs     a2, fr25
    fmfs     a3, fr26
    fmfs     a4, fr27
    addi     sp, 32
    stw      a1, (sp, 0)
    stw      a2, (sp, 4)
    stw      a3, (sp, 8)
    stw      a4, (sp, 12)
    fmfs     a1, fr28
    fmfs     a2, fr29
    fmfs     a3, fr30
    fmfs     a4, fr31
    stw      a1, (sp, 16)
    stw      a2, (sp, 20)
    stw      a3, (sp, 24)
    stw      a4, (sp, 28)
    subi     sp, 32
    subi     sp, 32
    subi     sp, 32
#endif

    lrw      r2, g_active_task
    ldw      r3, (r2)
    stw      sp, (r3)

#if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
    jbsr     krhino_stack_ovf_check
#endif

__task_switch_nosave:

    lrw      r2, g_preferred_ready_task
    lrw      r3, g_active_task
    ldw      r4, (r2)
    stw      r4, (r3)
    ldw      sp, (r4)

#ifdef __CSKY_HARD_FLOAT__
    ldw      a1, (sp, 0)
    ldw      a2, (sp, 4)
    ldw      a3, (sp, 8)
    ldw      a4, (sp, 12)
    fmts     a1, fr0
    fmts     a2, fr1
    fmts     a3, fr2
    fmts     a4, fr3
    ldw      a1, (sp, 16)
    ldw      a2, (sp, 20)
    ldw      a3, (sp, 24)
    ldw      a4, (sp, 28)
    fmts     a1, fr4
    fmts     a2, fr5
    fmts     a3, fr6
    fmts     a4, fr7
    addi     sp, 32
    ldw      a1, (sp, 0)
    ldw      a2, (sp, 4)
    ldw      a3, (sp, 8)
    ldw      a4, (sp, 12)
    fmts     a1, fr8
    fmts     a2, fr9
    fmts     a3, fr10
    fmts     a4, fr11
    ldw      a1, (sp, 16)
    ldw      a2, (sp, 20)
    ldw      a3, (sp, 24)
    ldw      a4, (sp, 28)
    fmts     a1, fr12
    fmts     a2, fr13
    fmts     a3, fr14
    fmts     a4, fr15
    addi     sp, 32
    ldw      a1, (sp, 0)
    ldw      a2, (sp, 4)
    ldw      a3, (sp, 8)
    ldw      a4, (sp, 12)
    fmts     a1, fr16
    fmts     a2, fr17
    fmts     a3, fr18
    fmts     a4, fr19
    ldw      a1, (sp, 16)
    ldw      a2, (sp, 20)
    ldw      a3, (sp, 24)
    ldw      a4, (sp, 28)
    fmts     a1, fr20
    fmts     a2, fr21
    fmts     a3, fr22
    fmts     a4, fr23
    addi     sp, 32
    ldw      a1, (sp, 0)
    ldw      a2, (sp, 4)
    ldw      a3, (sp, 8)
    ldw      a4, (sp, 12)
    fmts     a1, fr24
    fmts     a2, fr25
    fmts     a3, fr26
    fmts     a4, fr27
    ldw      a1, (sp, 16)
    ldw      a2, (sp, 20)
    ldw      a3, (sp, 24)
    ldw      a4, (sp, 28)
    fmts     a1, fr28
    fmts     a2, fr29
    fmts     a3, fr30
    fmts     a4, fr31
    addi     sp, 32
#endif
     ldw     r1, (sp, 0)
     mtlo    r1
     ldw     r1, (sp, 4)
     mthi    r1
     addi    sp, 8
     ldm     r1-r15, (sp)
     subi    sp, 8
     mov     r14, sp
     addi    r14, 32
     addi    r14, 32
     ldw     r15, (r14, 4)
     mtcr    r15, epsr
     ldw     r15, (r14, 8)
     mtcr    r15, epc
     addi    sp, 32
     addi    sp, 28
     ldw     r14, (sp)
     ldw     r15, (sp, 4)
     addi    sp, 16
     rte
