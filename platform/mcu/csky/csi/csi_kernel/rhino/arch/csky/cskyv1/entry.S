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
    subi    sp, 32
    subi    sp, 28
    stm     r1-r15, (sp)
    subi    sp, 8
    mfcr    r1, epsr
    stw     r1, (sp, 4)
    mfcr    r1, epc
    stw     r1, (sp, 0)

    lrw     r2, g_active_task
    ldw     r2, (r2)
    stw     sp, (r2)

    lrw     r1, g_top_irqstack
    mov     sp, r1

#if (YUNOS_CONFIG_TASK_STACK_OVF_CHECK > 0)
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

    jbsr    krino_intrpt_exit

    lrw     r2, g_active_task
    ldw     r2, (r2)
    ldw     sp, (r2)

    movi    r4, 0
    stw     r4, (r2)

    ldw     r1, (sp, 0)
    mtcr    r1, epc
    ldw     r1, (sp, 4)
    mtcr    r1, epsr
    addi    sp, 8
    ldm     r1-r15, (sp)
    addi    sp, 32
    addi    sp, 28

    rte
#else /* CONFIG_HAVE_VIC */
    bkpt
#endif /* CONFIG_HAVE_VIC */
