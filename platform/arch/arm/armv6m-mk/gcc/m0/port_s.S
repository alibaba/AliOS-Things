@
@ Copyright (C) 2015-2019 Alibaba Group Holding Limited
@

#include <k_config.h>
#include <aux_config.h>

@******************************************************************************
@                            EXTERN PARAMETERS
@******************************************************************************
.extern g_active_task
.extern g_preferred_ready_task
.extern krhino_stack_ovf_check
.extern krhino_task_sched_stats_get
.extern k_proc_switch

@******************************************************************************
@                            EXPORT FUNCTIONS
@******************************************************************************
.global cpu_intrpt_save
.global cpu_intrpt_restore
.global cpu_task_switch
.global cpu_intrpt_switch
.global cpu_first_task_start

.global PendSV_Handler

@******************************************************************************
@                                 EQUATES
@******************************************************************************
.equ SCB_ICSR,        0xE000ED04 @ Interrupt Control and State Register.
.equ SCB_VTOR,        0xE000ED08 @ Vector Table Offset Register.
.equ ICSR_PENDSVSET,  0x10000000 @ Value to trigger PendSV exception.

.equ SHPR2_PRI_11,    0xE000ED1F @ System Handler Priority Register 2 (SVC)
.equ PRI_LVL_SVC,     0xC0       @ SVC priority level (lowest)
.equ SHPR3_PRI_14,    0xE000ED22 @ System Handler Priority Register 3 (PendSV)
.equ PRI_LVL_PENDSV,  0xC0       @ PendSV priority level (lowest)
.equ SHPR3_PRI_15,    0xE000ED23 @ System Handler Priority Register 3 (SysTick)
.equ PRI_LVL_SYSTICK, 0xC0       @ SysTick priority level (lowest)

@******************************************************************************
@                        CODE GENERATION DIRECTIVES
@******************************************************************************
.text
.align 2
.thumb
.syntax unified

@******************************************************************************
@ Functions:
@     size_t cpu_intrpt_save(void);
@     void cpu_intrpt_restore(size_t cpsr);
@******************************************************************************
.thumb_func
cpu_intrpt_save:
    MRS     R0, PRIMASK
    CPSID   I
    BX      LR

.thumb_func
cpu_intrpt_restore:
    MSR     PRIMASK, R0
    BX      LR

@******************************************************************************
@ Functions:
@     void cpu_intrpt_switch(void);
@     void cpu_task_switch(void);
@******************************************************************************
.thumb_func
cpu_task_switch:
    LDR     R0, =SCB_ICSR
    LDR     R1, =ICSR_PENDSVSET
    STR     R1, [R0]
    BX      LR

.thumb_func
cpu_intrpt_switch:
    LDR     R0, =SCB_ICSR
    LDR     R1, =ICSR_PENDSVSET
    STR     R1, [R0]
    BX      LR

@******************************************************************************
@ Functions:
@     void cpu_first_task_start(void);
@******************************************************************************
.thumb_func
cpu_first_task_start:
    @ set SVC priority to the lowest level
    LDR     R0, =SHPR2_PRI_11
    LDR     R1, =PRI_LVL_SVC
    STRB    R1, [R0]

    @ set PendSV priority to the lowest level
    LDR     R0, =SHPR3_PRI_14
    LDR     R1, =PRI_LVL_PENDSV
    STRB    R1, [R0]

    @ set Systick priority to te lowest level
    LDR     R0, =SHPR3_PRI_15
    LDR     R1, =PRI_LVL_SYSTICK
    STRB    R1, [R0]

    @indicate PendSV_Handler branch to _pendsv_handler_nosave
    MOVS    R0, #0
    MSR     PSP, R0

    @make PendSV exception pending
    LDR     R0, =SCB_ICSR
    LDR     R1, =ICSR_PENDSVSET
    STR     R1, [R0]

    @goto PendSV_Handler
    CPSIE   I
    B       .

@******************************************************************************
@ Functions:
@     void krhino_pendsv_handler(void);
@******************************************************************************
.thumb_func
PendSV_Handler:
    CPSID   I
    MRS     R0, PSP
    @branch if cpu_first_task_start
    CMP     R0, #0
    BEQ     _first_task_restore

    @hardware saved R0~R3,R12,LR,PC,xPSR

    @save context
    SUBS    R0, R0, #0x24
    STMIA   R0!, {R4 - R7}
    MOV     R4, R8
    MOV     R5, R9
    MOV     R6, R10
    MOV     R7, R11
    STMIA   R0!, {R4 - R7}
    MOV     R4, LR
    STMIA   R0!, {R4}

    @g_active_task->task_stack = context region
    SUBS    R0, R0, #0x24
    LDR     R1, =g_active_task
    LDR     R1, [R1]
    STR     R0, [R1]

    MOVS    R3, #0x01
    LDRB    R2, [R1, #RHINO_CONFIG_TASK_MODE_OFFSET]
    TST     R2, R3
    BNE     .store_ustack
    STR     R0, [R1, #RHINO_CONFIG_TASK_KSTACK_OFFSET]
    B       .store_stack_done
.store_ustack:
    STR    R0, [R1, #RHINO_CONFIG_TASK_USTACK_OFFSET]
.store_stack_done:

    @save and restore LR
#if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
    BL      krhino_stack_ovf_check
#endif
#if (RHINO_CONFIG_SYS_STATS > 0)
    BL      krhino_task_sched_stats_get
#endif

.thumb_func
_pendsv_handler_nosave:
    LDR     R0, =g_active_task
    LDR     R1, =g_preferred_ready_task
    LDR     R2, [R1] @ R2 -> new_task
    LDR     R3, [R0] @ R3 -> old_task
    STR     R2, [R0]

    MOV     R4, R2
    MOV     R0, R2
    MOV     R1, R3
    BL      k_proc_switch
    MOV     R2, R4

    @ judge task mode, if it's unprivileged task,
    @ 1. change mode to unprivileged mode
    @ 2. use user stack, else use kernel stack
    LDRB    R0, [R2, #RHINO_CONFIG_TASK_MODE_OFFSET]
    MOVS    R3, #0x01
    TST     R0, R3
    BEQ     .privileged_mode

    MRS     R1, CONTROL
    ORRS    R1, R3
    MSR     CONTROL, R1
    ISB
    LDR     R0, [R2, #RHINO_CONFIG_TASK_USTACK_OFFSET]
    B       .restore_context

.privileged_mode:
    MRS     R1, CONTROL
    BICS    R1, R3
    MSR     CONTROL, R1
    ISB
    LDR     R0, [R2, #RHINO_CONFIG_TASK_KSTACK_OFFSET]

.restore_context:
    @restore context
    LDMIA   R0!, {R4 - R7}
    LDMIA   R0!, {R2 - R3}
    MOV     R8,  R2
    MOV     R9,  R3
    LDMIA   R0!, {R2 - R3}
    MOV     R10,  R2
    MOV     R11,  R3
    LDMIA   R0!,  {R2}
    MOV     LR,  R2

    @return stack = PSP
    MSR     PSP, R0

    CPSIE   I
    @hardware restore R0~R3,R12,LR,PC,xPSR
    BX      LR

.thumb_func
_first_task_restore:
    @set MSP to the base of system stack
    MRS     R0, MSP
    LSRS    R0, R0, #3
    LSLS    R0, R0, #3
    MSR     MSP, R0

    B       _pendsv_handler_nosave

.end

