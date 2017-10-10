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
.equ NVIC_SYSPRI14,   0xE000ED22 @ System priority register (priority 14).
.equ NVIC_PENDSV_PRI, 0xFF       @ PendSV priority value (lowest).
.equ NVIC_PENDSVSET,  0x10000000 @ Value to trigger PendSV exception.

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
    LDR     R0, =NVIC_SYSPRI14
    LDR     R1, =NVIC_PENDSV_PRI
    STRB    R1, [R0]

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

@******************************************************************************
@ Functions:
@     void krhino_pendsv_handler(void);
@******************************************************************************
.thumb_func
PendSV_Handler:
    CPSID   I
    MRS     R0, PSP
    CBZ     R0, _pendsv_handler_nosave

    #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    VSTMDB  r0!, {d8 - d15}
    #endif

    SUBS    R0, R0, #0x20
    STM     R0, {R4-R11}

    LDR     R1, =g_active_task
    LDR     R1, [R1]
    STR     R0, [R1]

.thumb_func
_pendsv_handler_nosave:
    LDR     R0, =g_active_task
    LDR     R1, =g_preferred_ready_task
    LDR     R2, [R1]
    STR     R2, [R0]

    LDR     R0, [R2]
    LDM     R0, {R4-R11}
    ADDS    R0, R0, #0x20

    #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    VLDMIA r0!, {d8 - d15}
    #endif

    MSR     PSP, R0
    ORR     LR, LR, #0x04
    CPSIE   I
    BX      LR

.end

