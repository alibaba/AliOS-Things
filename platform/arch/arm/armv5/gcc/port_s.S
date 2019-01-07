#include <k_config.h>

@******************************************************************************
@                            EXTERN PARAMETERS
@******************************************************************************
.extern g_active_task
.extern g_preferred_ready_task
.extern krhino_stack_ovf_check
.extern krhino_task_sched_stats_get

@******************************************************************************
@                            EXPORT FUNCTIONS
@******************************************************************************
.global cpu_intrpt_save
.global cpu_intrpt_restore
.global cpu_task_switch
.global cpu_intrpt_switch
.global cpu_first_task_start

@******************************************************************************
@                        CODE GENERATION DIRECTIVES
@******************************************************************************
.text
.align 2

.type cpu_intrpt_save, %function
cpu_intrpt_save:
    MRS     R0, CPSR                @ Set IRQ and FIQ bits in CPSR to disable all interrupts
    ORR     R1, R0, #0xC0
    MSR     CPSR_c, R1
    MRS     R1, CPSR                @ Confirm that CPSR contains the proper interrupt disable flags
    AND     R1, R1, #0xC0
    CMP     R1, #0xC0
    BNE     cpu_intrpt_save         @ Not properly disabled (try again)
    BX      LR                      @ Disabled, return the original CPSR contents in R0

.type cpu_intrpt_restore, %function
cpu_intrpt_restore:
    MSR     CPSR_c, R0
    BX      LR

.type cpu_first_task_start, %function
cpu_first_task_start:
    LDR     R0, =g_active_task
    LDR     R0, [R0]
    LDR     SP, [R0]

    LDMFD   SP!, {R0}
    LDR     R1, [SP, #56]
    TST     R1, #1
    ORRNE   R0, #32                 @ if PC is thumb mode, set SPSR to thumb
    MSR     SPSR_cxsf, R0
    LDMFD   SP!, {R0-R12, LR, PC}^

.type cpu_task_switch, %function
cpu_task_switch:
    STMFD   SP!, {LR}
    STMFD   SP!, {R0-R12,LR}
    MRS     R0, CPSR
    STMFD   SP!, {R0}               @ push current cpsr

    @ g_active_task->task_stack = SP
    LDR     R0, =g_active_task
    LDR     R0, [R0]
    STR     SP, [R0]

#if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
    BL      krhino_stack_ovf_check
#endif
#if (RHINO_CONFIG_SYS_STATS > 0)
    BL      krhino_task_sched_stats_get
#endif

.type cpu_intrpt_switch, %function
cpu_intrpt_switch:
    LDR     R0, =g_preferred_ready_task
    LDR     R1, =g_active_task
    LDR     R0, [R0]
    STR     R0, [R1]

    LDR     R0, =g_active_task
    LDR     R0, [R0]
    LDR     SP, [R0]

    @ Restore New Task context
    LDMFD   SP!, {R0}
    LDR     R1, [SP, #56]
    TST     R1, #1
    ORRNE   R0, #32                 @ if PC is thumb mode, set SPSR to thumb
    MSR     SPSR_cxsf, R0
    LDMFD   SP!, {R0-R12, LR, PC}^

    .end

