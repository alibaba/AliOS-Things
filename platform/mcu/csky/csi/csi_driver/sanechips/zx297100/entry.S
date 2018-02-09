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

#include <csi_config.h>
.import  vectorirq_handler
.import g_top_irqstack

.text
.align 2

/******************************************************************************
 * Functions:
 *     void vectorirq_handler(void);
 * vector irq handler
 ******************************************************************************/
#ifdef CONFIG_KERNEL_NONE
.global vectorirq_handler
.type   vectorirq_handler, %function
vectorirq_handler:
    psrset  ee
#ifndef CONFIG_HAVE_VIC
    subi    sp, 68
    stm     r0-r13, (sp)
    stw     r15, (sp, 56)
    mfcr    r0, epsr
    stw     r0, (sp, 60)
    mfcr    r0, epc
    stw     r0, (sp, 64)

    lrw     r1, g_irqvector
    mfcr    r0, psr
    lsri    r0, 16
    sextb   r0
    subi    r0, 32
    lsli    r0, 2
    add     r1, r0
    ldw     r1, (r1)
    lsri    r0, 2
    jsr     r1

    #movi    r2, 0
    #stw     r2, (r0)

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
#endif /* CONFIG_KERNEL_NONE */

/******************************************************************************
 * Functions:
 *     void trap(void);
 * default exception handler
 ******************************************************************************/
    .global trap
    .type   trap, %function
trap:
    psrset  ee

    mov     r0, r0
    subi    sp, 72
    stm     r0-r13, (sp)
    mov     r0, sp
    addi    r0, 72
    stw     r0, (sp, 56)
    stw     r15, (sp, 60)
    mfcr    r0, epsr
    stw     r0, (sp, 64)
    mfcr    r0, epc
    stw     r0, (sp, 68)

    mov     r0, sp

    /* for backtrace */
    subi    sp, 8
    stw     r8, (sp, 0)
    stw     r15, (sp, 4)
    mov     r8, sp

    lrw     r1, trap_c
    jmp     r1
