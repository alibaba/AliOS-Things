/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_config.h>
#include <k_default_config.h>
#include <aux_config.h>
#include <svc.h>

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

#define USER_MODE       0x10
#define FIQ_MODE        0x11
#define IRQ_MODE        0x12
#define SVC_MODE        0x13
#define ABT_MODE        0x17
#define UND_MODE        0x1b
#define SYS_MODE        0x1f
#define MODE_MASK       0x1f

#define FIQ_DIS         0x40
#define IRQ_DIS         0x80
#deinfe INT_DIS         FIQ_DIS | IRQ_DIS

#define REG_SPSR        0x00
#define REG_R0          0x04
#define REG_R1          0x08
#define REG_R2          0x0c
#define REG_R3          0x10
#define REG_R4          0x14
#define REG_R5          0x18
#define REG_R6          0x1c
#define REG_R7          0x20
#define REG_R8          0x24
#define REG_R9          0x28
#define REG_R10         0x2c
#define REG_R11         0x30
#define REG_R12         0x34
#define REG_SP          0x38
#define REG_LR          0x3c
#define REG_PC          0x40

#define REG_SIZE        0x44

.syntax unified
.file "svc.S"

/**********************************************
*                        MACRO DEFINED
**********************************************/
/* ortex-A9, ARMv7 VFPv4-D16 */
.macro POP_FP_REG reg
    POP     {\reg}
    /* Pop FPEXC. */
    VMSR    FPEXC, \reg
    VPOP    {D0-D15}
    POP     {\reg}
    /* Pop FPSCR. */
    VMSR    FPSCR, \reg
.endm

.macro PUSH_FP_REG reg
    /* Save FPSCR. */
    VMRS    \reg, FPSCR
    /* Save floating-point registers. */
    PUSH    {\reg}
    VPUSH   {D0-D15}
    /* Save FPEXC. */
    VMRS    \reg, FPEXC
    PUSH    {\reg}
.endm

.macro getcoreid reg
    mrc   p15, 0, \reg, c0, c0, 5
    and   \reg, \reg, #3
.endm

.extern syscall_tbl

.text
.global SVC_Handler
.type SVC_Handler, function
SVC_Handler:
    /* which stack are we running on???
     * we are using the task's kstack which was set
     * when task are scheded in.
     */

    /* {spsr, r0 ... r12, sp, lr, pc} */
    sub     sp, sp, #(REG_SIZE - REG_R0)
    stmia   sp, {r0-r12}
    mrs     r2, spsr
    push    {r2}
    /*       {spsr, r0 ... r12} is done */

    mov     r4, sp
    add     r1, sp, #REG_SP
    and     r2, r2, #MODE_MASK
    cmp     r2, #USER_MODE
    bne     .svc_enter_svc

    /* come from user mode */
    /* save user mode sp and lr */
    stmia   r1!, {sp, lr}^
    b       .store_pc

.svc_enter_svc:
    /* save svc mode sp and lr */
    add     r2, r4, #REG_SIZE
    stmia   r1!, {r2, lr}

.store_pc:
    /* stroe lr as pc */
    stmia   r1!, {lr}

#if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    /* Push task fpu register. */
    PUSH_FP_REG r1
#endif

    /* get svc intruction */
    ldr     r1, [r4, #REG_PC]
    sub     r1, r1, #4
    ldrh    r1, [r1]
    cmp     r1, #SVC_syscall
    beq     .svc_syscall

.bad_svc_no:
    b       .bad_svc_no

.svc_syscall:
    /* enable interrput */
    mrs     r1, cpsr
    bic     r1, #IRQ_DIS
    msr     cpsr_c, r1

    ldr     r0, [r4, #REG_R0]
    ldr     r1, [r4, #REG_R1]
    /* align the stack to 8 bytes */
    mov     r5, sp
    push    {r5}
    bic     sp, sp, #0x07
    bl      do_syscall
    pop     {r5}
    mov     sp, r5
    str     r0, [r4, #REG_R0]
    str     r1, [r4, #REG_R1]

#if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    /* Pop task fpu register. */
    POP_FP_REG r1
#endif

    pop     {r0}
    msr     spsr, r0

    and     r0, r0, #MODE_MASK
    cmp     r0, #USER_MODE
    bne     .ret_to_svc

    /* return to user mode */
    /* need disable interrupt??? */
    mrs     r1, cpsr
    orr     r1, r1, #IRQ_DIS
    msr     cpsr_c, r1

    mov     lr, sp
    add     sp, sp, #0x40
    ldmia   lr!, {r0-r12}
    ldmia   lr, {sp, lr}^
    add     lr, lr, #0x08
    ldmia   lr, {pc}^

.ret_to_svc:
    /* don't worry sp is not poped properly,
     * new sp will be restored to svc mode
     */
    ldmia   sp, {r0-r15}^

.size SVC_Handler, .-SVC_Handler

.text
.type do_syscall, function
do_syscall:
    push    {ip, lr}
    ldr     ip, =syscall_tbl
    ldr     ip, [ip, r0, lsl #2]
    mov     r0, r1
    blx     ip

    pop     {ip, pc}

.size do_syscall, .-do_syscall

