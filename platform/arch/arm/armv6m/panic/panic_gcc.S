#include "k_config.h"
#include "k_dftdbg_config.h"

@******************************************************************************
@                                 EQUATES
@******************************************************************************
.equ CONTEXT_REGION, 80   @bigger than sizeof(PANIC_CONTEXT)

@******************************************************************************
@                        CODE GENERATION DIRECTIVES
@******************************************************************************
.text
.align 2
.thumb
.syntax unified

#if (RHINO_CONFIG_PANIC > 0)

@******************************************************************************
@                            EXTERN PARAMETERS
@******************************************************************************
.extern panicHandler
.extern g_crash_steps
.extern _first_task_restore
.extern panicRestoreCheck
  
@******************************************************************************
@                            EXPORT FUNCTIONS
@******************************************************************************
.global	HardFault_Handler
.global	MemManage_Handler
.global	BusFault_Handler
.global	UsageFault_Handler

@******************************************************************************
@                             FAULT FUNCTIONS
@******************************************************************************
.thumb_func
HardFault_Handler:
.thumb_func
MemManage_Handler:
.thumb_func
BusFault_Handler:
.thumb_func
UsageFault_Handler:
    PUSH    {R1, LR}
    BL      panicRestoreCheck
    POP     {R1, R2}
    MOV     LR, R2
    CMP     R0, 0
    BEQ     unrecoverable_crash
    BL     _first_task_restore

unrecoverable_crash:
    @check double crash
    LDR     R1, =g_crash_steps
    LDR     R2, [R1]
    ADDS    R2, #1
    STR     R2, [R1]
    CMP     R2, #1
    BEQ     first_panic
    @return from exc to handle panic
    MRS     R1, PSP
    MOV     R2, LR
    LSLS    R2, R2, #29         @EXC_RETURN:bit2, 0 MSP, 1 PSP
    BMI     double_panic
    MRS     R1, MSP
double_panic:
    LDR     R0, =double_panic_entry
    STR     R0, [R1, #24]
    LDR     R0, [R1, #28]
    MOVS    R2, #1
    LSLS    R2, R2, #24
    ORRS    R0, R2
    STR     R0, [R1, #28]       @xPSR[24], 1 to set thumb mode
    BX      LR

double_panic_entry:
    MOVS    R0, #0              @double crash, do not save context
    BL      panicHandler
    B       .

first_panic:
    @R0 as PANIC_CONTEXT
    MOV     R0, SP
    SUBS    R0, #CONTEXT_REGION        

    @R1 as CONTEXT saved by hardware
    MRS     R1, PSP
    MOV     R2, LR
    LSLS    R2, R2, #29         @EXC_RETURN:bit2, 0 MSP, 1 PSP
    BMI     context_save
    MRS     R1, MSP
context_save:

    MOVS    R2, #16
    ADD     R2, R0
    STM     R2!,{R4-R7}         @ctx save, R4~R7
    MOV     R4, R8                
    MOV     R5, R9
    MOV     R6, R10
    MOV     R7, R11
    STM     R2!,{R4-R7}         @ctx save, R4~R11

    LDM     R1!,{R4-R7}
    STM     R0!,{R4-R7}         @ctx save, R0~R3
    SUBS    R0, #16

    LDM     R1!,{R4-R7}
    STM     R2!,{R4-R7}         @ctx save, R12 LR PC xPSR

    MOV     R4, R1              @SP before exception in R4
    SUBS    R1, #32

    @xPSR[9] to determine whether 
    @the previous top-of-stack was at offset 0x20, 0x24
    LSLS    R7, R7, #22         @xPSR:bit9, 0 no-aligner, 1 aligner
    BPL     sp_save
    ADDS    R4, #0x4
sp_save:
    STM     R2!,{R4}            @ctx save, SP

    MOV     R4, LR
    STM     R2!,{R4}            @ctx save, EXC_RETURN

    MRS     R4, IPSR
    STM     R2!,{R4}            @ctx save, EXC_NUMBER

    MRS     R4, PRIMASK
    STM     R2!,{R4}            @ctx save, PRIMASK

    @return from exc to handle panic
    STR     R0, [R1, #0]
    LDR     R0, =panic_entry
    STR     R0, [R1, #24]
    LDR     R0, [R1, #28]
    MOVS    R2, #1
    LSLS    R2, R2, #24
    ORRS    R0, R2
    STR     R0, [R1, #28]       @xPSR[24], 1 to set thumb mode
    CPSID   I
    BX      LR

panic_entry:
#if (RHINO_CONFIG_PANIC_PRT_INT > 0)
    MRS     R1, CONTROL
    MOVS    R2, #2
    BICS    R1, R2
    MSR     CONTROL, R1
    ISB
    @printf use interrupt, so here enable it
    CPSIE   I
#endif
    MOV     SP, R0
    BL      panicHandler
    B       .

#endif

.end

