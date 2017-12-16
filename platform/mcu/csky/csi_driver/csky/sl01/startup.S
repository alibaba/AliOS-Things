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
 * @brief    startup file for PHOBOS. Should use with
 *           GCC for CSKY Embedded Processors
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include <config.h>

#ifdef CONFIG_KERNEL_FREERTOS
.import CoretimeIsr
.import CKPendSVIsr
#endif

#undef  VIC_TSPR
#define VIC_TSPR  0xE000EC10

#ifdef CONFIG_KERNEL_RHINO
.import systick_handler
.import tspend_handler
#endif

#ifdef CONFIG_KERNEL_UCOS
.import OSTickISR
.import tspend_handler
#endif

	.section .vectors
	.align 10
	.globl	__Vectors
	.type	__Vectors, @object
__Vectors:
	.long	Reset_Handler         /* Reset Handler */
	.rept   15
	.long   Default_Handler
	.endr
#ifdef CONFIG_KERNEL_FREERTOS
	.long   CKPendSVIsr
#else
	.long   Default_Handler
#endif
	.rept   5
	.long   Default_Handler
	.endr
#if defined (CONFIG_KERNEL_UCOS) || defined (CONFIG_KERNEL_RHINO)
	.long   tspend_handler
	.rept	9
	.long	Default_Handler
	.endr
#else
	.rept   10
	.long   Default_Handler
	.endr
#endif

	/* External interrupts */
	.long	USART_IRQHandler         /*  0:  UART          */
#ifdef CONFIG_KERNEL_FREERTOS
	.long   CoretimeIsr             /*  1:  core Timer    */
#elif defined CONFIG_KERNEL_UCOS
	.long   OSTickISR               /*  1:  core Timer    */
#elif defined CONFIG_KERNEL_RHINO
    .long   systick_handler         /*  1:  core Timer    */
#else
	.long	CORET_IRQHandler        /*  1:  core Timer    */
#endif
	.long	TIM0_IRQHandler         /*  2:  Timer0        */
	.long	TIM1_IRQHandler         /*  3:  Timer1        */
	.long	TIM2_IRQHandler         /*  4:  Timer2        */
	.long	TIM3_IRQHandler         /*  5:  Timer3        */
	.long	Default_Handler
	.long	GPIO0_IRQHandler        /*  7:  GPIO0         */
	.long	GPIO1_IRQHandler        /*  8:  GPIO1         */
	.long	GPIO2_IRQHandler        /*  9:  GPIO2         */
	.long	GPIO3_IRQHandler        /*  10:  GPIO3        */
	.long	GPIO4_IRQHandler        /*  11:  GPIO4        */
	.long	GPIO5_IRQHandler        /*  12:  GPIO5        */
	.long	GPIO6_IRQHandler        /*  13:  GPIO6        */
	.long	GPIO7_IRQHandler        /*  14:  GPIO7        */
	.long	Default_Handler
	.long	STIM0_IRQHandler        /*  16:  STimer0      */
	.long	STIM1_IRQHandler        /*  17:  STimer1      */
	.long	STIM2_IRQHandler        /*  18:  STimer2      */
	.long	STIM3_IRQHandler        /*  19:  STimer3      */
    .long   PAD_IRQHandler          /*  20:  pad          */

	.size	__Vectors, . - __Vectors

	.text
	.align	1
_start:
	.text
	.align	1
	.globl	Reset_Handler
	.type	Reset_Handler, %function
Reset_Handler:
    /* under normal circumstances,  it should not be opened */

#ifndef CONFIG_SYSTEM_SECURE
    lrw   r0, 0x80000000
    mtcr  r0, psr
#endif

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
	lrw	r1, __erodata
	lrw	r2, __data_start__
	lrw	r3, __data_end__

	subu	r3, r2
	cmpnei	r3, 0
	bf	.L_loop0_done

.L_loop0:
	ldw	r0, (r1, 0)
	stw	r0, (r2, 0)
	addi	r1, 4
	addi	r2, 4
	subi	r3, 4
	cmpnei	r3, 0
	bt	.L_loop0

.L_loop0_done:

/*
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */
	lrw	r1, __bss_start__
	lrw	r2, __bss_end__

	movi	r0, 0

	subu	r2, r1
	cmpnei	r2, 0
	bf	.L_loop1_done

.L_loop1:
	stw	r0, (r1, 0)
	addi	r1, 4
	subi	r2, 4
	cmpnei	r2, 0
	bt	.L_loop1
.L_loop1_done:

#ifndef __NO_SYSTEM_INIT
	bsr	SystemInit
#endif

#ifndef __NO_BOARD_INIT
	bsr	board_init
#endif

#if defined (CONFIG_KERNEL_UCOS) || defined (CONFIG_KERNEL_RHINO)
	lrw    r0, VIC_TSPR
	movi   r1, 0xb00
	stw    r1, (r0)
#endif

	bsr	main
	.size	Reset_Handler, . - Reset_Handler

__exit:
	bkpt

	.align	1
	.weak	Default_Handler
	.type	Default_Handler, %function
Default_Handler:
	br	Default_Handler
	.size	Default_Handler, . - Default_Handler

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
	.macro	def_irq_handler	handler_name
	.weak	\handler_name
	.set	\handler_name, Default_Handler
	.endm

	def_irq_handler	CORET_IRQHandler
	def_irq_handler STIM0_IRQHandler
	def_irq_handler STIM1_IRQHandler
	def_irq_handler STIM2_IRQHandler
	def_irq_handler STIM3_IRQHandler
	def_irq_handler TIM0_IRQHandler
	def_irq_handler TIM1_IRQHandler
	def_irq_handler TIM2_IRQHandler
	def_irq_handler TIM3_IRQHandler
	def_irq_handler	USART_IRQHandler
	def_irq_handler	GPIO0_IRQHandler
	def_irq_handler	GPIO1_IRQHandler
	def_irq_handler	GPIO2_IRQHandler
	def_irq_handler	GPIO3_IRQHandler
	def_irq_handler	GPIO4_IRQHandler
	def_irq_handler	GPIO5_IRQHandler
	def_irq_handler	GPIO6_IRQHandler
	def_irq_handler	GPIO7_IRQHandler
	def_irq_handler	PAD_IRQHandler

	.end
