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

.global trap

.import g_top_irqstack
.import g_irqvector
.import csi_intc_get_pending_irq
/******************************************************************************
 * Functions:
 *     void trap(void);
 * default exception handler
 ******************************************************************************/

#    .global trap
#    .type   trap, %function
#trap:
#ifdef CONFIG_STACK_GUARD
    /*FIXME:the cr<14, 1> is used temporarily*/
#    mtcr    r1, cr<14, 1>
#
#    mfcr    r1, cr<0, 4>
#    bclri   r1, 0
#    mtcr    r1, cr<0, 4>
#
#    mfcr    r1, cr<14, 1>
#endif

#    psrset  ee
#
#    mov     r0, r0
#    subi    sp, 72
#    stm     r0-r13, (sp)
#    mov     r0, sp
#    addi    r0, 72
#    stw     r0, (sp, 56)
#    stw     r15, (sp, 60)
#    mfcr    r0, epsr
#    stw     r0, (sp, 64)
#    mfcr    r0, epc
#    stw     r0, (sp, 68)
#
#    mov     r0, sp
#
#    /* for backtrace */
#    subi    sp, 8
#    stw     r8, (sp, 0)
#    stw     r15, (sp, 4)
#    mov     r8, sp
#
#    lrw     r1, trap_c
#    jmp     r1

/******************************************************************************
 * Functions:
 *     void autoirq_hander(void);
 * novic default irq entry
 ******************************************************************************/

.global autoirq_hander
.type   autoirq_hander, %function
autoirq_hander:
    psrset  ee
#ifndef CONFIG_HAVE_VIC
    subi   sp, 32
    subi   sp, 32
    subi   sp, 12
    stw    r1, (sp, 8)
    mfhi   r1
    stw    r1, (sp, 4)
    mflo   r1
    stw    r1, (sp, 0)
    addi   sp, 12
    stm    r2-r15, (sp)
    subi   sp, 12
    mov    r15, sp
    addi   r15, 32
    addi   r15, 32
    addi   r15, 4
    mfcr   r14, epsr
    stw    r14, (r15)
    mfcr   r14, epc
    stw    r14, (r15, 4)
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

    lrw     r2, g_active_task
    ldw     r2, (r2)
    stw     sp, (r2)

    lrw     r1, g_top_irqstack
    mov     sp, r1

#if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
#    jbsr    krhino_stack_ovf_check
#endif

    jbsr    krhino_intrpt_enter

    lrw     r3, g_irqvector
#    mfcr    r2, psr
#    lsri    r2, 16
#    sextb   r2
#    subi    r2, 32
#    lsli    r2, 2
#    add     r3, r2
#    ldw     r3, (r3)
#    lsri    r2, 2
#    jsr     r3
    jbsr    csi_intc_get_pending_irq
    lsli    r2, 2
    add     r3, r2
    ldw     r3, (r3)
    lsri    r2, 2
    jsr     r3

    jbsr    krhino_intrpt_exit

    lrw     r2, g_active_task
    ldw     r2, (r2)
    ldw     sp, (r2)

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
#else /* CONFIG_HAVE_VIC */
    bkpt
#endif /* CONFIG_HAVE_VIC */


/******************************************************************************
 * Functions:
 *     void NOVIC_IRQ_Default_Handler(void);
 * novic default irq entry
 ******************************************************************************/
.global NOVIC_IRQ_Default_Handler
.type   NOVIC_IRQ_Default_Handler, %function
NOVIC_IRQ_Default_Handler:
    psrset   ee
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
    mov      r15, sp
    addi     r15, 32
    addi     r15, 32
    addi     r15, 4
    mfcr     r14, epsr
    stw      r14, (r15)
    mfcr     r14, epc
    stw      r14, (r15, 4)
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

    lrw      r5, g_active_task
    ldw      r5, (r5)
    stw      sp, (r5)

    lrw      r1, g_top_irqstack
    mov      sp, r1

#if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
    jbsr     krhino_stack_ovf_check
#endif

#ifdef CONFIG_SUPPORT_REE_SCHEDULE_IN_TEE
    jbsr     csky_get_tee_caller_task
#endif

    lrw      a1, g_irqvector
    mfcr     a0, psr
    lsri     a0, 16
    sextb    a0
    subi     a0, 32
    lsli     a0, 2
    add      a1, a0
    ldw      a5, (a1)
    lsri     a0, 2
    mov      a4, a0
    jbsr     krhino_intrpt_enter_hook
    mov      a0, a4
    jsr      a5
    mov      a0, a4
    jbsr     krhino_intrpt_exit_hook

#ifdef CONFIG_SUPPORT_REE_SCHEDULE_IN_TEE
    jbsr     csky_deal_tee_caller_task
#endif

#ifdef CONFIG_STACK_GUARD
    jbsr     csky_set_stackbound
#endif

    lrw      a0, g_active_task
    ldw      a0, (a0)
    ldw      sp, (a0)

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
