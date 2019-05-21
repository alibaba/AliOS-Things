/**
 ****************************************************************************************
 *
 * @file boot_vectors.s
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

	.globl _vector_start
	.globl	_undefined_instruction
	.globl	_software_interrupt
	.globl	_prefetch_abort
	.globl	_data_abort
	.globl	_not_used
	.globl	_irq
	.globl	_fiq
	
	.extern boot_reset
	.extern boot_swi
	.extern boot_undefined
	.extern boot_pabort
	.extern boot_dabort
	.extern boot_reserved
	.extern irq_handler
	.extern fiq_handler

/*
 *************************************************************************
 *
 * Vectors have their own section so linker script can map them easily
 *
 *************************************************************************
 */

	.section ".vectors", "ax"

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
	b	boot_reset
	ldr	pc, _undefined_instruction
	ldr	pc, _software_interrupt
	ldr	pc, _prefetch_abort
	ldr	pc, _data_abort
	ldr	pc, _not_used
	ldr	pc, _irq
	ldr	pc, _fiq

/*
 *************************************************************************
 *
 * Indirect vectors table
 *
 * Symbols referenced here must be defined somewhere else
 *
 *************************************************************************
 */
_undefined_instruction:	.word boot_undefined
_software_interrupt:	.word boot_swi
_prefetch_abort:	    .word boot_pabort
_data_abort:		    .word boot_dabort
_not_used:			    .word boot_reserved
_irq:				    .word irq_handler
_fiq:				    .word fiq_handler

	.balignl 16,0xdeadbeef

/* eof */	
