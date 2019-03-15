/**
 ****************************************************************************************
 *
 * @file boot_recovery.s
 *
 * @brief ARM Exception Vectors table.
 *
 * Copyright (C) Beken Corp 2017-2026
 *
 ****************************************************************************************
/*
 *************************************************************************
 *
 * Symbol _vector_start is referenced elsewhere, so make it global
 *
 *************************************************************************
 */
    .globl  _vector_start

    .extern rec_process_flags
    .extern recovery_check
    .extern recovery_main
    .extern recovery_error
    .extern rec_2boot_cmd_check
    .extern rec_2boot_cmd_process

/*
 *************************************************************************
 *
 * Vectors have their own section so linker script can map them easily
 *
 *************************************************************************
 */
    .section ".rec_text", "ax"

/*
 *************************************************************************
 *
 *                                Macros
 *
 *************************************************************************
 */\
    .equ PROCESS_ADDR        ,rec_process_flags
    .equ PROCESS_NORMAL      ,0x0
    .equ PROCESS_RECOVERY    ,0x1

    .equ CPSR_MODE_MASK      ,0x1F
    .equ CPSR_MODE_USR       ,0x10
    .equ CPSR_MODE_FIQ       ,0x11
    .equ CPSR_MODE_IRQ       ,0x12
    .equ CPSR_MODE_SVC       ,0x13
    .equ CPSR_MODE_ABT       ,0x17
    .equ CPSR_MODE_UND       ,0x1B
    .equ CPSR_MODE_SYS       ,0x1F

    .equ CPSR_INT_DISABLE    ,0xC0

    .equ EXC_STACK_BASE      ,_err_stack_start
    .equ EXC_STACK_LEN       ,_err_stack_len
    .equ EXC_STACK_TOP       ,_err_stack_end

    .equ SYS_STACK_BASE      ,_main_stack_start
    .equ SYS_STACK_LEN       ,_main_stack_len
    .equ SYS_STACK_TOP       ,_err_stack_end
    .equ _vector_table       ,0x1a5c0

/**
 * Macro for switching ARM mode
 */
    .macro    RECOVERY_CHANGE_MODE, mode, mode_mask
    MRS   R0, CPSR
    BIC   R0, R0, #\mode_mask
    ORR   R0, R0, #\mode
    MSR   CPSR_c, R0
    .endm

/* ========================================================================
/**
 * Macro for setting the stack
 */
    .macro  RECOVERY_SET_STACK, stackStart, stackLen, color
    LDR   R0, =\stackStart
    LDR   R1, =\stackLen

    ADD   R1, R1, R0
    MOV   SP, R1        //Set stack pointer

    LDR   R2, =\color

3:
    CMP   R0, R1        //End of stack?
    STRLT R2, [r0]      //Colorize stack word
    ADDLT R0, R0, #4
    BLT   3b            //branch to previous local label
    .endm

/*
 *************************************************************************
 *
 * Exception vectors as described in ARM reference manuals
 *
 * Uses indirect branch to allow reaching handlers anywhere in memory.
 *
 *************************************************************************
 */
_vector_start:
    B    _recovery_reset
    B    _recovery_undefined
    B    _recovery_swi
    B    _recovery_pabort
    B    _recovery_dabort
    B    _recovery_reserved
    B    _recovery_irq
    B    _recovery_fiq

_recovery_reset:
    //Disable IRQ and FIQ before starting anything
    MRS   R0, CPSR
    ORR   R0, R0, #CPSR_INT_DISABLE
    MSR   CPSR_c, R0

    //set normal flag
	LDR   R0, =PROCESS_ADDR
	MOV   R1, #PROCESS_NORMAL
    STR   R1, [R0]

    //tmp stack
    LDR   SP, =SYS_STACK_TOP
    BL    _call_recovery_check
    CMP   R0, #1
    //normal reset, do not recovery
    BEQ _call_2boot_check

    //set recovery flag
	LDR   R2, =PROCESS_ADDR
	MOV   R1, #PROCESS_RECOVERY
    STR   R1, [R2]
    B    _recovery_process

_call_2boot_check:
    LDR   R0, =rec_2boot_cmd_check
    BLX   R0
    CMP   R0, #0
    BNE   _vector_table+0       //branch to "_vector_table"

    //in second bootloader cmd mode
    B     _call_2boot_cmd_process

_call_recovery_check:
    //call C function "recovery_check" and change to thumb mode
    LDR   R0, =recovery_check
    BX    R0

