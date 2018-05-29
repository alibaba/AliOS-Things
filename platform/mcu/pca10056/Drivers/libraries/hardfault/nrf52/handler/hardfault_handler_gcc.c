/**
 * Copyright (c) 2015 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
#include "sdk_common.h"
#if NRF_MODULE_ENABLED(HARDFAULT_HANDLER)
#include <stdint.h>
#include "compiler_abstraction.h"

extern void HardFault_c_handler(uint32_t *);

void HardFault_Handler(void) __attribute__(( naked ));

void HardFault_Handler(void)
{
    __ASM volatile(
    "   tst lr, #4                              \n"

    /* PSP is quite simple and does not require additional handler */
    "   itt ne                                  \n"
    "   mrsne r0, psp                           \n"
    /* Jump to the handler, do not store LR - returning from handler just exits exception */
    "   bne  HardFault_Handler_Continue         \n"

    /* Processing MSP requires stack checking */
    "   mrs r0, msp                             \n"

    "   ldr   r1, =__StackTop                   \n"
    "   ldr   r2, =__StackLimit                 \n"

    /* MSP is in the range of the stack area */
    "   cmp   r0, r1                            \n"
    "   bhi   HardFault_MoveSP                  \n"
    "   cmp   r0, r2                            \n"
    "   bhi   HardFault_Handler_Continue        \n"

    "HardFault_MoveSP:                          \n"
    "   mov   sp, r1                            \n"
    "   mov   r0, #0                            \n"

    "HardFault_Handler_Continue:                \n"
#if HARDFAULT_HANDLER_GDB_PSP_BACKTRACE
    "   mov r3, sp                              \n" /* Remember old SP */
    "   mov sp, r0                              \n" /* SP changed the pointer when hardfault was generated - we cannot just switch to PSP in exception */
    "   push {r3,lr}                            \n" /* Save old SP and LR on the task stack */
#if !defined(__SES_ARM)
    "   .cfi_def_cfa_offset 8                   \n"
    "   .cfi_offset 14, -4                      \n"
#endif
   /* No information about saved SP above (no .cfi_offset 13, -8).
    * In other case this would direct us back to using always MSP while backtracking */
    "   ldr r3, =%0                             \n"
    "   blx r3                                  \n"
    "   pop {r3,lr}                             \n"
    "   mov sp, r3                              \n"
    "   bx lr                                   \n"
#else // HARDFAULT_HANDLER_GDB_PSP_BACKTRACE
    "   ldr r3, =%0                             \n"
    "   bx r3                                   \n"
#endif
    "   .ltorg                                  \n"
    : : "X"(HardFault_c_handler)
    );
}
#endif //NRF_MODULE_ENABLED(HARDFAULT_HANDLER)
