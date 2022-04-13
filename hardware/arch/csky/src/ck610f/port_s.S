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

.extern g_active_task
.extern g_preferred_ready_task
.extern krhino_task_sched_stats_get
.import g_irqvector
.import csi_intc_get_pending_irq

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
#if (RHINO_CONFIG_TASK_SCHED_STATS > 0)
    push    r15
    jbsr    krhino_task_sched_stats_get
    ldw     r15, (sp)
    addi    sp, 4
#endif
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
    subi    sp, 32
    subi    sp, 32
    subi    sp, 32
    subi    sp, 32
    subi    sp, 32
    subi    sp, 32
    subi    sp, 12

    stm     r1-r15, (sp)
    mov     r1, sp
    addi    r1, 32
    addi    r1, 28

    mflo    r2
    stw     r2, (r1, 0)
    mfhi    r2
    stw     r2, (r1, 4)
    addi    r1, 8

    fmfs    r2, fr0
    fmfs    r3, fr1
    fmfs    r4, fr2
    fmfs    r5, fr3
    fmfs    r6, fr4
    fmfs    r7, fr5
    fmfs    r8, fr6
    fmfs    r9, fr7
    stw     r2, (r1, 0)
    stw     r3, (r1, 4)
    stw     r4, (r1, 8)
    stw     r5, (r1, 12)
    stw     r6, (r1, 16)
    stw     r7, (r1, 20)
    stw     r8, (r1, 24)
    stw     r9, (r1, 28)
    addi    r1, 32

    fmfs    r2, fr8
    fmfs    r3, fr9
    fmfs    r4, fr10
    fmfs    r5, fr11
    fmfs    r6, fr12
    fmfs    r7, fr13
    fmfs    r8, fr14
    fmfs    r9, fr15
    stw     r2, (r1, 0)
    stw     r3, (r1, 4)
    stw     r4, (r1, 8)
    stw     r5, (r1, 12)
    stw     r6, (r1, 16)
    stw     r7, (r1, 20)
    stw     r8, (r1, 24)
    stw     r9, (r1, 28)
    addi    r1, 32

    fmfs    r2, fr16
    fmfs    r3, fr17
    fmfs    r4, fr18
    fmfs    r5, fr19
    fmfs    r6, fr20
    fmfs    r7, fr21
    fmfs    r8, fr22
    fmfs    r9, fr23
    stw     r2, (r1, 0)
    stw     r3, (r1, 4)
    stw     r4, (r1, 8)
    stw     r5, (r1, 12)
    stw     r6, (r1, 16)
    stw     r7, (r1, 20)
    stw     r8, (r1, 24)
    stw     r9, (r1, 28)
    addi    r1, 32

    fmfs    r2, fr24
    fmfs    r3, fr25
    fmfs    r4, fr26
    fmfs    r5, fr27
    fmfs    r6, fr28
    fmfs    r7, fr29
    fmfs    r8, fr30
    fmfs    r9, fr31
    stw     r2, (r1, 0)
    stw     r3, (r1, 4)
    stw     r4, (r1, 8)
    stw     r5, (r1, 12)
    stw     r6, (r1, 16)
    stw     r7, (r1, 20)
    stw     r8, (r1, 24)
    stw     r8, (r1, 28)
    addi    r1, 32

    mfcr    r2, psr
    stw     r2, (r1, 0)
    stw     r15, (r1, 4)

    lrw     r2, g_active_task
    ldw     r3, (r2)
    stw     sp, (r3)

#if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
    jbsr     krhino_stack_ovf_check
#endif

__task_switch_nosave:
#if (RHINO_CONFIG_TASK_SCHED_STATS > 0)
    jbsr    krhino_task_sched_stats_get
