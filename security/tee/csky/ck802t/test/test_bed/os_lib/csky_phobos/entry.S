/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "cksmart.h"

.global default_exception_handler
.global hw_vsr_default_exception
.global hw_vsr_autovec


.extern CK_Default_Exception_Handler
.extern CK_INTC_InterruptService


/*
 * default exception service routine, before r0 initialized
 */
    .text
    .balign 4
default_exception_handler:
    br    default_exception_handler
    rte

/*
 * Common exception  handler. Standard exceptions come through here first
 */
    .text
    .balign 4
hw_vsr_default_exception:
    br hw_vsr_default_exception /*********************************************************/
    /* save context */
    subi    sp, 28             /* Allocate space for all registers */
    stw     a0, (sp, 0)
    stw     a1, (sp, 4)
    stw     a2, (sp, 8)
    stw     a3, (sp, 12)
    stw     t0, (sp, 16)
    stw     t1, (sp, 20)
    /* r4 ~ r11 need not be saved */
    stw     lr, (sp, 24)

    subi    sp, 8
    mfcr    a3, epsr
    stw     a3, (sp, 4)    /* save epsr registwr */
    mfcr    a2, epc
    stw     a2, (sp, 0)    /* save epc register */

    // enable ie ee for exception nesting
    psrset    ie, ee

    mfcr    a0, psr        /* Get psr register */
    lsri    a0, 16        /* Get vector in 7 bits */
    sextb   a0            /* Fill upper bytes with zero */
    mov     a1, sp        /* pass stack point to r1*/
    bsr        CK_Default_Exception_Handler   /* exception   VSR.*/

    ldw     a2, (sp, 0)
    mtcr    a2, epc        /* restore the epc */
    ldw     a3, (sp, 4)
    mtcr    a3, epsr    /* restore the epsr */
    addi    sp, 8

    /* restore all the regs */
    ldw     a0, (sp, 0)
    ldw     a1, (sp, 4)
    ldw     a2, (sp, 8)
    ldw     a3, (sp, 12)
    ldw     t0, (sp, 16)
    ldw     t1, (sp, 20)
    /* r4 ~ r11 need not be saved */
    ldw     lr, (sp, 24)
    addi    sp, 28

    rte

/*
 * Normal interrupt vector handler
 */
    .text
    .balign 4
hw_vsr_autovec:
    /* save context */
    subi    sp, 28             /* Allocate space for all registers */
    stw     a0, (sp, 0)
    stw     a1, (sp, 4)
    stw     a2, (sp, 8)
    stw     a3, (sp, 12)
    stw     t0, (sp, 16)
    stw     t1, (sp, 20)
    /* r8 ~ r14 need not be saved */
    stw     lr, (sp, 24)

    subi    sp, 8
    mfcr    a3, epsr
    stw     a3, (sp, 4)    /* save epsr registwr */
    mfcr    a2, epc
    stw     a2, (sp, 0)    /* save epc register */

    /* read the interrupt vector number from interrupt status register */
    mfcr    a0, psr
    lsri    a0, 16
    sextb    a0
    subi    a0, 32
    bsr        CK_INTC_InterruptService


    ldw     a3, (sp, 0)
    mtcr    a3, epc        /* restore the epc */
    ldw     a2, (sp, 4)
    mtcr    a2, epsr    /* restore the epsr */
    addi    sp, 8

    /* restore all the regs */
    ldw     a0, (sp, 0)
    ldw     a1, (sp, 4)
    ldw     a2, (sp, 8)
    ldw     a3, (sp, 12)
    ldw     t0, (sp, 16)
    ldw     t1, (sp, 20)
    /* r8 ~ r14 need not be saved */
    ldw     lr, (sp, 24)
    addi    sp, 28

    rte

// vincent 
.global wsc_entry
.type   wsc_entry, %function
wsc_entry:
    psrset   ie, ee

    // save TW GPR
    subi sp, 0x30
    stm r4-r13, (sp)
    stw r15,    (sp, 0x28)

    bsr wsc_handler

    // restore TW GPR
    ldw r15,    (sp, 0x28)
    ldm r4-r13, (sp)
    addi sp, 0x30

    rte
