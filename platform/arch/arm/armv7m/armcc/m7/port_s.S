;******************************************************************************
;                            EXTERN PARAMETERS
;******************************************************************************

    IMPORT  g_active_task
    IMPORT  g_preferred_ready_task
    IMPORT  krhino_stack_ovf_check

;******************************************************************************
;                            EXPORT FUNCTIONS
;******************************************************************************

    EXPORT  cpu_intrpt_save    
    EXPORT  cpu_intrpt_restore
    EXPORT  cpu_task_switch
    EXPORT  cpu_intrpt_switch
    EXPORT  cpu_first_task_start

    EXPORT  PendSV_Handler

;******************************************************************************
;                                 EQUATES
;******************************************************************************

SCB_ICSR              EQU      0xE000ED04 ; Interrupt Control and State Register.
ICSR_PENDSVSET        EQU      0x10000000 ; Value to trigger PendSV exception.

SHPR3_PRI_14          EQU      0xE000ED22 ; System Handler Priority Register 3 (PendSV).
PRI_LVL_PENDSV        EQU      0xFF       ; PendSV priority level (lowest).
SHPR3_PRI_15          EQU      0xE000ED23 ; System Handler Priority Register 3 (SysTick).
PRI_LVL_SYSTICK       EQU      0xFF       ; SYstick priority level (lowest).

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
    ;set PendSV prority to the lowest
    LDR     R0, =SHPR3_PRI_14
    LDR     R1, =PRI_LVL_PENDSV
    STRB    R1, [R0]
    
    ;set Systick prority to the lowest
    LDR     R0, =SHPR3_PRI_15
    LDR     R1, =PRI_LVL_SYSTICK
    STRB    R1, [R0]

    ;indicate PendSV_Handler branch to _pendsv_handler_nosave
    MOVS    R0, #0
    MSR     PSP, R0

    ;align MSP to 8 byte
    MRS     R0, MSP
    LSRS    R0, R0, #3
    LSLS    R0, R0, #3
    MSR     MSP, R0

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
    CBZ     R0, _pendsv_handler_nosave

    ;hardware saved R0~R3,R12,LR,PC,xPSR

    ;save context
    IF      {FPU} != "SoftVFP"
    VSTMFD  R0!, {D8 - D15}
    ENDIF

    SUBS    R0, R0, #0x24
    STM     R0, {R4-R11, LR}

    ;g_active_task->task_stack = context region
    LDR     R1, =g_active_task
    LDR     R1, [R1]
    STR     R0, [R1]

    bl      krhino_stack_ovf_check

_pendsv_handler_nosave
    LDR     R0, =g_active_task
    LDR     R1, =g_preferred_ready_task
    LDR     R2, [R1]
    STR     R2, [R0]
    ;R0 = g_active_task->task_stack = context region
    LDR     R0, [R2]

    ;restore context
    LDM     R0, {R4-R11, LR}
    ADDS    R0, R0, #0x24

    IF	    {FPU} != "SoftVFP"
    VLDMFD  R0!, {D8 - D15}
    ENDIF

    ;return stack = PSP
    MSR     PSP, R0

    ;after exception return: stack = PSP
    ORR     LR, LR, #0x04
    
    CPSIE   I
    ;hardware restore R0~R3,R12,LR,PC,xPSR
    BX      LR

    ALIGN
    END