#endif
    lrw     r2, g_preferred_ready_task
    lrw     r3, g_active_task
    ldw     r4, (r2)
    stw     r4, (r3)
    ldw     sp, (r4)

    mov     r1, sp
    addi    r1, 32
    addi    r1, 28

    ldw     r2, (r1, 0)
    mtlo    r2
    ldw     r2, (r1, 4)
    mthi    r2
    addi    r1, 8

    ldw     r2, (r1, 0)
    ldw     r3, (r1, 4)
    ldw     r4, (r1, 8)
    ldw     r5, (r1, 12)
    ldw     r6, (r1, 16)
    ldw     r7, (r1, 20)
    ldw     r8, (r1, 24)
    ldw     r9, (r1, 28)
    fmts    r2, fr0
    fmts    r3, fr1
    fmts    r4, fr2
    fmts    r5, fr3
    fmts    r6, fr4
    fmts    r7, fr5
    fmts    r8, fr6
    fmts    r9, fr7
    addi    r1, 32

    ldw     r2, (r1, 0)
    ldw     r3, (r1, 4)
    ldw     r4, (r1, 8)
    ldw     r5, (r1, 12)
    ldw     r6, (r1, 16)
    ldw     r7, (r1, 20)
    ldw     r8, (r1, 24)
    ldw     r9, (r1, 28)
    fmts    r2, fr8
    fmts    r3, fr9
    fmts    r4, fr10
    fmts    r5, fr11
    fmts    r6, fr12
    fmts    r7, fr13
    fmts    r8, fr14
    fmts    r9, fr15
    addi    r1, 32

    ldw     r2, (r1, 0)
    ldw     r3, (r1, 4)
    ldw     r4, (r1, 8)
    ldw     r5, (r1, 12)
    ldw     r6, (r1, 16)
    ldw     r7, (r1, 20)
    ldw     r8, (r1, 24)
    ldw     r9, (r1, 28)
    fmts    r2, fr16
    fmts    r3, fr17
    fmts    r4, fr18
    fmts    r5, fr19
    fmts    r6, fr20
    fmts    r7, fr21
    fmts    r8, fr22
    fmts    r9, fr23
    addi    r1, 32

    ldw     r2, (r1, 0)
    ldw     r3, (r1, 4)
    ldw     r4, (r1, 8)
    ldw     r5, (r1, 12)
    ldw     r6, (r1, 16)
    ldw     r7, (r1, 20)
    ldw     r8, (r1, 24)
    ldw     r9, (r1, 28)
    fmts    r2, fr24
    fmts    r3, fr25
    fmts    r4, fr26
    fmts    r5, fr27
    fmts    r6, fr28
    fmts    r7, fr29
    fmts    r8, fr30
    fmts    r9, fr31
    addi    r1, 32

    ldw     r2, (r1, 0)
    mtcr    r2, epsr
    ldw     r2, (r1, 4)
    mtcr    r2, epc

    ldm     r1-r15, (sp)

    addi    sp, 32
    addi    sp, 32
    addi    sp, 32
    addi    sp, 32
    addi    sp, 32
    addi    sp, 32
    addi    sp, 12

    rte


/******************************************************************************
 * Functions:
 *     void NOVIC_IRQ_Default_Handler(void);
 * novic default irq entry
 ******************************************************************************/
.global NOVIC_IRQ_Default_Handler
.type   NOVIC_IRQ_Default_Handler, %function
NOVIC_IRQ_Default_Handler:
    psrset  ee
    subi    sp, 32
    subi    sp, 32
    subi    sp, 32
    subi    sp, 32
    subi    sp, 32
    subi    sp, 32
    subi    sp, 12

    stm     r1-r15, (sp)
    mov     r1, sp
    addi    r1, 32
    addi    r1, 28

    mflo    r2
    stw     r2, (r1, 0)
    mfhi    r2
    stw     r2, (r1, 4)
    addi    r1, 8

    fmfs    r2, fr0
    fmfs    r3, fr1
    fmfs    r4, fr2
    fmfs    r5, fr3
    fmfs    r6, fr4
    fmfs    r7, fr5
    fmfs    r8, fr6
    fmfs    r9, fr7
    stw     r2, (r1, 0)
    stw     r3, (r1, 4)
    stw     r4, (r1, 8)
    stw     r5, (r1, 12)
    stw     r6, (r1, 16)
    stw     r7, (r1, 20)
    stw     r8, (r1, 24)
    stw     r9, (r1, 28)
    addi    r1, 32

    fmfs    r2, fr8
    fmfs    r3, fr9
    fmfs    r4, fr10
    fmfs    r5, fr11
    fmfs    r6, fr12
    fmfs    r7, fr13
    fmfs    r8, fr14
    fmfs    r9, fr15
    stw     r2, (r1, 0)
    stw     r3, (r1, 4)
    stw     r4, (r1, 8)
    stw     r5, (r1, 12)
    stw     r6, (r1, 16)
    stw     r7, (r1, 20)
    stw     r8, (r1, 24)
    stw     r9, (r1, 28)
    addi    r1, 32

    fmfs    r2, fr16
    fmfs    r3, fr17
    fmfs    r4, fr18
    fmfs    r5, fr19
    fmfs    r6, fr20
    fmfs    r7, fr21
    fmfs    r8, fr22
    fmfs    r9, fr23
    stw     r2, (r1, 0)
    stw     r3, (r1, 4)
    stw     r4, (r1, 8)
    stw     r5, (r1, 12)
    stw     r6, (r1, 16)
    stw     r7, (r1, 20)
    stw     r8, (r1, 24)
    stw     r9, (r1, 28)
    addi    r1, 32

    fmfs    r2, fr24
    fmfs    r3, fr25
    fmfs    r4, fr26
    fmfs    r5, fr27
    fmfs    r6, fr28
    fmfs    r7, fr29
    fmfs    r8, fr30
    fmfs    r9, fr31
    stw     r2, (r1, 0)
    stw     r3, (r1, 4)
    stw     r4, (r1, 8)
    stw     r5, (r1, 12)
    stw     r6, (r1, 16)
    stw     r7, (r1, 20)
    stw     r8, (r1, 24)
    stw     r8, (r1, 28)
    addi    r1, 32

    mfcr    r2, epsr
    stw     r2, (r1, 0)
    mfcr    r2, epc
    stw     r2, (r1, 4)

    lrw     r5, g_active_task
    ldw     r5, (r5)
    stw     sp, (r5)

    lrw     r1, g_top_irqstack
    mov     sp, r1

