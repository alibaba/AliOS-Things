#include <k_config.h>

;******************************************************************************
;                            EXTERN PARAMETERS
;******************************************************************************
    EXTERN g_active_task
    EXTERN g_preferred_ready_task
    EXTERN krhino_stack_ovf_check
    EXTERN krhino_task_sched_stats_get

;******************************************************************************
;                            EXPORT FUNCTIONS
;******************************************************************************
    EXPORT cpu_intrpt_save
    EXPORT cpu_intrpt_restore
    EXPORT cpu_task_switch
    EXPORT cpu_intrpt_switch
    EXPORT cpu_first_task_start

;******************************************************************************
;                        CODE GENERATION DIRECTIVES
;******************************************************************************
    PRESERVE8
    CODE32
    AREA |.text|, CODE, READONLY

    GLOBAL cpu_intrpt_save
cpu_intrpt_save
    MRS     R0, CPSR                ; Set IRQ and FIQ bits in CPSR to disable all interrupts
    ORR     R1, R0, #0xC0
    MSR     CPSR_c, R1
    MRS     R1, CPSR                ; Confirm that CPSR contains the proper interrupt disable flags
    AND     R1, R1, #0xC0
    CMP     R1, #0xC0
    BNE     cpu_intrpt_save         ; Not properly disabled (try again)
    BX      LR                      ; Disabled, return the original CPSR contents in R0

    GLOBAL cpu_intrpt_restore
cpu_intrpt_restore
    MSR     CPSR_c, R0
    BX      LR

cpu_first_task_start
    LDR     R0, =g_active_task
    LDR     R0, [R0]
    LDR     SP, [R0]

    LDMFD   SP!, {R0}
    LDR     R1, [SP, #56]
    TST     R1, #1
    ORRNE   R0, #32                 ; if PC is thumb mode, set SPSR to thumb
    MSR     SPSR_cxsf, R0
    LDMFD   SP!, {R0-R12, LR, PC}^

    GLOBAL cpu_task_switch
cpu_task_switch
    STMFD   SP!, {LR}
    STMFD   SP!, {R0-R12,LR}
    MRS     R0, CPSR
    STMFD   SP!, {R0}               ; push current cpsr

    ; g_active_task->task_stack = SP
    LDR     R0, =g_active_task
    LDR     R0, [R0]
    STR     SP, [R0]

#if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
    BL      krhino_stack_ovf_check
#endif
#if (RHINO_CONFIG_SYS_STATS > 0)
    BL      krhino_task_sched_stats_get
#endif

    GLOBAL cpu_intrpt_switch
cpu_intrpt_switch
    LDR     R0, =g_preferred_ready_task
    LDR     R1, =g_active_task
    LDR     R0, [R0]
    STR     R0, [R1]

    LDR     R0, =g_active_task
    LDR     R0, [R0]
    LDR     SP, [R0]

    ; Restore New Task context
    LDMFD   SP!, {R0}
    LDR     R1, [SP, #56]
    TST     R1, #1
    ORRNE   R0, #32          ; if PC is thumb mode, set SPSR to thumb
    MSR     SPSR_cxsf, R0
    LDMFD   SP!, {R0-R12, LR, PC}^

    END

