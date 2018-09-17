#include <k_config.h>

@******************************************************************************
@                            EXTERN SYMBOLS
@******************************************************************************
.extern g_active_task
.extern g_preferred_ready_task
.extern krhino_stack_ovf_check
.extern sys_stack_top
.extern exceptionHandler
.extern cpu_interrupt_handler

@******************************************************************************
@                            EXPORT SYMBOLS
@******************************************************************************
.global cpu_intrpt_save
.global cpu_intrpt_restore
.global cpu_task_switch
.global cpu_intrpt_switch
.global cpu_first_task_start

.global _interrupt_handler
.global _panic_handler
.global _interrupt_return_address

@******************************************************************************
@                                 EQUATES
@******************************************************************************
@ Bits in CPSR (Current Program Status Register)
.equ CPSR_Mode_USR,             0x10
.equ CPSR_Mode_FIQ,             0x11
.equ CPSR_Mode_IRQ,             0x12
.equ CPSR_Mode_SVC,             0x13
.equ CPSR_Mode_ABT,             0x17
.equ CPSR_Mode_UND,             0x1B
.equ CPSR_Mode_SYS,             0x1F

.equ CPSR_FIQ_DIS,              0x40                    @ Disable FIQ.
.equ CPSR_IRQ_DIS,              0x80                    @ Disable IRQ.
.equ CPSR_INT_DIS,              CPSR_FIQ_DIS | CPSR_IRQ_DIS
.equ CPSR_THUMB,                0x20                    @ Set Thumb mode.

@******************************************************************************
@                        CODE GENERATION DIRECTIVES
@******************************************************************************
.section .text.isr, "ax"
.arm

@******************************************************************************
@                        MACRO DEFINED
@******************************************************************************
@ ortex-A5, ARMv7 VFPv4-D16
.macro POP_FP_REG reg
    POP     {\reg}
    VMSR    FPEXC, \reg                                 @ Pop FPEXC.
    VPOP    {D0-D15}
    POP     {\reg}
    VMSR    FPSCR, \reg                                 @ Pop FPSCR.
.endm

.macro PUSH_FP_REG reg
    VMRS    \reg, FPSCR                                 @ Save FPSCR.
    PUSH    {\reg}                                      @ Save floating-point registers.
    VPUSH   {D0-D15}
    VMRS    \reg, FPEXC                                 @ Save FPEXC.
    PUSH    {\reg}
.endm

@******************************************************************************
@ Functions:
@     size_t cpu_intrpt_save(void);
@     void cpu_intrpt_restore(size_t cpsr);
@******************************************************************************
cpu_intrpt_save:
    MRS     R0, CPSR
    CPSID   IF
    DSB                                                 @ no need dsb here?
    BX      LR

cpu_intrpt_restore:
    DSB                                                 @ no need dsb here?
    MSR     CPSR, R0
    BX      LR

@******************************************************************************
@ Functions:
@     void   cpu_first_task_start(void);
@******************************************************************************
cpu_first_task_start:
    MSR     CPSR_c, #(CPSR_INT_DIS | CPSR_Mode_SVC)     @ change to SVC mode.
    BL      _task_restore

@******************************************************************************
@ Functions:
@     void cpu_task_switch(void);
@******************************************************************************
cpu_task_switch:
    @ save current task context:
    PUSH    {LR}                                        @ Push PC.
    PUSH    {R0-R12, LR}                                @ Push R0-R12 LR
    MRS     R0, CPSR                                    @ Push old task CPSR
    TST     LR, #1                                      @ test if called from Thumb mode,
    ORRNE   R0, R0, #CPSR_THUMB                         @ if yes, set the T-bit.
    PUSH    {R0}

    #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    PUSH_FP_REG R0                                      @ Push fpu register.
    #endif

    @g_active_task->task_stack = context region
    LDR     R1, =g_active_task                          @ g_active_task->task_stack = SP;
    LDR     R1, [R1]
    STR     SP, [R1]

    #if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
    BL      krhino_stack_ovf_check
    #endif

    #if (RHINO_CONFIG_TASK_SCHED_STATS > 0)
    BL      krhino_task_sched_stats_get
    #endif

    BL      _task_restore

