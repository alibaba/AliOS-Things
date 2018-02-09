/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 /******************************************************************************
 * @file     startup.S
 * @brief    startup file for ZX297100. Should use with
 *           GCC for CSKY Embedded Processors
 * @version  V1.0
 * @date     27. Sep 2017
 ******************************************************************************/

#include <csi_config.h>
/* For exporting varialbe or functions */
.export Reset_Handler                 /* The system entry. */
.export ISR_Reserved
.import trap
#ifdef CONFIG_KERNEL_NONE
.import vectorirq_handler
#endif
#ifndef CONFIG_KERNEL_NONE
#ifndef CONFIG_HAVE_VIC
    .import NOVIC_IRQ_Default_Handler
#endif
#endif

.section .vectors
    .align 10
    .globl ckcpu_vsr_table
ckcpu_vsr_table:
    .long Reset_Handler         /* Reset Handler */
#ifdef CONFIG_KERNEL_NONE
    .rept 31
    .long Default_Handler
    .endr
    .long Default_Handler       /* 32 default interrupt entry  */
    .rept 30
    .long vectorirq_handler
    .endr
    .long vectorirq_handler      /* 29 gpio6 irq hander */
#else
    .rept 31
    .long trap
    .endr
    .long NOVIC_IRQ_Default_Handler       /* 32 default interrupt entry  */
    .rept 30
    .long NOVIC_IRQ_Default_Handler
    .endr
    .long NOVIC_IRQ_Default_Handler       /* 63 tspend entry  */
#endif

.text
     .globl  Reset_Handler
Reset_Handler:
    /*
     * Setup initial vector base table for interrupts and exceptions
     */
#ifndef CONFIG_SYSTEM_SECURE
    lrw     r3, 0x80000100
    mtcr    r3, psr
#endif

    /* Initialize the normal stack pointer from the linker definition. */
    lrw a3, __StackTop
    mov sp, a3

    /*
     *  The ranges of copy from/to are specified by following symbols
     *    __etext: LMA of start of the section to copy from. Usually end of text
     *    __data_start__: VMA of start of the section to copy to
     *    __data_end__: VMA of end of the section to copy to
     *
     *  All addresses must be aligned to 4 bytes boundary.
     */
    lrw r3, __erodata
    lrw r4, __data_start__
    lrw r5, __data_end__

    subu    r5, r4
    cmpnei  r5, 0
    bf  .L_loop0_done

.L_loop0:
    ldw r6, (r3, 0)
    stw r6, (r4, 0)
    addi    r3, 4
    addi    r4, 4
    subi    r5, 4
    cmpnei  r5, 0
    bt  .L_loop0

.L_loop0_done:

    /*Zero out the bss region.
     * NOTE: __bss_start__ and __bss_end__ must align 4
     */
    lrw    r7, __bss_start__    /* Get start of bss from linking script file */
    lrw    r6, __bss_end__      /* Get end of bss from linking script file */
    subu   r6, r7               /* Calculate size of bss */
    lsri   r6, r6, 2            /* Size of whole words */
    cmpnei r6, 0
    bf     .Lbss_init_end
    movi   r5, 0            /* Set zero value to write */

.Lbss_init_loop:
    stw    r5, (r7)         /* Zero next word */
    addi   r7, 4            /* Increase bss pointer */
    subi   r6, 1            /* Decrease counter */
    cmpnei r6, 0
    bt     .Lbss_init_loop  /* Repeat for all bss */

.Lbss_init_end:
#ifndef __NO_SYSTEM_INIT
    jbsr SystemInit
#endif

#ifndef __NO_BOARD_INIT
    jbsr board_init
#endif
    jbsr main
    .size   Reset_Handler, . - Reset_Handler

__exit:
    br __exit

    .align  1
    .weak   Default_Handler
    .type   Default_Handler, %function
Default_Handler:
        br      trap
        .size   Default_Handler, . - Default_Handler

    /*    Macro to define default handlers. Default handler
     *    will be weak symbol and just dead loops. They can be
     *    overwritten by other handlers
     */
	.macro	def_irq_handler	handlers_name
	.weak	\handler_name
	.set	\handler_name, Default_Handler
	.endm
ISR_Reserved:
    br ISR_Reserved

.section .bss

	.align  2
	.globl  g_intstackalloc
	.global g_intstackbase
	.global g_top_irqstack
g_intstackalloc:
g_intstackbase:
	.space CONFIG_ARCH_INTERRUPTSTACK
g_top_irqstack:


