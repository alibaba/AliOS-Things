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

.global watchpoint_handler

.import decode_irq
.import g_top_irqstack
.import csky_get_tee_caller_task
.import csky_deal_tee_caller_task
.import g_irqvector

/******************************************************************************
 * Functions:
 *     void watchpoint_handler(void);
 * watchpoint exception handler
 ******************************************************************************/

#if defined(CONFIG_WATCHPOINT)
    .align  2
    .globl  watchpoint_handler
    .type   watchpoint_handler, %function
watchpoint_handler:
    psrset  ee
    subi    sp, 72
    stm     r0-r15, (sp)
    addi    r0, sp, 72
    stw     r0, (sp, 56)
    mfcr    r0, epsr
    stw     r0, (sp, 64)
    mfcr    r0, epc
    stw     r0, (sp, 68)

    ldh     r0, (r0)
    cmpnei  r0, 0
    lrw     r0, wp_get_callback
    lrw     r1, trap_c
    movf    r0, r1
    jsr     r0

    mov     r0, sp

    ldw     r0, (sp, 64)
    mtcr    r0, epsr
    ldw     r0, (sp, 68)
    mtcr    r0, epc

    ldm     r0-r13, (sp)
    ldw     r15, (sp, 60)
    ldw     sp, (sp, 56)

    rte

    .size   watchpoint_handler, . - watchpoint_handler
#endif

/******************************************************************************
 * Functions:
 *     void NOVIC_IRQ_Default_Handler(void);
 * novic default irq entry
 ******************************************************************************/

.global NOVIC_IRQ_Default_Handler
.type   NOVIC_IRQ_Default_Handler, %function
NOVIC_IRQ_Default_Handler:
    psrset  ee
#ifndef CONFIG_HAVE_VIC
    subi    sp, 68
    stm     r0-r13, (sp)
    stw     r15, (sp, 56)
    mfcr    r0, epsr
    stw     r0, (sp, 60)
    mfcr    r0, epc
    stw     r0, (sp, 64)

#ifdef CONFIG_STACK_GUARD
    lrw     r0, g_top_irqstack
    mtcr    r0, cr<1, 4>
    subi    r0, CONFIG_ARCH_INTERRUPTSTACK
    mtcr    r0, cr<2, 4>

    mfcr    r0, cr<0, 4>
    bseti   r0, 0
    bseti   r0, 1
    mtcr    r0, cr<0, 4>
#endif

    lrw     r0, g_active_task
    ldw     r0, (r0)
    stw     sp, (r0)

    lrw     sp, g_top_irqstack

#if (YUNOS_CONFIG_TASK_STACK_OVF_CHECK > 0)
    jbsr    krhino_stack_ovf_check
#endif

#ifdef CONFIG_SUPPORT_REE_SCHEDULE_IN_TEE
    jbsr    csky_get_tee_caller_task
#endif

    lrw     r1, g_irqvector
    mfcr    r0, psr
    lsri    r0, 16
    sextb   r0
    cmphsi  r0, 11
    bt      .Lautovector
    jbsr    decode_irq
    br      .Lreturn
.Lautovector:
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

.Lreturn:

#ifdef CONFIG_SUPPORT_REE_SCHEDULE_IN_TEE
    jbsr    csky_deal_tee_caller_task
#endif

#ifdef CONFIG_STACK_GUARD
    jbsr    csky_set_stackbound
#endif

    lrw     r0, g_active_task
    ldw     r0, (r0)
    ldw     sp, (r0)

#ifdef CONFIG_STACK_GUARD
    mfcr    r3, cr<0, 4>
    bseti   r3, 0
    bseti   r3, 1
    mtcr    r3, cr<0, 4>
#endif

    ldw     r0, (sp, 64)
    mtcr    r0, epc
    ldw     r0, (sp, 60)
    mtcr    r0, epsr
    ldm     r0-r13, (sp)
    ldw     r15, (sp, 56)
    addi    sp, 68

    rte
#else /* CONFIG_HAVE_VIC */
    bkpt
#endif /* CONFIG_HAVE_VIC */

    .align  1
    .weak   decode_irq
    .type   decode_irq, %function
decode_irq:
    rts
    .size   decode_irq, . - decode_irq
