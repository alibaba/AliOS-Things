/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_config.h>
#include <k_default_config.h>
#include <aux_config.h>

#if (RHINO_CONFIG_USER_SPACE > 0)

.syntax unified
.thumb
.file "svc.S"

.extern syscall_tbl


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


    // get the svc instruction to get svcall no.
    ldr     r1, [r0, #6*4]
    // svc instruction is 2 bytes
    subs    r1, r1, #2
    ldrb    r1, [r1]

    cmp     r1, #0x01
    beq     .cpu_switch_to_usermode

    cmp     r1, #0x02
    beq     .svc_utask_return

    //      syscall
    cmp     r1, #0x10
    beq     .svc_syscall

.cpu_switch_to_usermode:
    ldr     r1, =g_active_task
    ldr     r1, [r1]

    // save utask.kstack, stored on stack r0
    ldr     r2, [r0]
    str     r2, [r1]

    // set task.mode[1:0]=11b
    ldr     r2, [r1, #RHINO_CONFIG_TASK_MODE_OFFSET]
    orr     r2, r2, #0x03
    str     r2, [r1, #RHINO_CONFIG_TASK_MODE_OFFSET]

    // set control[0]=1
    mrs     r2, control
    orr     r2, r2, #0x01
    msr     control, r2

    // use utask.ustack
    ldr     r2, [r1, #RHINO_CONFIG_TASK_USTACK_OFFSET]
    push    {r3-r11}
    ldmia   r0!, {r4-r11}
    stmdb   r2!, {r4-r11}
    pop     {r3-r11}
    str     r0, [r1, #RHINO_CONFIG_TASK_KSTACK_OFFSET]
    msr     psp, r2

    bx      lr

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

    bx      lr

.svc_syscall:
    // save return address
    ldr     r8, [r0, #6*4]
    ldr     r1, =do_syscall
    str     r1, [r0, #6*4]

    // check the return mode, if it's thumb, the last
    // bit of the return address shouldn't zero.
    tst     lr, #0x01
    it      ne
    orrne   r8, r8, #0x01

    ldr     r1, =g_active_task
    ldr     r1, [r1]
    ldr     r2, [r1, #RHINO_CONFIG_TASK_MODE_OFFSET] // mode offset
    and     r2, r2, #0x02
    cmp     r2, #0x02
    bne     .return

    bic     r2, #0x01 // clear bit[0], set task to privilegend mode
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

.size SVC_Handler, .-SVC_Handler


.text
.thumb_func
.type do_syscall, function
do_syscall:
    push    {r4,r8,ip,lr}

    // check CONTROL register value, remove it later
    mrs     ip, CONTROL
    ldr     ip, =syscall_tbl
    ldr     ip, [ip, r0, lsl #2]
    // prepare syscall function args
    mov     r0, r1
    blx     ip

    // set task cur mode to unprivileged if it's utask
    ldr     r1, =g_active_task
    ldr     r1, [r1]
    // load mode
    ldr     r2, [r1, #RHINO_CONFIG_TASK_MODE_OFFSET]
    and     r2, r2, #0x02
    cmp     r2, #0x02
    bne     .ktask_return

    // now that it's user task, switch stack to user stack
    // then changes system mode to user mode.


    // save utask kstack
    pop     {r4,r8,ip,lr}
    svc     #0x02 // return to utask.

    // shoudn't come here...

    orr     r2, r2, #0x01
    // store mode
    str     r2, [r1, #RHINO_CONFIG_TASK_MODE_OFFSET]
    // load ustask
    ldr     r2, [r1, #RHINO_CONFIG_TASK_USTACK_OFFSET]
    msr     psp, r2

    mrs     r2, CONTROL
    orr     r2, r2, #0x1
    msr     CONTROL, r2
    // TODO: remove it later
    mrs     r2, CONTROL

    mov     r2, r4

    ldmia   r2!, {r4,r8,ip,lr}

    // save kstack
    str     r2, [r1, #RHINO_CONFIG_TASK_KSTACK_OFFSET]
    msr     primask, r3
    bx      r8

.ktask_return:
    pop     {r4,r8,ip,lr}

    bx      r8

.size do_syscall, .-do_syscall

#endif // RHINO_CONFIG_USER_SPACE

