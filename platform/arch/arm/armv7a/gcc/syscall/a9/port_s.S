/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_config.h>
#include <aux_config.h>

/**********************************************
*   EXTERN SYMBOLS
**********************************************/
.extern g_active_task
.extern g_preferred_ready_task
.extern krhino_stack_ovf_check
.extern sys_stack_top
.extern exceptionHandler
.extern cpu_interrupt_handler

/**********************************************
*   EXPORT SYMBOLS
**********************************************/
.global cpu_intrpt_save
.global cpu_intrpt_restore
.global cpu_task_switch
.global cpu_intrpt_switch
.global cpu_first_task_start

.global _interrupt_handler
.global _panic_handler
.global _interrupt_return_address
.global cpu_get_cpuid

/**********************************************
*   EQUATES
**********************************************/
/* Bits in CPSR (Current Program Status Register) */
.equ CPSR_Mode_USR,     0x10
.equ CPSR_Mode_FIQ,     0x11
.equ CPSR_Mode_IRQ,     0x12
.equ CPSR_Mode_SVC,     0x13
.equ CPSR_Mode_ABT,     0x17
.equ CPSR_Mode_UND,     0x1B
.equ CPSR_Mode_SYS,     0x1F
.equ CPSR_Mode_MASK,    0x1F

/* Disable FIQ. */
.equ CPSR_FIQ_DIS,      0x40
/* Disable IRQ. */
.equ CPSR_IRQ_DIS,      0x80
.equ CPSR_INT_DIS,      CPSR_FIQ_DIS | CPSR_IRQ_DIS
/* Set Thumb mode. */
.equ CPSR_THUMB,        0x20

/**********************************************
 *  CODE GENERATION DIRECTIVES
**********************************************/
.section .text.isr, "ax"
.arm

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


/**********************************************
* Functions:
*     size_t cpu_intrpt_save(void);
*     void cpu_intrpt_restore(size_t cpsr);
***********************************************/
cpu_intrpt_save:
    MRS     R0, CPSR
    CPSID   IF
    /* no need dsb here? */
    DSB
    BX      LR

cpu_intrpt_restore:
    /* no need dsb here? */
    DSB
    MSR     CPSR, R0
    BX      LR

/**********************************************
* Functions:
*     void   cpu_first_task_start(void);
**********************************************/
cpu_first_task_start:
    /* change to SVC mode. */
    MSR     CPSR_c, #(CPSR_INT_DIS | CPSR_Mode_SVC)
    BL      _task_restore