_recovery_undefined:
	PUSH  {R0}
	LDR   R0, =PROCESS_ADDR
	LDR   R0, [R0]
	CMP   R0, #PROCESS_RECOVERY
	POP   {R0}
    BNE   _vector_table+4       //branch to "_vector_table"
    //exception info saved
    STR   R0, [SP, #-4]
    SUB   R0, LR, #0
    STR   R0, [SP, #-8]
    B      _recovery_error

_recovery_swi:
	PUSH  {R0}
	LDR   R0, =PROCESS_ADDR
	LDR   R0, [R0]
	CMP   R0, #PROCESS_RECOVERY
	POP   {R0}
    BNE   _vector_table+8       //branch to "_vector_table"
    //exception info saved
    STR   R0, [SP, #-4]
    SUB   R0, LR, #0
    STR   R0, [SP, #-8]
    B     _recovery_error

_recovery_pabort:
	PUSH  {R0}
	LDR   R0, =PROCESS_ADDR
	LDR   R0, [R0]
	CMP   R0, #PROCESS_RECOVERY
	POP   {R0}
    BNE   _vector_table+12       //branch to "_vector_table"
    //exception info saved
    STR   R0, [SP, #-4]
    SUB   R0, LR, #4
    STR   R0, [SP, #-8]
    B     _recovery_error

_recovery_dabort:
	PUSH  {R0}
	LDR   R0, =PROCESS_ADDR
	LDR   R0, [R0]
	CMP   R0, #PROCESS_RECOVERY
	POP   {R0}
    BNE   _vector_table+16       //branch to "_vector_table"
    //exception info saved
    STR   R0, [SP, #-4]
    SUB   R0, LR, #8
    STR   R0, [SP, #-8]
    B     _recovery_error

_recovery_reserved:
	PUSH  {R0}
	LDR   R0, =PROCESS_ADDR
	LDR   R0, [R0]
	CMP   R0, #PROCESS_RECOVERY
	POP   {R0}
    BNE   _vector_table+20       //branch to "_vector_table"
    //exception info saved
    STR   R0, [SP, #-4]
    SUB   R0, LR, #0
    STR   R0, [SP, #-8]
    B     _recovery_error

/* recovery process always disable int, so here just branch */
_recovery_irq:
    B     _vector_table+24       //branch to "_vector_table"

_recovery_fiq:
    B     _vector_table+28       //branch to "_vector_table"

_recovery_error:
    MOV   R0, #(CPSR_INT_DISABLE|CPSR_MODE_SVC)
    MSR   CPSR_c, R0
    LDR   R0, =EXC_STACK_TOP
    LDR   R0, [R0, #-8]
    PUSH  {R0}                    //save exc_pc
    PUSH  {LR}
    ADD   R0, SP, #8
    PUSH  {R0}                    //save exc_sp
    PUSH  {R1-R12}
    LDR   R0, =EXC_STACK_TOP
    LDR   R0, [R0, #-4]
    PUSH  {R0}                    //save exc_r0

    MOV   R0, SP
    BL    _call_recovery_error
    B     .

_call_recovery_error:
    //call C function "recovery_error" and change to thumb mode
    LDR   R1, =recovery_error
    BX    R1

.macro PROCESS_PRAPARE
    //Setup all stacks //Note: Abt and Usr mode are not used
    RECOVERY_CHANGE_MODE CPSR_MODE_SYS CPSR_MODE_MASK
    RECOVERY_SET_STACK   EXC_STACK_BASE EXC_STACK_LEN 0

    RECOVERY_CHANGE_MODE CPSR_MODE_ABT CPSR_MODE_MASK
    RECOVERY_SET_STACK   EXC_STACK_BASE EXC_STACK_LEN 0

    RECOVERY_CHANGE_MODE CPSR_MODE_UND CPSR_MODE_MASK
    RECOVERY_SET_STACK   EXC_STACK_BASE EXC_STACK_LEN 0

    RECOVERY_CHANGE_MODE CPSR_MODE_IRQ CPSR_MODE_MASK
    RECOVERY_SET_STACK   EXC_STACK_BASE EXC_STACK_LEN 0

    RECOVERY_CHANGE_MODE CPSR_MODE_FIQ CPSR_MODE_MASK
    RECOVERY_SET_STACK   EXC_STACK_BASE EXC_STACK_LEN 0

    //Clear FIQ banked registers while in FIQ mode
    MOV   R8,  #0
    MOV   R9,  #0
    MOV   R10, #0
    MOV   R11, #0
    MOV   R12, #0

    //Stay in Supervisor Mode
    RECOVERY_CHANGE_MODE CPSR_MODE_SVC CPSR_MODE_MASK
    RECOVERY_SET_STACK   SYS_STACK_BASE SYS_STACK_LEN 0

    //copy data from binary to ram
    BL _recovery_copy_data_to_ram

    ///*Init the BSS section*/
    BL _recovery_zi_init

    //==================
    //Clear Registers
    MOV   R0, #0
    MOV   R1, #0
    MOV   R2, #0
    MOV   R3, #0
    MOV   R4, #0
    MOV   R5, #0
    MOV   R6, #0
    MOV   R7, #0
    MOV   R8, #0
    MOV   R9, #0
    MOV   R10, #0
    MOV   R11, #0
    MOV   R12, #0
.endm

//recovery update process. once run here, never go to normal.
_recovery_process:
    PROCESS_PRAPARE
    BL    _call_recovery_main
    B     .

_call_2boot_cmd_process:
    PROCESS_PRAPARE
    BL    rec_2boot_cmd_process
    B     .

_call_recovery_main:
    //call C function "recovery_main" and change to thumb mode
    LDR   R1, =recovery_main
    BX    R1

/*FUNCTION:     _recovery_copy_data_to_ram*/
/*DESCRIPTION:  copy main stack code from FLASH/ROM to SRAM*/
_recovery_copy_data_to_ram:
    LDR   R0, =_data_flash_begin
    LDR   R1, =_data_ram_begin
    LDR   R2, =_data_ram_end

4:  CMP   R1, R2
    LDRLO R4, [R0], #4
    STRLO R4, [R1], #4
    BLO   4b
    BX    LR

/*FUNCTION:     _recovery_zi_init*/
/*DESCRIPTION:  Initialise Zero-Init Data Segment*/
_recovery_zi_init:
    LDR   R0, =_bss_start
    LDR   R1, =_bss_end

    MOV   R3, R1
    MOV   R4, R0
    MOV   R2, #0
5:  CMP   R4, R3
    STRLO R2, [R4], #4
    BLO   5b
    BX    LR

    .section ".rec_flags", "a"
    .word 0x0           //start with normal
    //.word 0xffffffff    //start with recovery
    .fill 511, 4 , 0xffffffff

/* eof */    