#if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
    jbsr     krhino_stack_ovf_check
#endif

    lrw     a1, g_irqvector
    mfcr    a0, psr
    lsri    a0, 16
    sextb   a0
    subi    a0, 32
    lsli    a0, 2
    add     a1, a0
    ldw     a5, (a1)
    lsri    a0, 2
    mov     a4, a0
    jbsr    krhino_intrpt_enter_hook
    mov     a0, a4
    jsr     a5
    mov     a0, a4
    jbsr    krhino_intrpt_exit_hook

    lrw     a0, g_active_task
    ldw     a0, (a0)
    ldw     sp, (a0)

    mov     r1, sp
    addi    r1, 32
    addi    r1, 28

    ldw     r2, (r1, 0)
    mtlo    r2
    ldw     r2, (r1, 4)
    mthi    r2
    addi    r1, 8

    ldw     r2, (r1, 0)
    ldw     r3, (r1, 4)
    ldw     r4, (r1, 8)
    ldw     r5, (r1, 12)
    ldw     r6, (r1, 16)
    ldw     r7, (r1, 20)
    ldw     r8, (r1, 24)
    ldw     r9, (r1, 28)
    fmts    r2, fr0
    fmts    r3, fr1
    fmts    r4, fr2
    fmts    r5, fr3
    fmts    r6, fr4
    fmts    r7, fr5
    fmts    r8, fr6
    fmts    r9, fr7
    addi    r1, 32

    ldw     r2, (r1, 0)
    ldw     r3, (r1, 4)
    ldw     r4, (r1, 8)
    ldw     r5, (r1, 12)
    ldw     r6, (r1, 16)
    ldw     r7, (r1, 20)
    ldw     r8, (r1, 24)
    ldw     r9, (r1, 28)
    fmts    r2, fr8
    fmts    r3, fr9
    fmts    r4, fr10
    fmts    r5, fr11
    fmts    r6, fr12
    fmts    r7, fr13
    fmts    r8, fr14
    fmts    r9, fr15
    addi    r1, 32

    ldw     r2, (r1, 0)
    ldw     r3, (r1, 4)
    ldw     r4, (r1, 8)
    ldw     r5, (r1, 12)
    ldw     r6, (r1, 16)
    ldw     r7, (r1, 20)
    ldw     r8, (r1, 24)
    ldw     r9, (r1, 28)
    fmts    r2, fr16
    fmts    r3, fr17
    fmts    r4, fr18
    fmts    r5, fr19
    fmts    r6, fr20
    fmts    r7, fr21
    fmts    r8, fr22
    fmts    r9, fr23
    addi    r1, 32

    ldw     r2, (r1, 0)
    ldw     r3, (r1, 4)
    ldw     r4, (r1, 8)
    ldw     r5, (r1, 12)
    ldw     r6, (r1, 16)
    ldw     r7, (r1, 20)
    ldw     r8, (r1, 24)
    ldw     r9, (r1, 28)
    fmts    r2, fr24
    fmts    r3, fr25
    fmts    r4, fr26
    fmts    r5, fr27
    fmts    r6, fr28
    fmts    r7, fr29
    fmts    r8, fr30
    fmts    r9, fr31
    addi    r1, 32

    ldw     r2, (r1, 0)
    mtcr    r2, epsr
    ldw     r2, (r1, 4)
    mtcr    r2, epc

    ldm     r1-r15, (sp)

    addi    sp, 32
    addi    sp, 32
    addi    sp, 32
    addi    sp, 32
    addi    sp, 32
    addi    sp, 32
    addi    sp, 12

    rte
