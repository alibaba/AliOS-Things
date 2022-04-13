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

#ifdef __CSKY_HARD_FLOAT__
#define SAVE_VR_REGISTERS
#endif

.extern g_active_task
.extern g_preferred_ready_task
.extern krhino_task_sched_stats_get
.import csky_deal_tee_caller_task
.import csky_get_tee_caller_task
.import g_irqvector

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
    mfcr    r0, psr
    psrclr  ie
    rts

.type cpu_intrpt_restore, %function
cpu_intrpt_restore:
    mtcr    r0, psr
    rts

/******************************************************************************
 * Functions:
 *     void cpu_intrpt_switch(void);
 *     void cpu_task_switch(void);
 ******************************************************************************/

.type cpu_task_switch, %function
cpu_task_switch:
    lrw     r0, g_intrpt_nested_level
    ldb     r0, (r0)
    cmpnei  r0, 0
    jbf     __task_switch

    lrw     r0, g_active_task
    lrw     r1, g_preferred_ready_task
    ldw     r2, (r1)
    stw     r2, (r0)

    rts

.type cpu_intrpt_switch, %function
cpu_intrpt_switch:
#if (RHINO_CONFIG_TASK_SCHED_STATS > 0)
    push    r15
    jbsr    krhino_task_sched_stats_get
    ldw     r15, (sp)
    addi    sp, 4
#endif
    lrw     r0, g_active_task
    lrw     r1, g_preferred_ready_task
    ldw     r2, (r1)
    stw     r2, (r0)

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
    subi    sp, 68
    stm     r0-r13, (sp)
    stw     r15, (sp, 56)
    mfcr    r1, epsr
    stw     r1, (sp, 60)
    stw     r15, (sp, 64)

    subi    sp, 72
    stm     r16-r31, (sp)
    mflo    r1
    stw     r1, (sp, 64)
    mfhi    r1
    stw     r1, (sp, 68)

    subi    sp, 64
    fstmd   vr0-vr15, (sp)

    lrw     r2, g_active_task
    ldw     r2, (r2)
    stw     sp, (r2)

#if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
    jbsr    krhino_stack_ovf_check
#endif

__task_switch_nosave:
#if (RHINO_CONFIG_TASK_SCHED_STATS > 0)
    jbsr    krhino_task_sched_stats_get
#endif
    lrw     r4, g_preferred_ready_task
    lrw     r5, g_active_task
    ldw     r6, (r4)
    stw     r6, (r5)

    ldw     sp, (r6)

    fldmd   vr0-vr15, (sp)
    addi    sp, 64

    ldw     r1, (sp, 68)
    mthi    r1
    ldw     r1, (sp, 64)
    mtlo    r1

    ldm     r16-r31, (sp)
    addi    sp, 72

    ldw     r0, (sp, 64)
    mtcr    r0, epc
    ldw     r0, (sp, 60)
    mtcr    r0, epsr

    ldm     r0-r13, (sp)
    ldw     r15, (sp, 56)
    addi    sp, 68

    rte

/******************************************************************************
 * Functions:
 *     void NOVIC_IRQ_Default_Handler(void);
 * novic default irq entry
 ******************************************************************************/

.section .text.NOVIC_IRQ_Default_Handler
.global NOVIC_IRQ_Default_Handler
.type   NOVIC_IRQ_Default_Handler, %function
.section .text.Default_IRQHandler
.global Default_IRQHandler
.type   Default_IRQHandler, %function
NOVIC_IRQ_Default_Handler:
Default_IRQHandler:
    subi    sp, 68
    stm     r0-r13, (sp)
    stw     r15, (sp, 56)
    mfcr    r1, epsr
    stw     r1, (sp, 60)
    mfcr    r1, epc
    stw     r1, (sp, 64)

    subi    sp, 72
    stm     r16-r31, (sp)
    mflo    r1
    stw     r1, (sp, 64)
    mfhi    r1
    stw     r1, (sp, 68)

    subi    sp, 64
    fstmd   vr0-vr15, (sp)

    lrw     r2, g_active_task
    ldw     r2, (r2)
    stw     sp, (r2)

    lrw     sp, g_top_irqstack

#if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
    jbsr    krhino_stack_ovf_check
#endif

#if CONFIG_CSI_V2
    jbsr    do_irq
#else
    lrw     r1, g_irqvector
    mfcr    r0, psr
    lsri    r0, 16
    sextb   r0
    subi    r0, 32
    lsli    r0, 2
    add     r1, r0
    ldw     r5, (r1)
    lsri    r0, 2
    mov     r4, r0
    jbsr    krhino_intrpt_enter_hook
    mov     r0, r4
    jsr     r5
    mov     r0, r4
    jbsr    krhino_intrpt_exit_hook
#endif

    lrw     r0, g_active_task
    ldw     r0, (r0)
    ldw     sp, (r0)

    fldmd   vr0-vr15, (sp)
    addi    sp, 64

    ldw     r1, (sp, 68)
    mthi    r1
    ldw     r1, (sp, 64)
    mtlo    r1

    ldm     r16-r31, (sp)
    addi    sp, 72

    ldw     r0, (sp, 64)
    mtcr    r0, epc
    ldw     r0, (sp, 60)
    mtcr    r0, epsr

    ldm     r0-r13, (sp)
    ldw     r15, (sp, 56)
    addi    sp, 68

    rte
