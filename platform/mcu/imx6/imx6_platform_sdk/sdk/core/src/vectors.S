/*
 * Copyright (c) 2010-2012, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "asm_defines.h"

    .code 32
    
    .global _start
    .global data_abort_handler
    .global prefetch_abort_handler
    .global IRQ_HDLR

/*
 * Vector table
 *
 * This table will be copied into the RAM vector table by the startup code. The ROM
 * vector table at address 0 will then point to the copy of this table.
 */
    .section .vectors,"ax"
    .code 32
    .global vectors

vectors:
    ldr pc, _vector_reset
    ldr pc, _vector_undef
    ldr pc, _vector_swi
    ldr pc, _vector_pabt
    ldr pc, _vector_dabt
    ldr pc, _vector_resv
    ldr pc, _vector_irq
    ldr pc, _vector_fiq
    .word 0                     // extra word in the RAM vectors

_vector_reset:
    .word     _start
_vector_undef:
    .word     exception_handler
_vector_swi:
    .word     exception_handler
_vector_pabt:
    .word     prefetch_abort_handler
_vector_dabt:
    .word     data_abort_handler
_vector_resv:
    .word     exception_handler
_vector_irq:
    .word     vector_irq
        //IRQ_HDLR
_vector_fiq:
    .word     exception_handler

    .word 0 // extra word in RAM vectors


    .text

/*
 * Common exception handlers
 */
    .global exception_handler
    .func exception_handler
    // Just loop infinitely for exceptions we don't explicitly handle.
    // For debug builds, we cause a breakpoint first so a debugger will halt.
exception_handler:
#if DEBUG
        bkpt    #0
#endif
1:
        b       1b
    .endfunc

    .end
