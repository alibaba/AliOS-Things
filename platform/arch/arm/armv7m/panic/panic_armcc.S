#include "k_config.h"
#include "k_dftdbg_config.h"

;******************************************************************************
;                                 EQUATES
;******************************************************************************
CONTEXT_REGION  EQU  88   ;bigger than sizeof(PANIC_CONTEXT)

;******************************************************************************
;                        CODE GENERATION DIRECTIVES
;******************************************************************************
    AREA |.text|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8

#if (RHINO_CONFIG_PANIC > 0)

;******************************************************************************
;                            EXTERN PARAMETERS
;******************************************************************************
    EXTERN panicHandler
    EXTERN g_crash_steps
    EXTERN _first_task_restore
    EXTERN panicRestoreCheck

;******************************************************************************
;                            EXPORT FUNCTIONS
;******************************************************************************
    EXPORT HardFault_Handler
    EXPORT MemManage_Handler
    EXPORT BusFault_Handler
    EXPORT UsageFault_Handler

;******************************************************************************
;                             FAULT FUNCTIONS
;******************************************************************************
HardFault_Handler
MemManage_Handler
BusFault_Handler
UsageFault_Handler
    PUSH    {R1, LR}
    BL      panicRestoreCheck
    POP     {R1, LR}
    CBZ     R0, unrecoverable_crash
    BL     _first_task_restore

unrecoverable_crash
    ;check double crash
    LDR     R1, =g_crash_steps
    LDR     R2, [R1]
    ADD     R3, R2, #1
    STR     R3, [R1]
    CBZ     R2, first_panic
    ;return from exc to handle panic
    MRS     R1, PSP
    AND     R2, LR, #4          ;EXC_RETURN:bit2, 0 MSP, 1 PSP
    CBNZ    R2, double_panic
    MRS     R1, MSP
double_panic
    LDR     R0, =double_panic_entry
    STR     R0, [R1, #24]
    LDR     R0, [R1, #28]
    ORR     R0, R0, #0x1000000
    STR     R0, [R1, #28]       ;set thumb mode
    BX      LR

double_panic_entry
    MOV     R0, #0              ;double crash, do not save context
    BL      panicHandler
    B       .

first_panic
    ;R0 as PANIC_CONTEXT
    SUB     R0, SP, #CONTEXT_REGION        

    ;R1 as CONTEXT saved by hardware
    MRS     R1, PSP
    AND     R2, LR, #4          ;EXC_RETURN:bit2, 0 MSP, 1 PSP
    CBNZ    R2, context_save
    MRS     R1, MSP
context_save

    ADD     R2, R0, #16
    STM     R2!,{R4-R11}        ;ctx save, R4~R11

    LDM     R1, {R4-R11}
    STM     R0, {R4-R7}         ;ctx save, R0~R3

    STM     R2!,{R8-R11}        ;ctx save, R12 LR PC xPSR

    ;xPSR[9] and EXC_RETURN[4] to determine whether 
    ;the previous top-of-stack was at offset 0x20, 0x24, 0x68, or0x6C
    ADD     R4, R1, #0x20
  IF {FPU} != "SoftVFP"
    AND     R3, LR, #0x10       ;EXC_RETURN:bit4, 0 floating, 1 non-floating
    CBNZ    R3, check_aligner
    ADD     R4, R4, #0x48
check_aligner
  ENDIF
    LDR     R3, [R1, #28]
    AND     R3, R3, #0x200      ;xPSR:bit9, 0 no-aligner, 1 aligner
    CBZ     R3, sp_save
    ADD     R4, R4, #0x4
sp_save
    STM     R2!,{R4}            ;ctx save, SP

    MOV     R4, LR
    STM     R2!,{R4}            ;ctx save, EXC_RETURN

    MRS     R4, IPSR
    STM     R2!,{R4}            ;ctx save, EXC_NUMBER

    MRS     R4, PRIMASK
    STM     R2!,{R4}            ;ctx save, PRIMASK

    MRS     R4, FAULTMASK
    STM     R2!,{R4}            ;ctx save, FAULTMASK

    MRS     R4, BASEPRI
    STM     R2!,{R4}            ;ctx save, BASEPRI

    ;return from exc to handle panic
    STR     R0, [R1, #0]
    LDR     R0, =panic_entry
    STR     R0, [R1, #24]
    LDR     R0, [R1, #28]
    ORR     R0, R0, #0x1000000
    STR     R0, [R1, #28]       ;set thumb mode
    CPSID   I
    BX      LR

panic_entry
#if (RHINO_CONFIG_PANIC_PRT_INT > 0)
    MRS     R1, CONTROL
    MOVS    R2, #2
    BICS    R1, R2
    MSR     CONTROL, R1
    ISB
    ;printf use interrupt, so here enable it
    CPSIE   I
#endif

    MOV     SP, R0
    BL      panicHandler
    B       .

    ALIGN

#endif

    END
