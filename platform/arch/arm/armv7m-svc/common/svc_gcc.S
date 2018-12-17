@
@ Copyright (C) 2015-2017 Alibaba Group Holding Limited
@

#include <k_config.h>
#include <k_default_config.h>
#include <aux_config.h>
#include <svc.h>
#include <syscall_no.h>

@
@   SVC exception description:
@
@   --------------------------------------------
@   |  no  | description                       |
@   --------------------------------------------
@   | 0x02 | return to user task               |
@   --------------------------------------------
@   | 0x03 | syscall                           |
@   --------------------------------------------
@   |others| reserved                          |
@   --------------------------------------------
@
@   svc no is defined in svc.h
@


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
    @ {R0, R1, R2, R3, R12, LR, PC, XPSR} have been
    @ saved on stack, it's safe using these registers.

    TST     LR, #0x04
    ITE     EQ
    MRSEQ   R0, MSP
    MRSNE   R0, PSP


    @ get svc instruction to get svcall no.
    LDR     R1, [R0, #6*4]
    @ svc instruction is 2 bytes
    SUBS    R1, R1, #2
    LDRB    R1, [R1]

    CMP     R1, #SVC_return_utask
    BEQ     .svc_utask_return

    @      syscall
    CMP     R1, #SVC_syscall
    BEQ     .svc_syscall

    STMDB   R0!, {R4-R11}
    BL      invalid_svcall_handler
    B       .err_svc_no

.svc_utask_return:
    LDR     R1, =g_active_task
    LDR     R1, [R1]

    @ change utask.mode to 0x11
    LDRB    R2, [R1, #RHINO_CONFIG_TASK_MODE_OFFSET]
    ORR     R2, R2, #0x01
    STRB    R2, [R1, #RHINO_CONFIG_TASK_MODE_OFFSET]

    @ change mode to user thread, set control[0]
    MRS     R2, control
    ORR     R2, #0x01
    MSR     CONTROL, R2
    @ TODO: test stub, remove it later
    MRS     R2, CONTROL

    @ switch psp to utask.ustack
    @ copy utask.kstack to utask.ustack
    LDR     R2, [R1, #RHINO_CONFIG_TASK_USTACK_OFFSET]
    PUSH    {R4-R11}
    LDMIA   R0!, {R4-r11}
    STMDB   R2!, {R4-r11}
    POP     {R4-R11}

    @ set return address on the stack
    STR     R8, [R2, #6*4]

    @ save utask.kstack
    STR     R0, [R1, #RHINO_CONFIG_TASK_KSTACK_OFFSET]

    MSR     PSP, R2
    ISB

    BX      LR

.svc_syscall:
    @ save return address
    LDR     R8, [R0, #6*4]
    LDR     R1, =do_syscall
    STR     R1, [r0, #6*4]

    @ check the return mode, if it's thumb, the last
    @ bit of the return address shouldn't be zero.
    TST     LR, #0x01
    IT      NE
    ORRNE   R8, R8, #0x01

    LDR     R1, =g_active_task
    LDR     R1, [R1]
    LDRB    R2, [R1, #RHINO_CONFIG_TASK_MODE_OFFSET]
    AND     R2, R2, #0x02
    CMP     R2, #0x02
    BNE     .return

    @ clear bit[0], set task to privilegend mode
    BIC     R2, #0x01
    STRB    R2, [R1, #RHINO_CONFIG_TASK_MODE_OFFSET]

    @ switch psp to utask.kstack
    @ copy utask.kstack to utask.ustack
    LDR     R2, [R1, #RHINO_CONFIG_TASK_KSTACK_OFFSET]
    PUSH    {R4-R11}
    LDMIA   R0!, {R4-r11}
    STMDB   R2!, {R4-r11}
    POP     {R4-R11}

    @ store psp to task.ustack
    STR     R0, [R1, #RHINO_CONFIG_TASK_USTACK_OFFSET]

    MSR     PSP, R2

    @ return to priviledged thread mode
    MRS     R0, CONTROL
    BIC     R0, R0, #0x1
    MSR     CONTROL, R0
    ISB

.return:
    BX      LR

.err_svc_no:
    B       .err_svc_no

.size SVC_Handler, .-SVC_Handler


.text
.thumb_func
.type do_syscall, function
do_syscall:
    PUSH    {R4,R5,IP,LR}

    @ align stack to 8 bytes
    MRS     R4, PSP
    BIC     R5, R4, #0x7
    MSR     PSP, R5

    CMP     R0, #MAX_SYSCALL_NO
    BGT     .err_syscall

    @ check CONTROL register value, remove it later
    MRS     IP, CONTROL
    LDR     IP, =syscall_tbl
    LDR     IP, [ip, r0, lsl #2]
    @ prepare syscall function args
    MOV     R0, r1
    BLX     IP

    @ restore stack
    MSR     PSP, R5

    @ r0, r1 may take return value, don't touch them
    @ set task cur mode to unprivileged if it's utask
    LDR     R2, =g_active_task
    LDR     R2, [R2]
    @ load mode
    LDRB    R3, [R2, #RHINO_CONFIG_TASK_MODE_OFFSET]
    AND     R3, R3, #0x02
    CMP     R3, #0x02
    BNE     .ktask_return

    @ return to app by svc
    POP     {R4,R5,IP,LR}
    SVC     #SVC_return_utask

    @ shouldn't come here...
.err_loop:
    B      .err_loop

.err_syscall:
    @ r0: syscall no, r1: syscall arg pointer r2: syscall pc
    MOV   R2, R8
    BL    invalid_syscall_handler
    B     .err_loop

.ktask_return:
    POP     {R4,R5,IP,LR}

    BX      R8

.size do_syscall, .-do_syscall

