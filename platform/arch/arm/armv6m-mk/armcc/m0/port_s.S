;
; Copyright (C) 2015-2019 Alibaba Group Holding Limited
;

#include <k_config.h>
#include <aux_config.h>

;******************************************************************************
;                            EXTERN PARAMETERS
;******************************************************************************
    EXTERN g_active_task
    EXTERN g_preferred_ready_task
    EXTERN krhino_stack_ovf_check
    EXTERN krhino_task_sched_stats_get
    EXTERN k_proc_switch

;******************************************************************************
;                            EXPORT FUNCTIONS
;******************************************************************************
    EXPORT cpu_intrpt_save
    EXPORT cpu_intrpt_restore
    EXPORT cpu_task_switch
    EXPORT cpu_intrpt_switch
    EXPORT cpu_first_task_start
    EXPORT _first_task_restore
    EXPORT PendSV_Handler

;******************************************************************************
;                                 EQUATES
;******************************************************************************
SCB_ICSR              EQU      0xE000ED04 ; Interrupt Control and State Register.
SCB_VTOR              EQU      0xE000ED08 ; Vector Table Offset Register.
ICSR_PENDSVSET        EQU      0x10000000 ; Value to trigger PendSV exception.

SHPR2_PRI_11          EQU      0xE000ED1F ; System Handler Priority Register 2 (SVC)
PRI_LVL_SVC           EQU      0xC0       ;SVC priority level (lowest)

SHPR3_PRI_14          EQU      0xE000ED22 ; System Handler Priority Register 3 (PendSV + SysTick).
PRI_LVL_PENDSV        EQU      0xC0

SHPR3_PRI_15          EQU      0xE000ED23 ; System Handler Priority Register 3 (PendSV + SysTick).
PRI_LVL_SYSTICK       EQU      0xC0 ; PendSV + SysTick priority level (lowest).

;******************************************************************************
;                        CODE GENERATION DIRECTIVES
;******************************************************************************
    AREA |.text|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8

;******************************************************************************
; Functions:
;     size_t cpu_intrpt_save(void);
;     void cpu_intrpt_restore(size_t cpsr);
;******************************************************************************
cpu_intrpt_save
    MRS     R0, PRIMASK
    CPSID   I
    BX      LR

cpu_intrpt_restore
    MSR     PRIMASK, R0
    BX      LR

;******************************************************************************
; Functions:
;     void cpu_intrpt_switch(void);
;     void cpu_task_switch(void);
;******************************************************************************
cpu_task_switch
    LDR     R0, =SCB_ICSR
    LDR     R1, =ICSR_PENDSVSET
    STR     R1, [R0]
    BX      LR

cpu_intrpt_switch
    LDR     R0, =SCB_ICSR
    LDR     R1, =ICSR_PENDSVSET
    STR     R1, [R0]
    BX      LR

;******************************************************************************
; Functions:
;     void cpu_first_task_start(void);
;******************************************************************************
cpu_first_task_start
    ;set SVC priority to the lowest level
    LDR     R0, =SHPR2_PRI_11
    LDR     R1, =PRI_LVL_SVC
    STRB    R1, [R0]

    ;set PendSV priority to the lowest level
    LDR     R0, =SHPR3_PRI_14
    LDR     R1, =PRI_LVL_PENDSV
    STRB    R1, [R0]

    ;set Systick priority to te lowest level
    LDR     R0, =SHPR3_PRI_15
    LDR     R1, =PRI_LVL_SYSTICK
    STRB    R1, [R0]

    ;indicate PendSV_Handler branch to _pendsv_handler_nosave
    MOVS    R0, #0
    MSR     PSP, R0

    ;make PendSV exception pending
    LDR     R0, =SCB_ICSR
    LDR     R1, =ICSR_PENDSVSET
    STR     R1, [R0]

    ;goto PendSV_Handler
    CPSIE   I
    B       .

;******************************************************************************
; Functions:
;     void krhino_pendsv_handler(void);
;******************************************************************************
PendSV_Handler
    CPSID   I
    MRS     R0, PSP
    ;branch if cpu_first_task_start
    CMP     R0, #0
    BEQ     _first_task_restore

    ;hardware saved R0~R3,R12,LR,PC,xPSR

    ;save context
    SUBS    R0, R0, #0x24
    STMIA   R0!, {R4 - R7}
    MOV     R4, R8
    MOV     R5, R9
    MOV     R6, R10
    MOV     R7, R11
    STMIA   R0!, {R4 - R7}
    MOV     R4, LR
    STMIA   R0!, {R4}

    ;g_active_task->task_stack = context region
    SUBS    R0, R0, #0x24
    LDR     R1, =g_active_task
    LDR     R1, [R1]
    STR     R0, [R1]

    MOVS    R3, #0x01
    LDRB    R2, [R1, #RHINO_CONFIG_TASK_MODE_OFFSET]
    TST     R2, R3
    BNE     store_ustack
    STR     R0, [R1, #RHINO_CONFIG_TASK_KSTACK_OFFSET]
    B       store_stack_done
store_ustack
    STR    R0, [R1, #RHINO_CONFIG_TASK_USTACK_OFFSET]
store_stack_done

    ;save and restore LR
#if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
    BL      krhino_stack_ovf_check
#endif
#if (RHINO_CONFIG_SYS_STATS > 0)
    BL      krhino_task_sched_stats_get
#endif

_pendsv_handler_nosave
    LDR     R0, =g_active_task
    LDR     R1, =g_preferred_ready_task
    LDR     R2, [R1] ; R2 -> new_task
    LDR     R3, [R0] ; R3 -> old_task
    STR     R2, [R0]

    MOV     R4, R2
    MOV     R0, R2
    MOV     R1, R3
    BL      k_proc_switch
    MOV     R2, R4

    ; judge task mode, if it's unprivileged task,
    ; 1. change mode to unprivileged mode
    ; 2. use user stack, else use kernel stack
    LDRB    R0, [R2, #RHINO_CONFIG_TASK_MODE_OFFSET]
    MOVS    R3, #0x01
    TST     R0, R3
    BEQ     privileged_mode

    MRS     R1, CONTROL
    ORRS    R1, R3
    MSR     CONTROL, R1
    ISB
    LDR     R0, [R2, #RHINO_CONFIG_TASK_USTACK_OFFSET]
    B       restore_context

privileged_mode
    MRS     R1, CONTROL
    BICS    R1, R3
    MSR     CONTROL, R1
    ISB
    LDR     R0, [R2, #RHINO_CONFIG_TASK_KSTACK_OFFSET]

restore_context
    ;restore context
    LDMIA   R0!, {R4 - R7}
    LDMIA   R0!, {R2 - R3}
    MOV     R8,  R2
    MOV     R9,  R3
    LDMIA   R0!, {R2 - R3}
    MOV     R10,  R2
    MOV     R11,  R3
    LDMIA   R0!,  {R2}
    MOV     LR,  R2

    ;return stack = PSP
    MSR     PSP, R0

    CPSIE   I
    ;hardware restore R0~R3,R12,LR,PC,xPSR
    BX      LR

_first_task_restore
    ;set MSP to the base of system stack
    MRS     R0, MSP
    LSRS    R0, R0, #3
    LSLS    R0, R0, #3
    MSR     MSP, R0

    B       _pendsv_handler_nosave

    ALIGN
    END

