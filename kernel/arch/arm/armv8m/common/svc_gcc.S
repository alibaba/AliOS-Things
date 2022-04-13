@
@ Copyright (C) 2015-2021 Alibaba Group Holding Limited
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
.file "svc_gcc.S"

.extern syscall_tbl
.extern invalid_svcall_handler
.extern invalid_syscall_hanldler
.extern krhino_utask_cancel_check
.extern usig_sigreturn
.extern usig_do_signal


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

    @ get svc instruction to get svc exception no.
    LDR     R1, [R0, #6*4]
    @ svc instruction is 2 bytes
    SUBS    R1, R1, #2
    LDRB    R1, [R1]

    CMP     R1, #SVC_return_utask
    BEQ     .svc_utask_return

    @ Real syscall
    CMP     R1, #SVC_syscall
    BEQ     .svc_syscall

    STMDB   R0!, {R4-R11}
    BL      invalid_svcall_handler
    B       .err_svc_no

.svc_utask_return:
    @ Set current task mode to user space mode.
    LDR     R1, =g_active_task
    LDR     R1, [R1]
    LDRB    R2, [R1, #RHINO_CONFIG_TASK_MODE_OFFSET]
    ORR     R2, R2, #0x01
    STRB    R2, [R1, #RHINO_CONFIG_TASK_MODE_OFFSET]

    @ Set unprivileged thread mode.
    MRS     R2, control
    ORR     R2, #0x01
    MSR     CONTROL, R2
    ISB

    LDR     R2, [R1, #RHINO_CONFIG_TASK_USTACK_OFFSET]
    PUSH    {R4-R11}

    @ Check float point is used or not.
    TST     LR, #0x10
    IT      EQ

    MOVEQ   R3, R2
    IT      EQ
    SUBEQ   R2, R2, #0x48

    @ Copy the integer context saved by svc exception
    @ from utask.kstack to utask.ustack.
    LDMIA   R0!, {R4-R11}
    STMDB   R2!, {R4-R11}

    @ Copy the float point context saved by svc exception
    @ from utask.kstack to utask.ustack.
    IT      EQ
    SUBEQ   R3, #8
    IT      EQ
    LDREQ   R4, [R0, #0x40]
    IT      EQ
    STREQ   R4, [R3]
    IT      EQ
    VLDMIAEQ  R0!, {D0-D7}
    IT      EQ
    VSTMDBEQ  R3!, {D0-D7}
    IT      EQ
    ADDEQ   R0, #8

    POP     {R4-R11}

    @ Set user space return address on the ustack.
    STR     R4, [R2, #6*4]

    @ Restore utask.kstack to the original address before svc.
    STR     R0, [R1, #RHINO_CONFIG_TASK_KSTACK_OFFSET]

    @ Switch PSP to user space stack.
    MSR     PSP, R2
    ISB

    BX      LR

.svc_syscall:
    @ Save original return address.
    LDR     R4, [R0, #6*4]
    @ Replace the return address with do_syscall.
    LDR     R1, =do_syscall
    STR     R1, [R0, #6*4]

    @ check the return mode, if it's thumb, the last
    @ bit of the return address shouldn't be zero.
    TST     LR, #0x01
    IT      NE
    ORRNE   R4, R4, #0x01

    @ Check current task mode, just return if it's a kernel task.
    LDR     R1, =g_active_task
    LDR     R1, [R1]
    LDRB    R2, [R1, #RHINO_CONFIG_TASK_MODE_OFFSET]
    AND     R2, R2, #0x02
    CMP     R2, #0x02
    BNE     .return

    @ It's a user task.
    @ Clear bit[0], set task mode flag to kernel mode.
    BIC     R2, #0x01
    STRB    R2, [R1, #RHINO_CONFIG_TASK_MODE_OFFSET]

    @ Get kernel stack address
    LDR     R2, [R1, #RHINO_CONFIG_TASK_KSTACK_OFFSET]
    PUSH    {R4-R11}

    @ Save original user space return address again.
    PUSH    {R12}
    MOV     R12, R4

    @ Check float point is used or not.
    TST     LR, #0x10
    IT      EQ

    MOVEQ   R3, R2
    IT      EQ
    SUBEQ   R2, R2, #0x48

    @ Copy the integer context saved by svc exception
    @ from utask.ustack to utask.kstack.
    LDMIA   R0!, {R4-R11}
    STMDB   R2!, {R4-R11}
    @ Restore original user space return address in ustack,
    @ because the return address do_syscall is copied into kstack.
    STR     R12, [R0, #-8]
    POP     {R12}

    @ Copy the float point context saved by svc exception
    @ from utask.ustack to utask.kstack if float point is used.
    IT      EQ
    SUBEQ   R3, #8
    IT      EQ
    LDREQ   R4, [R0, #0x40]
    IT      EQ
    STREQ   R4, [R3]
    IT      EQ
    VLDMIAEQ   R0!, {D0-D7}
    IT      EQ
    VSTMDBEQ   R3!, {D0-D7}
    IT      EQ
    ADDEQ   R0, #8

    POP     {R4-R11}

    @ Store original utask sp before svc exception to task.ustack.
    STR     R0, [R1, #RHINO_CONFIG_TASK_USTACK_OFFSET]

    @ Switch psp to kernel stack to run syscall function.
    MSR     PSP, R2

    @ Return to privileged thread mode.
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
    PUSH    {R2, R3, R5, R6}

    @ Align kernel stack to 8 bytes.
    MRS     R5, PSP
    BIC     R5, R5, #0x7
    MSR     PSP, R5

#if (RHINO_CONFIG_USIGNAL > 0)
    CMP     R0, #SYS_SIGNAL_RETURN
    BNE     .syscall
    PUSH    {R4}
    PUSH    {R0, LR}
    MRS     R0, PSP
    BL      usig_sigreturn
    POP     {R0, LR}
    POP     {R4}
    B       .out_signal
.syscall:
#endif

    @ Find the syscall function and call it.
    LDR     R2, =syscall_tbl
    LDR     R2, [R2, R0, LSL #2]
    CMP     R2, #0
    BEQ     .err_syscall
    MOV     R0, R1  @ R1:syscall args.
    BLX     R2

    MOV     R6, R0
    BL      krhino_utask_cancel_check
    @ Restore kernel stack.
    MSR     PSP, R5
    MOV     R0, R6
#if (RHINO_CONFIG_USIGNAL > 0)
    PUSH    {R4}
    PUSH    {R0, LR}
    MRS     R0, PSP
    BL      usig_do_signal
    POP     {R0, LR}
    POP     {R4}
.out_signal:
#endif

    @ Load task mode to check it's utask or ktask.
    LDR     R2, =g_active_task
    LDR     R2, [R2]
    LDRB    R3, [R2, #RHINO_CONFIG_TASK_MODE_OFFSET]
    AND     R3, R3, #0x02
    CMP     R3, #0x02
    BNE     .ktask_return

    @ Return to user space for utask by svc.
    POP     {R2, R3, R5, R6}
    SVC     #SVC_return_utask
    @ Never reach here.
.err_loop:
    B      .err_loop

.ktask_return:
    @ Just return for ktask.
    POP     {R2, R3, R5, R6}
    BX      R4

.err_syscall:
    @ r0: syscall no, r1: syscall arg pointer r2: syscall pc
    MOV   R2, R4
    BL    invalid_syscall_handler
    B     .err_loop

.size do_syscall, .-do_syscall

@ This will be put on user space stack by signal processing
@ to execute in user space.
.section .rodata
.global sigreturncode
.type sigreturncode, #object
.align
sigreturncode:
    MOV   R0, #SYS_SIGNAL_RETURN
    SVC   #SVC_syscall
    .space 4

.size	sigreturncode, .-sigreturncode
