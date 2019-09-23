/**
 ****************************************************************************************
 *
 * @file boot_2ndboot.s
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
    .extern ota_2ndboot_error
    .extern boot_main

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
    .equ PROCESS_ADDR        ,0x00
    .equ PROCESS_NORMAL      ,0x00
    .equ PROCESS_2NDBOOT    ,0x01

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
    .macro    NDBOOT_CHANGE_MODE, mode, mode_mask
    MRS   R0, CPSR
    BIC   R0, R0, #\mode_mask
    ORR   R0, R0, #\mode
    MSR   CPSR_c, R0
    .endm

/* ========================================================================
/**
 * Macro for setting the stack
 */
    .macro  NDBOOT_SET_STACK, stackStart, stackLen, color
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
    B    _2ndboot_reset
    B    _2ndboot_undefined
    B    _2ndboot_swi
    B    _2ndboot_pabort
    B    _2ndboot_dabort
    B    _2ndboot_reserved
    B    _2ndboot_irq
    B    _2ndboot_fiq

_2ndboot_reset:
    //Disable IRQ and FIQ before starting anything
    MRS   R0, CPSR
    ORR   R0, R0, #CPSR_INT_DISABLE
    MSR   CPSR_c, R0

    //set 2ndboot flag
    LDR   R0, =PROCESS_ADDR
    MOV   R1, #PROCESS_2NDBOOT
    STR   R1, [R0]

    //tmp stack
    LDR   SP, =SYS_STACK_TOP
    B    _2ndboot_process

_2ndboot_undefined:
    PUSH  {R0}
    LDR   R0, =PROCESS_ADDR
    LDR   R0, [R0]
    CMP   R0, #PROCESS_2NDBOOT
    POP   {R0}
    BNE   _vector_table+4       //branch to "_vector_table"
    //exception info saved
    STR   R0, [SP, #-4]
    SUB   R0, LR, #0
    STR   R0, [SP, #-8]
    B      _ota_2ndboot_error

_2ndboot_swi:
    PUSH  {R0}
    LDR   R0, =PROCESS_ADDR
    LDR   R0, [R0]
    CMP   R0, #PROCESS_2NDBOOT
    POP   {R0}
    BNE   _vector_table+8       //branch to "_vector_table"
    //exception info saved
    STR   R0, [SP, #-4]
    SUB   R0, LR, #0
    STR   R0, [SP, #-8]
    B     _ota_2ndboot_error

_2ndboot_pabort:
    PUSH  {R0}
    LDR   R0, =PROCESS_ADDR
    LDR   R0, [R0]
    CMP   R0, #PROCESS_2NDBOOT
    POP   {R0}
    BNE   _vector_table+12       //branch to "_vector_table"
    //exception info saved
    STR   R0, [SP, #-4]
    SUB   R0, LR, #4
    STR   R0, [SP, #-8]
    B     _ota_2ndboot_error

_2ndboot_dabort:
    PUSH  {R0}
    LDR   R0, =PROCESS_ADDR
    LDR   R0, [R0]
    CMP   R0, #PROCESS_2NDBOOT
    POP   {R0}
    BNE   _vector_table+16       //branch to "_vector_table"
    //exception info saved
    STR   R0, [SP, #-4]
    SUB   R0, LR, #8
    STR   R0, [SP, #-8]
    B     _ota_2ndboot_error

_2ndboot_reserved:
    PUSH  {R0}
    LDR   R0, =PROCESS_ADDR
    LDR   R0, [R0]
    CMP   R0, #PROCESS_2NDBOOT
    POP   {R0}
    BNE   _vector_table+20       //branch to "_vector_table"
    //exception info saved
    STR   R0, [SP, #-4]
    SUB   R0, LR, #0
    STR   R0, [SP, #-8]
    B     _ota_2ndboot_error

/* recovery process always disable int, so here just branch */
_2ndboot_irq:
    B     _vector_table+24       //branch to "_vector_table"

_2ndboot_fiq:
    B     _vector_table+28       //branch to "_vector_table"

_ota_2ndboot_error:
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
    BL    _call_ota_2ndboot_error
    B     .

_call_ota_2ndboot_error:
    //call C function "ota_2ndboot_error" and change to thumb mode
    LDR   R1, =ota_2ndboot_error
    BX    R1

.macro PROCESS_PRAPARE
    //Setup all stacks //Note: Abt and Usr mode are not used
    NDBOOT_CHANGE_MODE CPSR_MODE_SYS CPSR_MODE_MASK
    NDBOOT_SET_STACK   EXC_STACK_BASE EXC_STACK_LEN 0

    NDBOOT_CHANGE_MODE CPSR_MODE_ABT CPSR_MODE_MASK
    NDBOOT_SET_STACK   EXC_STACK_BASE EXC_STACK_LEN 0

    NDBOOT_CHANGE_MODE CPSR_MODE_UND CPSR_MODE_MASK
    NDBOOT_SET_STACK   EXC_STACK_BASE EXC_STACK_LEN 0

    NDBOOT_CHANGE_MODE CPSR_MODE_IRQ CPSR_MODE_MASK
    NDBOOT_SET_STACK   EXC_STACK_BASE EXC_STACK_LEN 0

    NDBOOT_CHANGE_MODE CPSR_MODE_FIQ CPSR_MODE_MASK
    NDBOOT_SET_STACK   EXC_STACK_BASE EXC_STACK_LEN 0

    //Clear FIQ banked registers while in FIQ mode
    MOV   R8,  #0
    MOV   R9,  #0
    MOV   R10, #0
    MOV   R11, #0
    MOV   R12, #0

    //Stay in Supervisor Mode
    NDBOOT_CHANGE_MODE CPSR_MODE_SVC CPSR_MODE_MASK
    NDBOOT_SET_STACK   SYS_STACK_BASE SYS_STACK_LEN 0

    //copy data from binary to ram
    BL _2ndboot_copy_data_to_ram

    ///*Init the BSS section*/
    BL _2ndboot_zi_init

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
_2ndboot_process:
    PROCESS_PRAPARE
    BL    boot_main
    B     _vector_table+0

/*FUNCTION:     _2ndboot_copy_data_to_ram*/
/*DESCRIPTION:  copy main stack code from FLASH/ROM to SRAM*/
_2ndboot_copy_data_to_ram:
    LDR   R0, =_data_flash_begin
    LDR   R1, =_data_ram_begin
    LDR   R2, =_data_ram_end

4:  CMP   R1, R2
    LDRLO R4, [R0], #4
    STRLO R4, [R1], #4
    BLO   4b
    BX    LR

/*FUNCTION:     _2ndboot_zi_init*/
/*DESCRIPTION:  Initialise Zero-Init Data Segment*/
_2ndboot_zi_init:
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
