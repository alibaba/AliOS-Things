/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "hal_memmap.h"
#include "tee_addr_map.h"
#include "tee_comm.h"

#define EXCEPTION_STACK_SIZE    (240)

.global sys_exception_handler
.global wsc_intr_handler
.import dispatcher_main
.export g_ree_entry
/*
 * Common exception  handler. Standard exceptions come through here first
 */
    .text
    .balign 4
sys_exception_handler:

    /* get exception stack */
    subi    sp, 4
    stw     r0, (sp, 0)
    lrw     r0, __exception_stack

    /* save context r1 - r3 */
    stw     r1,  (r0,  0x04)
    stw     r2,  (r0,  0x08)
    stw     r3,  (r0,  0x0C)

    /* disable non-secure IE/EE, secure IE for safe */
    mfcr r2, cr<0, 3> /* non-secure NS_PSR */
    bclri r2, 6       /* disable IE */
    bclri r2, 8       /* disable EE */
    mtcr r2, cr<0, 3>

    /* restore original r0/sp */
    ldw     r1, (sp, 0) /* ORG r0 in r1 */
    addi    sp, 4       /* sp restore */

    /* save original r0 */
    stw     r1,  (r0,  0x00)
    /* save other registers */

    stw     r4,  (r0,  0x10)
    stw     r5,  (r0,  0x14)
    stw     r6,  (r0,  0x18)
    stw     r7,  (r0,  0x1C)
    stw     r8,  (r0,  0x20)
    stw     r9,  (r0,  0x24)
    stw     r10, (r0,  0x28)
    stw     r11, (r0,  0x2C)
    stw     r12, (r0,  0x30)
    stw     r13, (r0,  0x34)
    stw     r14, (r0,  0x38)
    stw     r15, (r0,  0x3C)

    /* save epc/epsr */
    mfcr    r1, cr<4,3> /* NS_EPC */
    mfcr    r2, cr<2,3> /* NS_EPSR */
    stw     r1,  (r0,  0x40)
    stw     r2,  (r0,  0x44)

    /* save sepc/sepsr */
    mfcr    r1, epc   /* sepc */
    mfcr    r2, epsr  /* sepsr */
    stw     r1,  (r0,  0x48)
    stw     r2,  (r0,  0x4C)

    /* save psr */
    mfcr    r1, psr       /* Get psr register */
    stw     r1,  (r0,  0x50)

    /* Don't support exception nesting */

    mfcr    r2, psr       /* Get psr register */
    lsri    r2, 16        /* Get vector in 7 bits */
    sextb   r2            /* Fill upper bytes with zero */
    mov     r1, r0        /* pass stack point to r1 */
    mov     r0, r2        /* pass exception ID to r0 */

    /* Reset safe stack pointer. */
    lrw      r2, __exception_stack + EXCEPTION_STACK_SIZE;
    mov      sp, r2

    bsr     hal_sys_exception_handler   /* exception   VSR.*/
    rte

    .text
    .balign 4
    /*
     * wsc_intr_handler: this handler only handle NTW's new request!!!
     */
wsc_intr_handler:
    /*
     * now 1. all interrupts/exceptions are unmasked.
     *     2. non-secure NS_PSR, NS_PC are pushed to non-secure stack.
     *     2. sp is secure world svc sp.
     */

    /* 1. temporary store r4-r5 */
    subi sp, 8
    stm r4-r5, (sp)

    /* 2. disable secure IE for safe */
    mfcr r4, psr   /* secure PSR */
    bclri r4, 6
    mtcr r4, psr

    /* 3. check our status if really need to handle this new request */
        /* S  == 1 */
        /* SE == 1 */
        /* SP == 0 */
        /* HS == 0 */
        /* SC == 1 */
    lrw     r5, 0xf8000000
    and     r4, r4, r5
    lsri    r4, r4, 24
    cmpnei  r4, 0xc8
    bf      __valid_condition

    /* no valid status, we should direct rte */
    ldm r4-r5, (sp)
    addi sp, 8
    rte

__valid_condition:
    ldm r4-r5, (sp)
    addi sp, 8

    /* 1. save NTW GP registers to stack */
    subi sp, GP_CONTEXT_SIZE
    stm r4-r13, (sp)
    stw r15,    (sp, 0x28)

    /* 2. restore TW GP registers from stack */
    addi sp, GP_CONTEXT_SIZE
    ldw r15,    (sp, 0x28)
    ldm r4-r13, (sp)

    /* 3. restore sp */
    subi sp, GP_CONTEXT_SIZE

    /* 4. disable secure interrupt for safe */
    psrclr   ie

    /* now we are ready for TEE dispatcher! */
    bsr     dispatcher_main

    /* we have finished one command */

    /* 1. save TW GP registers */
    addi sp, GP_CONTEXT_SIZE
    stm r4-r13, (sp)
    stw r15,    (sp, 0x28)

    /* 2. restore NTW GP registers */
    subi sp, GP_CONTEXT_SIZE
    ldw r15,    (sp, 0x28)
    ldm r4-r13, (sp)

    /* 3. restore sp */
    addi sp, GP_CONTEXT_SIZE

    /* now we are ready to return */
    rte

#if 0
    /* TODO(zhenke): SW does not support interrupt */
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
    sextb   a0
    subi    a0, 32
    bsr     ck_intc_interruptservice

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
#else
default_interrupt_handler:
    br default_interrupt_handler
#endif

    .globl ntw_entry
    .type ntw_entry, function
ntw_entry:

    /* 1. save TW GP registers to current stack */
    subi sp, GP_CONTEXT_SIZE
    stm r4-r13, (sp)
    stw r15,    (sp, 0x28)

    /* 2. clear all GP registers */
    movi r0, 0
    movi r1, 0
    movi r2, 0
    movi r3, 0
    movi r4, 0
    movi r5, 0
    movi r6, 0
    movi r7, 0
    movi r8, 0
    movi r9, 0
    movi r10, 0
    movi r11, 0
    movi r12, 0
    movi r13, 0
    movi r15, 0

    /* 3. save psr to epsr, because SC == 1 in PSR */
    mfcr    r0, psr
    mtcr    r0, epsr

    /* 4. disable EE/IE clear SP */
    bclri   r0, 29  /* clear SP */
    bclri   r0, 8   /* disable EE */
    bclri   r0, 6   /* disable IE */
    mtcr    r0, psr

    /* 5. set non-secure SP */
    lrw  r0, NTW_RW_ADDR + NTW_RW_SIZE
    subi r0, 8
    mtcr r0, cr<6, 3>

    /* 6. prepare non-secure stack */
  //  lrw     r1, g_ree_entry
  //  ldw     r1, (r1, 0)

    lrw     r1, NTW_ENTRY_ADDR
    ldw     r1, (r1, 0)         /* set pc */
    movi    r2, 0
    bseti   r2, 31              /* set psr */
    stw     r2, (r0, 0)
    stw     r1, (r0, 4)

    movi r0, 0
    movi r1, 0
    movi r2, 0

    /* Ready to NTW */
    rte

    .size ntw_entry, . - ntw_entry

.section .data
.align  2
__exception_stack:
.rept   (EXCEPTION_STACK_SIZE / 4)
.long   0x00000000
.endr

g_ree_entry:
.rept   1
.long   0x00000000
.endr
