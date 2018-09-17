@******************************************************************************
@                            EXTERN PARAMETERS
@******************************************************************************

#include <k_config.h>
#include <k_default_config.h>
#include <aux_config.h>

.extern  g_active_task
.extern  g_preferred_ready_task
.extern  krhino_stack_ovf_check

@******************************************************************************
@                            EXPORT FUNCTIONS
@******************************************************************************

.global  cpu_get_sp
.global  cpu_set_sp
.global  cpu_intrpt_save
.global  cpu_intrpt_restore
.global  cpu_task_switch
.global  cpu_intrpt_switch
.global  cpu_first_task_start
.global  cpu_switch_to_usermode

.global  PendSV_Handler
.global  _first_task_restore

@******************************************************************************
@                                 EQUATES
@******************************************************************************

.equ SCB_ICSR,        0xE000ED04 @ Interrupt Control and State Register.
.equ SCB_VTOR,        0xE000ED08 @ Vector Table Offset Register.
.equ ICSR_PENDSVSET,  0x10000000 @ Value to trigger PendSV exception.

.equ SHPR2_PRI_11,    0xE000ED1F @ System Handler Priority Register 2 (SVC)
.equ PRI_LVL_SVC,     0xFF       @ SVC priority level (lowest)
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
cpu_get_sp:
    MRS     R0, CONTROL
    TST     R0, #0x02
    ITE     EQ
    MRSEQ   R0, MSP
    MRSNE   R0, PSP
    BX      LR

.thumb_func
cpu_set_sp:
    PUSH    {R0}
    MRS     R0, CONTROL
    TST     R0, #0x02
    ITE     EQ
    MRSEQ   R0, MSP
    MRSNE   R0, PSP
    STMDB   R0!, {R1,R2}
    LDR     R1, [R0, #8]
    MRS     R2, CONTROL
    TST     R2, #0x02
    ITE     EQ
    MSREQ   MSP, R1
    MSRNE   PSP, R1
    LDMIA   R0!, {R1, R2}
    BX      LR


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
    @set SVC priority to the lowest
    LDR     R0, =SHPR2_PRI_11
    LDR     R1, =PRI_LVL_SVC
    STRB    R1, [R0]

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
    #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    @if the switchout task use FPU, save the FPU regs
    TST     LR, #0x10
    IT      EQ
    VSTMDBEQ R0!, {D8 - D15}
    @hardware saved D0~D7, FPSCR
    #endif

    SUBS    R0, R0, #0x24
    STM     R0, {R4-R11, LR}

    @g_active_task->task_stack = context region
    LDR     R1, =g_active_task
    LDR     R1, [R1]

#if (RHINO_CONFIG_USER_SPACE > 0)
    LDRB    R2, [R1, #RHINO_CONFIG_TASK_MODE_OFFSET]
    AND     R2, #0x03
    TST     R2, #0x01
    ITE     EQ
    STREQ   R0, [R1]
    STRNE   R0, [R1, #RHINO_CONFIG_TASK_USTACK_OFFSET]
#else
    STR     R0, [R1]
#endif

    bl      krhino_stack_ovf_check

.thumb_func
_pendsv_handler_nosave:
    LDR     R0, =g_active_task
    LDR     R1, =g_preferred_ready_task
    LDR     R2, [R1]
    STR     R2, [R0]

#if (RHINO_CONFIG_USER_SPACE > 0)
    // judge task mode, if it's unprivileged task,
    // 1. change mode to unprivileged mode
    // 2. use user stack, else use kernel stack
    LDRB     R0, [R2, #RHINO_CONFIG_TASK_MODE_OFFSET]
    AND      R0, R0, #0x03
    TST      R0, #0x01
    IT       EQ
    BEQ      .priviledged_mode

    MRS      R1, CONTROL
    ORR      R1, #0x01
    MSR      CONTROL, R1
    MRS      R1, CONTROL  // read back control

    LDR      R0, [R2, #RHINO_CONFIG_TASK_USTACK_OFFSET]
    B        .restore_context

.priviledged_mode:
#endif // RHINO_CONFIG_USER_SPACE

    @R0 = g_active_task->task_stack = context region
    LDR     R0, [R2]

.restore_context:
    @restore context
    LDM     R0, {R4-R11, LR}
    ADDS    R0, R0, #0x24

    #if (defined(__VFP_FP__) && !defined(__SOFTFP__))
    @if the switchin task use FPU, save the FPU regs
    TST     LR, #0x10
    IT      EQ
    VLDMIAEQ R0!, {D8 - D15}
    @hardware will restore D0~D7, FPSCR
    #endif

    @return stack = PSP
    MSR     PSP, R0

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

#if (RHINO_CONFIG_USER_SPACE > 0)
.thumb_func
cpu_switch_to_usermode:
    // set cur task mode[0] to 1
    push   {r0, r1, r2}
    ldr    r0, =g_active_task
    ldr    r0, [r0]
    ldr    r1, [r0, #RHINO_CONFIG_TASK_MODE_OFFSET]
    orr    r1, #0x03
    str    r1, [r0, #RHINO_CONFIG_TASK_MODE_OFFSET]

    // change cpu mode to unpriviledged mode
    mrs    r0, control
    orr    r0, r0, #0x01
    msr    control, r0

    pop {r0, r1, r2}
    bx     lr
#endif // RHINO_CONFIG_USER_SPACE

.end

