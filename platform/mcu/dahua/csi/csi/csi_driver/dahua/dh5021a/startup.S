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
 * @brief    startup file for HOBBIT1_2. Should use with
 *           GCC for CSKY Embedded Processors
 * @version  V1.0
 * @date     24. August 2017
 ******************************************************************************/

#include <config.h>

#ifndef CONFIG_KERNEL_NONE
#ifndef CONFIG_HAVE_VIC
.import NOVIC_IRQ_Default_Handler
.import NOVIC_CORETIM_Handler
#endif
#endif

#ifdef CONFIG_KERNEL_FREERTOS
.import CoretimeIsr
.import CKTrap0ISR
#endif

#ifdef CONFIG_KERNEL_RHINO
.import systick_handler
#endif

#ifdef CONFIG_KERNEL_UCOS
.import OSTickISR
#endif

    .section .vectors
    .align 10
    .globl   __Vectors
    .type    __Vectors, @object
__Vectors:
    .long   Reset_Handler         /* Reset Handler */
    .rept   9
    .long   Default_Handler
    .endr
    .long   NOVIC_IRQ_Default_Handler
    .rept   22
    .long   Default_Handler
    .endr

    /* External interrupts */
    .size    __Vectors, . - __Vectors

    .text
    .align    1
_start:
    .text
    .align    1
    .globl    Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:

    lrw r0, 0xe000f000   /* Cache register base address */    
    lrw r1, 0x03000069   /* CRCR0 value means: Cached data from 0x030000000, Size=256KB, Enable*/    
    stw r1, (r0, 0x8)    /* store 0x10000063 to CRCR0(0xe000f008) */    
    lrw r1, 0x1          /* store 0x1 to r1 */    
    stw r1, (r0, 0x4)    /* set CIR(0xe000f004) to 0x1, means invalid all caches */    
    lrw r1, 0x3          /* store 0x3 to r1 */    
    stw r1, (r0, 0x0)    /* set CER(0xe000f000) to 0x3, means enable all caches */
    /* under normal circumstances,  it should not be opened */

#ifndef CONFIG_SYSTEM_SECURE
    lrw   r0, 0x80000000
    mtcr  r0, psr
#endif

	/*lrw r0, 0x14030000
	movi r1, 's'
	stb r1, (r0, 0)*/

/* Initialize the normal stack pointer from the linker definition. */
    lrw a1, __StackTop
    mov sp, a1

/*
 *  The ranges of copy from/to are specified by following symbols
 *    __etext: LMA of start of the section to copy from. Usually end of text
 *    __data_start__: VMA of start of the section to copy to
 *    __data_end__: VMA of end of the section to copy to
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
    lrw    r1, __erodata
    lrw    r2, __data_start__
    lrw    r3, __data_end__

    subu    r3, r2
    cmpnei    r3, 0
    bf    .L_loop0_done

.L_loop0:
    ldw    r0, (r1, 0)
    stw    r0, (r2, 0)
    addi    r1, 4
    addi    r2, 4
    subi    r3, 4
    cmpnei    r3, 0
    bt    .L_loop0

.L_loop0_done:

/*
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */
    lrw    r1, __bss_start__
    lrw    r2, __bss_end__

    movi    r0, 0

    subu    r2, r1
    cmpnei    r2, 0
    bf    .L_loop1_done

.L_loop1:
    stw    r0, (r1, 0)
    addi    r1, 4
    subi    r2, 4
    cmpnei    r2, 0
    bt    .L_loop1
.L_loop1_done:

#ifndef __NO_SYSTEM_INIT
    bsr    SystemInit
#endif

#ifndef __NO_BOARD_INIT
    bsr    board_init
#endif

    bsr    main

    .size    Reset_Handler, . - Reset_Handler
__exit:
    bkpt

    .align  1
    .weak   Default_Handler
    .type   Default_Handler, %function
Default_Handler:
#ifndef CONFIG_KERNEL_NONE
#    br      trap
    br .
#else
    br      Default_Handler
#endif
    .size   Default_Handler, . - Default_Handler

.section .bss

    .align  2
    .globl  g_intstackalloc
    .global g_intstackbase
    .global g_top_irqstack
g_intstackalloc:
g_intstackbase:
    .space CONFIG_ARCH_INTERRUPTSTACK
g_top_irqstack:

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro  def_irq_handler handler_name
    .weak   \handler_name
    .set    \handler_name, Default_Handler
    .endm

    def_irq_handler CORET_IRQHandler
    def_irq_handler TIMA0_IRQHandler
    def_irq_handler TIMA1_IRQHandler
    def_irq_handler TIMB0_IRQHandler
    def_irq_handler TIMB1_IRQHandler
    def_irq_handler USART0_IRQHandler
    def_irq_handler USART1_IRQHandler
    def_irq_handler USART2_IRQHandler
    def_irq_handler USART3_IRQHandler
    def_irq_handler GPIO0_IRQHandler
    def_irq_handler GPIO1_IRQHandler
    def_irq_handler I2C0_IRQHandler
    def_irq_handler I2C1_IRQHandler
    def_irq_handler SPI0_IRQHandler
    def_irq_handler SPI1_IRQHandler
    def_irq_handler RTC_IRQHandler
    def_irq_handler RTC1_IRQHandler
    def_irq_handler WDT_IRQHandler
    def_irq_handler PWM_IRQHandler
    def_irq_handler DMAC_IRQHandler
    def_irq_handler AES_IRQHandler
    def_irq_handler RSA_IRQHandler
    def_irq_handler SHA_IRQHandler

    .end
