@******************************************************************************
@                            EXTERN PARAMETERS
@******************************************************************************

.extern g_active_task
.extern g_preferred_ready_task

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

.equ NVIC_INT_CTRL,   0xE000ED04 @ Interrupt control state register.
.equ NVIC_SYSPRI2,    0xE000ED20 @ System priority register.
.equ NVIC_KERNEL_PRI, 0xFFFF0000 @ PendSV and SysTick priority value (lowest).
.equ NVIC_PENDSVSET,  0x10000000 @ Value to trigger PendSV exception.

@******************************************************************************
@                        CODE GENERATION DIRECTIVES
@******************************************************************************

.text
.align 4
.syntax unified
.code 16        /* assemble in Thumb-2  (.thumb" can also be used) */

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
    LDR     R0, =NVIC_INT_CTRL
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    BX      LR

.thumb_func
cpu_intrpt_switch:
    LDR     R0, =NVIC_INT_CTRL
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]
    BX      LR

@******************************************************************************
@ Functions:
@     void cpu_first_task_start(void);
@******************************************************************************

.thumb_func
cpu_first_task_start:
    LDR     R0, =NVIC_SYSPRI2
    LDR     R1, =NVIC_KERNEL_PRI
    STR     R1, [R0]

    MOVS    R0, #0
    MSR     PSP, R0

    MRS     R0, MSP
    LSRS    R0, R0, #3
    LSLS    R0, R0, #3
    MSR     MSP, R0

    LDR     R0, =NVIC_INT_CTRL
    LDR     R1, =NVIC_PENDSVSET
    STR     R1, [R0]

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
    CMP     R0, #0
    BEQ     _pendsv_handler_nosave

    SUBS    R0, R0, #32
    LDR     R1, =g_active_task
    LDR     R1, [R1]
    STR     R0, [R1]
    STMIA   R0!, {R4-R7}
    MOV     R4, R8
    MOV     R5, R9
    MOV     R6, R10
    MOV     R7, R11
    STMIA   R0!, {R4-R7}  

.thumb_func
_pendsv_handler_nosave:
    LDR     R0, =g_active_task
    LDR     R1, =g_preferred_ready_task
    LDR     R2, [R1]
    STR     R2, [R0]

    LDR     R0, [R2]
    ADDS    R0, R0, #16
    LDMIA   R0!, {R4-R7}
    MOV     R8, R4
    MOV     R9, R5
    MOV     R10, R6
    MOV     R11, R7
    MSR     PSP, R0
    SUBS    R0, R0, #32
    LDMIA   R0!, {R4-R7} 

    MOV     R0, LR
    MOVS    R1, #4
    ORRS    R0, R1
    MOV     LR, R0
    CPSIE   I
    BX      LR

.end

