#include "k_config.h"
#include "k_dftdbg_config.h"

@******************************************************************************
@                                 EQUATES
@******************************************************************************

@******************************************************************************
@                        CODE GENERATION DIRECTIVES
@******************************************************************************
.text
.align 2
.syntax unified

#if (RHINO_CONFIG_PANIC > 0)

@******************************************************************************
@                            EXTERN PARAMETERS
@******************************************************************************
.extern panicHandler
  
@******************************************************************************
@                            EXPORT FUNCTIONS
@******************************************************************************
.global	_exc_udf_handler
.global	_exc_pabt_handler
.global	_exc_dabt_handler

@******************************************************************************
@                             FAULT FUNCTIONS
@******************************************************************************
_exc_udf_handler:
    PUSH    {R0-R3}                                        
    MOV     R2, #1              @ Exception type
    MRS     R1, SPSR            @ CPSR before exception
    SUB     R0, LR, #2 
    TST     R1, #0x20           @ CPSR[5], 1 Thumb, 0 ARM
    SUBEQ   R0, R0, #2          @ PC before exception
    PUSH    {R4}                                        
    MOV     R3, SP              @ SP_udf
    ADD     SP, SP, #20
    B       _exc_handler

_exc_pabt_handler:
    PUSH    {R0-R3}                                        
    MOV     R2, #2              @ Exception type
    MRS     R1, SPSR            @ CPSR before exception
    SUB     R0, LR, #4          @ PC before exception
    PUSH    {R4}                                        
    MOV     R3, SP              @ SP_abt
    ADD     SP, SP, #20
    B       _exc_handler

_exc_dabt_handler:
    PUSH    {R0-R3}                                        
    MOV     R2, #3              @ Exception type
    MRS     R1, SPSR            @ CPSR before exception
    SUB     R0, LR, #8          @ PC before exception
    PUSH    {R4}                                        
    MOV     R3, SP              @ SP_abt
    ADD     SP, SP, #20
    B       _exc_handler

    @input: R0 PC; R1 CPSR; R2, exctype; R3 where saved context R0~R3; R4 temp
_exc_handler:
    AND     R4, R1, #0x1F
    ORR     R4, R4, #0xC0
    MSR     CPSR_c, R4
    LDMFD   R3!, {R4}
    STMFD   R3!, {R0-R2}        @ save "PANIC_CONTEXT" on exception stack
    MOV     R0, SP
    STMFD   R3!, {R0, LR}
    STMFD   R3!, {R4-R12}
    ADD     R0, R3, #56
    LDMFD   R0, {R5-R8}
    STMFD   R3!, {R5-R8}

    LDR     R0, =g_crash_steps
    LDR     R1, [R0]
    ADD     R1, #1
    STR     R1, [R0]

    MOV     R4, SP
    MOV     SP, R3
    MOV     R0, R3
    CMP     R1, #1
    MOVNE   R0, #0
    BL      panicHandler

#endif

.end

