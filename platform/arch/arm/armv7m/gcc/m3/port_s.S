@******************************************************************************
@                            EXTERN PARAMETERS
@******************************************************************************

.extern  g_active_task
.extern  g_preferred_ready_task
.extern  krhino_stack_ovf_check

@******************************************************************************
@                            EXPORT FUNCTIONS
@******************************************************************************

.global  cpu_intrpt_save
.global  cpu_intrpt_restore
.global  cpu_task_switch
.global  cpu_intrpt_switch
.global  cpu_first_task_start

.global  PendSV_Handler
.global  _first_task_restore

@******************************************************************************
@                                 EQUATES
@******************************************************************************

.equ SCB_ICSR,        0xE000ED04 @ Interrupt Control and State Register.
.equ SCB_VTOR,        0xE000ED08 @ Vector Table Offset Register.
.equ ICSR_PENDSVSET,  0x10000000 @ Value to trigger PendSV exception.

.equ SHPR3_PRI_14,    0xE000ED22 @ System Handler Priority Register 3 (PendSV).
.equ PRI_LVL_PENDSV,  0xFF       @ PendSV priority level (lowest).
.equ SHPR3_PRI_15,    0xE000ED23 @ System Handler Priority Register 3 (Systick).
.equ PRI_LVL_SYSTICK, 0xFF       @ SYstick priority level (lowest).

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
    @set PendSV prority to the lowest
    LDR     R0, =SHPR3_PRI_14
    LDR     R1, =PRI_LVL_PENDSV
    STRB    R1, [R0]
    
    @set Systick prority to the lowest
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
    STM     R0, {R4-R11, LR}

    @g_active_task->task_stack = context region
    LDR     R1, =g_active_task
    LDR     R1, [R1]
    STR     R0, [R1]

    bl      krhino_stack_ovf_check
.thumb_func
_pendsv_handler_nosave:
    LDR     R0, =g_active_task
    LDR     R1, =g_preferred_ready_task
    LDR     R2, [R1]
    STR     R2, [R0]
    @R0 = g_active_task->task_stack = context region
    LDR     R0, [R2]

    @restore context
    LDM     R0, {R4-R11, LR}
    ADDS    R0, R0, #0x24

    @return stack = PSP
    MSR     PSP, R0

    @after exception return: stack = PSP
    ORR     LR, LR, #0x04
    
    CPSIE   I
    @hardware restore R0~R3,R12,LR,PC,xPSR
    BX      LR

.thumb_func
_first_task_restore:
    @set MSP to the base of system stack
    LDR     R0, =SCB_VTOR
    LDR     R0, [R0]
    LDR     R0, [R0]
    MSR     MSP, R0

    B       _pendsv_handler_nosave

.end