@******************************************************************************
@ Functions:
@     void   cpu_intrpt_switch(void);
@******************************************************************************
cpu_intrpt_switch:
    PUSH    {FP, LR}

    #if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
    BL      krhino_stack_ovf_check
    #endif

    #if (RHINO_CONFIG_TASK_SCHED_STATS > 0)
    BL      krhino_task_sched_stats_get
    #endif

    LDR     R0, =g_active_task                          @ g_active_task = g_preferred_ready_task;
    LDR     R1, =g_preferred_ready_task
    LDR     R2, [R1]
    STR     R2, [R0]

    POP     {FP, LR}

@******************************************************************************
@ _task_restore
@ _context_restore
@******************************************************************************
_task_restore:
    LDR     R0, =g_active_task                          @ g_active_task = g_preferred_ready_task;
    LDR     R1, =g_preferred_ready_task
    LDR     R2, [R1]
    STR     R2, [R0]                                    @ R0 = g_active_task->task_stack = context region

    LDR     SP, [R2]

_context_restore:
    #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    POP_FP_REG R0                                       @ Pop fpu register.
    #endif

    POP     {R0}                                        @ Pop cpsr of task
    MSR     SPSR_cxsf, R0
    LDMFD   SP!, {R0-R12, LR, PC}^                      @ switch to task

@******************************************************************************
@ _interrupt_handler
@******************************************************************************
@ R0 exc_cause, R1 SPSR, R2 PC, R3 SP of old mode
_interrupt_handler:
    @ change to SVC mode & disable interruptions.
    MSR     CPSR_c, #(CPSR_INT_DIS | CPSR_Mode_SVC)

    PUSH    {R2}                                        @ Push old task PC,
    PUSH    {LR}                                        @ Push old task LR,
    PUSH    {R4-R12}                                    @ Push old task R12-R4,
    LDMFD   R3!, {R5-R8}                                @ Pop old task R3-R0 from mode stack.
    PUSH    {R5-R8}                                     @ Push old task R3-R0,
    PUSH    {R1}                                        @ Push task CPSR.

    #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    PUSH_FP_REG R1                                      @ Push task fpu register.
    #endif

    LDR     R3, =g_sys_stat                             @ if (g_sys_stat == RHINO_RUNNING)
    LDR     R4, [R3]
    CMP     R4, #3                                      @ RHINO_RUNNING = 3
    BNE     _interrupt_while_init

_interrupt_while_task:
    LDR     R3, =g_active_task                          @ g_active_task->task_stack = SP;
    LDR     R4, [R3]
    STR     SP, [R4]

    LDR     R3, =sys_stack_top                          @ Switch to system stack.
    MOV     SP, R3

    BL      cpu_interrupt_handler                       @ cpu_interrupt_handler(except_type = R0)
_interrupt_return_address:
    LDR     R3, =g_active_task                          @ SP = g_active_task->task_stack;
    LDR     R4, [R3]
    LDR     SP, [R4]

    BL      _context_restore

_interrupt_while_init:
    @ align SP to 8 byte.
    MOV     R1, SP
    AND     R1, R1, #4
    SUB     SP, SP, R1
    PUSH    {R1, LR}

    BL      cpu_interrupt_handler                       @ cpu_interrupt_handler(except_type = R0)
    POP     {R1, LR}
    ADD     SP, SP, R1

    BL      _context_restore
 
_panic_handler:
    @ change to SVC mode & disable interruptions.
    MSR     CPSR_c, #(CPSR_INT_DIS | CPSR_Mode_SVC)

    PUSH    {R2}                                        @ Push old task PC,
    ADD     R2, SP, #4
    PUSH    {LR}                                        @ Push old task LR,
    PUSH    {R4-R12}                                    @ Push old task R12-R4,
    LDMFD   R3!, {R5-R8}                                @ Pop old task R3-R0 from mode stack.
    PUSH    {R5-R8}                                     @ Push old task R3-R0,
    PUSH    {R1}                                        @ Push task CPSR.

    #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    PUSH_FP_REG R1                                      @ Push task fpu register.
    #endif

    PUSH    {R0, R2}                                    @ Push SP and exc_type

    @ align SP to 8 byte.
    MOV     R0, SP
    MOV     R1, SP
    AND     R1, R1, #4
    SUB     SP, SP, R1
    PUSH    {R1, LR}

    BL      exceptionHandler

    POP     {R1, LR}
    ADD     SP, SP, R1
    POP     {R0, R2}

    BL      _context_restore

