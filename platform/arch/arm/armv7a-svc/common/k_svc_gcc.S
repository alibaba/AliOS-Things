@
@ Copyright (C) 2015-2017 Alibaba Group Holding Limited
@

#include <k_config.h>
#include <k_default_config.h>
#include <aux_config.h>
#include <svc.h>
#include <syscall_no.h>

@
@  SVC exception description:
@
@  --------------------------------------------
@  |  no  | description                       |
@  --------------------------------------------
@  | 0x03 | syscall                           |
@  --------------------------------------------
@  |others| reserved                          |
@  --------------------------------------------
@
@  svc no is defined in svc.h
@

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
#define INT_DIS         FIQ_DIS | IRQ_DIS

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

@******************************************************************************
@                            EXTERN SYMBOLS
@******************************************************************************
.extern syscall_tbl
.extern syscall_pre_debug
.extern syscall_post_debug
.extern exceptionHandler
.extern _context_restore

@******************************************************************************
@                            EXPORT SYMBOLS
@******************************************************************************
.global SVC_Handler

@******************************************************************************
@                        MACRO DEFINED
@******************************************************************************
@ Cortex-A9, ARMv7 VFPv3-D16
.macro POP_FP_REG reg
    POP     {\reg}
    VMSR    FPEXC, \reg                      @ Pop FPEXC.
    POP     {\reg}
    VMSR    FPSCR, \reg                      @ Pop FPSCR.
#ifdef __ARM_NEON
    VPOP    {Q0-Q7}
    VPOP    {Q8-Q15}
#else
    VPOP    {D0-D15}
#endif
.endm

.macro PUSH_FP_REG reg
#ifdef __ARM_NEON
    VPUSH   {Q8-Q15}
    VPUSH   {Q0-Q7}
#else
    VPUSH   {D0-D15}
#endif
    VMRS    \reg, FPSCR                      @ Save FPSCR.
    PUSH    {\reg}                           @ Save floating-point registers.
    VMRS    \reg, FPEXC                      @ Save FPEXC.
    PUSH    {\reg}
.endm

@******************************************************************************
@                        CODE GENERATION DIRECTIVES
@******************************************************************************
.section .text.isr, "ax"
.arm

@******************************************************************************
@ Functions:
@       void SVC_Handler(void);
@******************************************************************************
SVC_Handler:
    @ which stack are we running on???
    @ we are using the tasks kstack which was set
    @ when task are scheded in.
    @

    @ {spsr, R0 ... R12, SP, LR, PC}
    SUB     SP, SP, #(REG_SIZE - 4)
    STMIA   SP, {R0-R12}
    MRS     R2, SPSR
    PUSH    {R2}
    @ {SPSR, R0 ... R12} is done

    MOV     R4, SP
    ADD     R1, R4, #REG_SP
    AND     R2, R2, #MODE_MASK
    CMP     R2, #USER_MODE
    BNE     .svc_enter_svc

    @ come from user mode
    @ save user mode SP and LR
    STMIA   R1, {SP, LR}^
    ADD     R1, R1, #8
    B       .store_pc

.svc_enter_svc:
    @ save svc mode SP and LR
    ADD     R2, R4, #REG_SIZE
    STMIA   R1!, {R2, LR}

.store_pc:
    @ stroe LR as PC
    STMIA   R1!, {LR}

#if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    @ Push task fpu register.
    PUSH_FP_REG R1
#endif

    @ align stack to 8 bytes
    MOV     R5, SP
    BIC     SP, SP, #0x07

#if (CONFIG_SYSCALL_DEBUG > 0)
    @ syscall pre debug
    MOV     R0, R5
    BL      syscall_pre_debug
#endif

    @ get svc intruction
    LDR     R1, [R4, #REG_PC]
    @ judge arm or thumb2 instruction
    LDR     R2, [R4, #REG_SPSR]
    TST     R2, #0x20
    SUBEQ   R1, R1, #4
    SUBNE   R1, R1, #2
    LDRHEQ  R1, [R1]
    LDRBNE  R1, [R1]
    CMP     R1, #SVC_syscall
    BNE     .svc_exception

.svc_syscall:
    @ enable interrput
    MRS     R1, CPSR
    BIC     R1, #IRQ_DIS
    MSR     CPSR_c, R1

    @ prepare args for _do_syscall
    LDR     R0, [R4, #REG_R0]
    LDR     R1, [R4, #REG_R1]
    BL      _do_syscall
    @ save syscall return data
    STR     R0, [R4, #REG_R0]
    STR     R1, [R4, #REG_R1]

    @ disable interrupt
    MRS     R1, CPSR
    ORR     R1, R1, #IRQ_DIS
    MSR     CPSR_c, R1

    @ restore SP
    MOV     SP, R5

#if (CONFIG_SYSCALL_DEBUG> 0)
    @ syscall post debug
    MOV     R0, SP
    BL      syscall_post_debug
#endif

#if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    @ Pop task fpu register.
    POP_FP_REG R1
#endif

    POP     {R0}
    MSR     SPSR, R0

    AND     R0, R0, #MODE_MASK
    CMP     R0, #USER_MODE
    BNE     .ret_to_svc

    @ return to user mode
    MOV     LR, SP
    ADD     SP, SP, #(REG_SIZE - 4)
    LDMIA   LR!, {R0-R12}
    LDMIA   LR, {SP, LR}^
    ADD     LR, LR, #0x08
    LDMIA   LR, {PC}^

.ret_to_svc:
    @ don't worry SP is not poped properly,
    @ new SP will be restored to svc mode
    LDMIA   SP, {R0-R15}^

@******************************************************************************
@ Functions:
@       static unsigned long long _do_syscall(int syscall_no, void *arg);
@******************************************************************************
.text
.type _do_syscall, function
_do_syscall:
    PUSH    {IP, LR}
    CMP     R0, #MAX_SYSCALL_NO
    BGT     .bad_syscall
    LDR     IP, =syscall_tbl
    LDR     IP, [IP, R0, LSL #2]
    MOV     R0, R1
    CMP     IP, #0x0
    BEQ     .bad_syscall
    BLX     IP
    POP     {IP, PC}

.bad_syscall:
    @ disable interrupt
    MRS     R1, CPSR
    ORR     R1, R1, #IRQ_DIS
    MSR     CPSR_c, R1
    ADD     SP, #0x08

.svc_exception:
    LDR     R2, [R4, #REG_SP]
    @ set SWI exception
    MOV     R0, #0x02
    PUSH    {R0, R2}
    MOV     R0, SP

    BL      exceptionHandler
    MOV     SP, R5
    BL      _context_restore