/**********************************************
* Functions:
*     void cpu_task_switch(void);
**********************************************/
cpu_task_switch:
    /* save current task context: */
    PUSH    {LR}             /* Push PC */
    PUSH    {LR}             /* Push LR */
    ADD     LR, SP, #0x08
    PUSH    {LR}             /* Push old SP */
    PUSH    {R0-R12}         /* Push R0-R12 LR */

    /* Note: when @cpu_task_switch is called, the task
     * is running at SVC mode, the next time the task
     * is switched in, it should run in the same mode.
     * so, we save CPSR, rather than SPSR.
     */
    MRS     R0, CPSR             /* Push old task CPSR */
    TST     LR, #1               /* test if called from Thumb mode */
    ORRNE   R0, R0, #CPSR_THUMB  /* if yes, set the T-bit */
    PUSH    {R0}

    #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    /* Push fpu register. */
    PUSH_FP_REG R0
    #endif

    getcoreid   R3
    LSL     R3, R3, #2

    LDR     R1, =g_active_task
    ADD     R1, R1, R3

    /* g_active_task->task_stack = SP; */
    LDR     R1, [R1]
    STR     SP, [R1, #RHINO_CONFIG_TASK_KSTACK_OFFSET]

    #if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
    BL      krhino_stack_ovf_check
    #endif

    #if (RHINO_CONFIG_TASK_SCHED_STATS > 0)
    BL      krhino_task_sched_stats_get
    #endif
    #if (RHINO_CONFIG_CPU_NUM > 1)
    BL      os_unlock_sys_spin
    #endif

    BL      _task_restore

/**********************************************
* Functions:
*     void   cpu_intrpt_switch(void);
**********************************************/
cpu_intrpt_switch:
    PUSH    {FP, LR}

    #if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
    BL      krhino_stack_ovf_check
    #endif

    #if (RHINO_CONFIG_TASK_SCHED_STATS > 0)
    BL      krhino_task_sched_stats_get
    #endif

    getcoreid   R3
    LSL     R3, R3, #2

    /* g_active_task[cpu] = g_preferred_ready_task[cpu] */
    LDR     R0, =g_active_task
    LDR     R1, =g_preferred_ready_task

    ADD     R0, R0, R3
    ADD     R1, R1, R3

    LDR     R2, [R1]
    STR     R2, [R0]

    POP     {FP, PC}

/**********************************************
* _task_restore
* _context_restore
**********************************************/
_task_restore:
    getcoreid   R3
    LSL     R3, R3, #2

    /* g_active_task[cpu] = g_preferred_ready_task[cpu] */
    LDR     R0, =g_active_task
    LDR     R1, =g_preferred_ready_task

    ADD     R0, R0, R3
    ADD     R1, R1, R3

    LDR     R2, [R1]
    STR     R2, [R0]

    LDR     SP, [R2, #RHINO_CONFIG_TASK_KSTACK_OFFSET]

_context_restore:
    #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    /* Pop fpu register. */
    POP_FP_REG R0
    #endif

    /* Pop cpsr of task */
    POP     {R0}
    MSR     SPSR_cxsf, R0

    /* judge which mode should the task running at */
    AND     R0, R0, #CPSR_Mode_MASK
    CMP     R0, #CPSR_Mode_USR
    BNE     1f

    /* user mode */
    MOV     LR, SP
    /* pop {r0-r15} */
    ADD     SP, SP, #0x40
    LDMIA   LR!, {R0-R12}
    LDMIA   LR, {SP, LR}^
    ADD     LR, LR, #0x08
    LDMIA   LR, {PC}^

1:
    /* svc mode */
    MOV     R0, SP
    ADD     SP, SP, #0x40
    LDMFD   R0, {R0-R12, SP, LR, PC}^

/**********************************************
* _interrupt_handler
**********************************************/
/* R0 exc_cause, R1 SPSR, R2 PC, R3 SP of old mode */
_interrupt_handler:
    /* change to SVC mode & disable interruptions. */
    MSR     CPSR_c, #(CPSR_INT_DIS | CPSR_Mode_SVC)
    PUSH    {R2}   /* Push old task PC */
    AND     R2, R1, #CPSR_Mode_MASK
    CMP     R2, #CPSR_Mode_USR
    BNE     1f

    /* user mode */
    SUB     SP, SP, #0x08
    MOV     R2, SP
    STMIA   R2, {SP, LR}^
    B       2f
1:
    /* svc mode */
    ADD     R2, SP, #0x04
    PUSH    {R2, LR}     /* Push SP,LR */
2:
    PUSH    {R4-R12}     /* Push old task R12-R4, */
    LDMFD   R3!, {R5-R8} /* Pop old task R3-R0 from mode stack. */
    PUSH    {R5-R8}      /* Push old task R3-R0, */
    PUSH    {R1}         /* Push task SPSR. */

    #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    /* Push task fpu register. */
    PUSH_FP_REG R1
    #endif

    /* if (g_sys_stat == RHINO_RUNNING) */
    LDR     R3, =g_sys_stat
    LDR     R4, [R3]
    CMP     R4, #3  /* RHINO_RUNNING = 3 */
    BNE     _interrupt_while_init

_interrupt_while_task:
    /* g_active_task->task_stack = context region */
    LDR     R3, =g_active_task

    getcoreid   R5
    LSL     R5,R5,#2
    ADD     R3, R3, R5

    LDR     R4, [R3]
    STR     SP, [R4, #RHINO_CONFIG_TASK_KSTACK_OFFSET]

    /* Switch to system stack. */
    LDR     R3, =sys_stack_top
    MOV     R4, #RHINO_EACHCORE_SYSTEM_STACK_SIZE
    MUL     R4, R4, R5
    SUB     R3, R3, R4

    MOV     SP, R3

    /* cpu_interrupt_handler(except_type = R0) */
    BL      cpu_interrupt_handler
_interrupt_return_address:

    /* SP = g_active_task->task_stack */
    LDR     R3, =g_active_task
    getcoreid   R4
    LSL     R4, R4, #2
    ADD     R3, R3, R4

    LDR     R4, [R3]
    LDR     SP, [R4, #RHINO_CONFIG_TASK_KSTACK_OFFSET]

    BL      _context_restore

_interrupt_while_init:
    /* align SP to 8 byte. */
    MOV     R1, SP
    AND     R1, R1, #4
    SUB     SP, SP, R1
    PUSH    {R1, LR}

    /* cpu_interrupt_handler(except_type = R0) */
    BL      cpu_interrupt_handler
    POP     {R1, LR}
    ADD     SP, SP, R1

    BL      _context_restore

_panic_handler:
    /* change to SVC mode & disable interruptions. */
    MSR     CPSR_c, #(CPSR_INT_DIS | CPSR_Mode_SVC)

    PUSH    {R2}          /* Push old task PC, */
    ADD     R2, SP, #4
    PUSH    {LR}          /* Push old task LR, */
    ADD     LR, LR, #0x08
    PUSH    {LR}          /* Push old SP */
    PUSH    {R4-R12}      /* Push old task R12-R4, */
    LDMFD   R3!, {R5-R8}  /* Pop old task R3-R0 from mode stack. */
    PUSH    {R5-R8}       /* Push old task R3-R0, */
    PUSH    {R1}          /* Push task CPSR. */

    #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    PUSH_FP_REG R1        /* Push task fpu register. */
    #endif

    PUSH    {R0, R2}      /* Push SP and exc_type */

    /* align SP to 8 byte. */
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

  /* int cpu_get_cpuid(void)@ */
  /* get current CPU ID */
cpu_get_cpuid:
    mrc   p15, 0, r0, c0, c0, 5
    and   r0, r0, #3
    BX    lr

