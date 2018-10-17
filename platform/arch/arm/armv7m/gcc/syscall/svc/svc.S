/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_config.h>
#include <k_default_config.h>
#include <aux_config.h>
#include <svc.h>
#include <syscall_no.h>

/**
 *   SVC exception description:
 *
 *   --------------------------------------------
 *   |  no  | description                       |
 *   --------------------------------------------
 *   | 0x02 | return to user task               |
 *   --------------------------------------------
 *   | 0x03 | syscall                           |
 *   --------------------------------------------
 *   |others| reserved                          |
 *   --------------------------------------------
 *
 *   svc no is defined in svc.h
 */


.syntax unified
.thumb
.file "svc.S"

.extern syscall_tbl
.extern invalid_svcall_handler
.extern invalid_syscall_hanldler


.text
.global SVC_Handler
.type SVC_Handler, function
SVC_Handler:
    // {r0, r1, r2, r3, r12, lr, pc, xpsr} have been
    // saved on stack, it's safe using these registers.

    tst     lr, #0x04
    ite     eq
    mrseq   r0, msp
    mrsne   r0, psp


    // get svc instruction to get svcall no.
    ldr     r1, [r0, #6*4]
    // svc instruction is 2 bytes
    subs    r1, r1, #2
    ldrb    r1, [r1]

    cmp     r1, #SVC_return_utask
    beq     .svc_utask_return

    //      syscall
    cmp     r1, #SVC_syscall
    beq     .svc_syscall

    stmdb   r0!, {r4-r11}
    bl      invalid_svcall_handler
    b       .err_svc_no

.svc_utask_return:
    ldr     r1, =g_active_task
    ldr     r1, [r1]

    // change utask.mode to 0x11
    ldr     r2, [r1, #RHINO_CONFIG_TASK_MODE_OFFSET]
    orr     r2, r2, #0x01
    str     r2, [r1, #RHINO_CONFIG_TASK_MODE_OFFSET]

    // change mode to user thread, set control[0]
    mrs     r2, control
    orr     r2, #0x01
    msr     control, r2
    // TODO: test stub, remove it later
    mrs     r2, control

    // switch psp to utask.ustack
    // copy utask.kstack to utask.ustack
    ldr     r2, [r1, #RHINO_CONFIG_TASK_USTACK_OFFSET]
    push    {r4-r11}
    ldmia   r0!, {r4-r11}
    stmdb   r2!, {r4-r11}
    pop     {r4-r11}

    // set return address on the stack
    str     r8, [r2, #6*4]

    // save utask.kstack
    str     r0, [r1, #RHINO_CONFIG_TASK_KSTACK_OFFSET]

    msr     psp, r2
    isb

    bx      lr

.svc_syscall:
    // save return address
    ldr     r8, [r0, #6*4]
    ldr     r1, =do_syscall
    str     r1, [r0, #6*4]

    // check the return mode, if it's thumb, the last
    // bit of the return address shouldn't be zero.
    tst     lr, #0x01
    it      ne
    orrne   r8, r8, #0x01

    ldr     r1, =g_active_task
    ldr     r1, [r1]
    ldr     r2, [r1, #RHINO_CONFIG_TASK_MODE_OFFSET]
    and     r2, r2, #0x02
    cmp     r2, #0x02
    bne     .return

    // clear bit[0], set task to privilegend mode
    bic     r2, #0x01
    str     r2, [r1, #RHINO_CONFIG_TASK_MODE_OFFSET]

    // switch psp to utask.kstack
    // copy utask.kstack to utask.ustack
    ldr     r2, [r1, #RHINO_CONFIG_TASK_KSTACK_OFFSET]
    push    {r4-r11}
    ldmia   r0!, {r4-r11}
    stmdb   r2!, {r4-r11}
    pop     {r4-r11}

    // store psp to task.ustack
    str     r0, [r1, #RHINO_CONFIG_TASK_USTACK_OFFSET]

    msr     psp, r2

    // return to priviledged thread mode
    mrs     r0, CONTROL
    bic     r0, r0, #0x1
    msr     CONTROL, r0
    isb

.return:
    bx      lr

.err_svc_no:
    b       .err_svc_no

.size SVC_Handler, .-SVC_Handler


.text
.thumb_func
.type do_syscall, function
do_syscall:
    push    {r8,ip,lr}

    cmp     r0, #MAX_SYSCALL_NO
    bgt     .err_syscall

    // check CONTROL register value, remove it later
    mrs     ip, CONTROL
    ldr     ip, =syscall_tbl
    ldr     ip, [ip, r0, lsl #2]
    // prepare syscall function args
    mov     r0, r1
    blx     ip

    // r0, r1 may take return value, don't touch them
    // set task cur mode to unprivileged if it's utask
    ldr     r2, =g_active_task
    ldr     r2, [r2]
    // load mode
    ldr     r3, [r2, #RHINO_CONFIG_TASK_MODE_OFFSET]
    and     r3, r3, #0x02
    cmp     r3, #0x02
    bne     .ktask_return

    // return to app by svc
    pop     {r8,ip,lr}
    svc     #SVC_return_utask

    // shouldn't come here...
.err_loop:
    b      .err_loop

.err_syscall:
    // r0: syscall no, r1: syscall arg pointer r2: syscall pc
    mov   r2, r8
    bl    invalid_syscall_handler
    b     .err_loop

.ktask_return:
    pop     {r8,ip,lr}

    bx      r8

.size do_syscall, .-do_syscall

