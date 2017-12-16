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

#ifndef CONFIG_KERNEL_NONE
#ifndef CONFIG_HAVE_VIC
.import NOVIC_IRQ_Default_Handler
.import NOVIC_CORETIM_Handler
#endif
#endif

#undef  VIC_TSPR
#define VIC_TSPR  0xE000EC10

#ifdef CONFIG_HAVE_VIC
.import CKPendSVIsr
#else
.import CKTrap0ISR
#endif

#ifdef CONFIG_HAVE_VIC
.import tspend_handler
#endif

.import CORET_IRQHandler

	.section .vectors
	.align 10
	.globl	__Vectors
	.type	__Vectors, @object
__Vectors:
	.long	Reset_Handler         /* Reset Handler */
#if defined(CONFIG_WATCHPOINT)
	.rept   6
	.long   Default_Handler
	.endr
	.long   watchpoint_handler    /* watchpoint Handler */
	.rept   8
#else
	.rept   15
#endif
	.long   Default_Handler
	.endr
#ifdef CONFIG_KERNEL_FREERTOS
#ifndef CONFIG_HAVE_VIC
	.long   CKTrap0ISR
#else
	.long   Default_Handler
#endif
#else
	.long  Default_Handler
#endif
	.rept   5
	.long   Default_Handler
	.endr
#if defined (CONFIG_KERNEL_UCOS) || defined (CONFIG_KERNEL_RHINO)
#ifdef CONFIG_HAVE_VIC
	.long   tspend_handler
	.rept   9
	.long   Default_Handler
	.endr
#else
	.rept   10
	.long   Default_Handler
	.endr
#endif
#elif defined CONFIG_KERNEL_FREERTOS
#ifdef CONFIG_HAVE_VIC
	.long   CKPendSVIsr
	.rept   9
	.long   Default_Handler
	.endr
#else
	.rept   10
	.long   Default_Handler
	.endr
#endif
#else
	.rept   10
	.long   Default_Handler
	.endr
#endif

#ifdef CONFIG_KERNEL_NONE
	/* External interrupts */
	.long	GPIOA_IRQHandler         /*  0:  GPIOA          */
	.long	CORET_IRQHandler         /*  1:  core Timer     */
	.long	TIMA0_IRQHandler         /*  2:  TimerA0         */
	.long	TIMA1_IRQHandler         /*  3:  TimerA1         */
	.long	Default_Handler
	.long	WDT_IRQHandler           /*  5:  WDT            */
	.long	USART0_IRQHandler        /*  6:  UART0          */
	.long	USART1_IRQHandler        /*  7:  UART1          */
	.long	USART2_IRQHandler        /*  8:  UART2          */
	.long	I2C0_IRQHandler          /*  9:  I2C0           */
	.long	I2C1_IRQHandler          /*  10: I2C1           */
	.long	SPI1_IRQHandler          /*  11: SPI1           */
	.long	SPI0_IRQHandler          /*  12: SPI0           */
	.long	RTC_IRQHandler           /*  13: RTC            */
	.long	Default_Handler
	.long	Default_Handler
	.long	Default_Handler
	.long	DMAC_IRQHandler          /*  17: DMAC           */
	.long	Default_Handler
	.long	PWM_IRQHandler           /*  19: PWM            */
	.long	Default_Handler
	.long	USART3_IRQHandler        /*  21: UART3          */
	.long	Default_Handler
	.long	TIMB0_IRQHandler         /*  23: TimerB0        */
	.long	TIMB1_IRQHandler         /*  24: TimerB1        */
	.long	Default_Handler
	.long   AES_IRQHandler           /*  26:  AES            */
	.long	GPIOB_IRQHandler         /*  27:  GPIOB          */
	.long	Default_Handler
	.long   SHA_IRQHandler           /*  29:  SHA            */
	.long   Default_Handler
	.long   Default_Handler
#else
#ifdef CONFIG_HAVE_VIC
	/* External interrupts */
	.long   GPIOA_IRQHandler        /*  0:  GPIOA          */
	.long   CORET_IRQHandler        /*  1:  core Timer     */
	.long   TIMA0_IRQHandler         /*  2:  TimerA0         */
	.long   TIMA1_IRQHandler         /*  3:  TimerA1         */
	.long   Default_Handler
	.long   WDT_IRQHandler           /*  5:  WDT            */
	.long   USART0_IRQHandler        /*  6:  UART0          */
	.long   USART1_IRQHandler        /*  7:  UART1          */
	.long   USART2_IRQHandler        /*  8:  UART2          */
	.long   I2C0_IRQHandler          /*  9:  I2C0           */
	.long   I2C1_IRQHandler          /*  10: I2C1           */
	.long   SPI1_IRQHandler          /*  11: SPI1           */
	.long   SPI0_IRQHandler          /*  12: SPI0           */
	.long   RTC_IRQHandler           /*  13: RTC            */
	.long   Default_Handler
	.long   Default_Handler
	.long   Default_Handler
	.long   DMAC_IRQHandler          /*  17: DMAC           */
	.long   Default_Handler
	.long   PWM_IRQHandler           /*  19: PWM            */
	.long   Default_Handler
	.long   USART3_IRQHandler        /*  21: UART3          */
	.long   Default_Handler
	.long   TIMB0_IRQHandler         /*  23: TimerB0        */
	.long   TIMB1_IRQHandler         /*  24: TimerB1        */
	.long   Default_Handler
	.long   AES_IRQHandler           /*  26:  AES            */
	.long   GPIOB_IRQHandler         /*  27:  GPIOB          */
	.long   Default_Handler
	.long   SHA_IRQHandler           /*  29:  SHA            */
	.long   Default_Handler
	.long   Default_Handler
#else
#ifdef CONFIG_KERNEL_FREERTOS
        .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
        .long   NOVIC_CORETIM_Handler        /*  novic coretime handler specific*/
        .rept   30
        .long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
        .endr
#else
	.rept   32
	.long   NOVIC_IRQ_Default_Handler    /*  default interrupt entry  */
	.endr
#endif
#endif
#endif
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

#ifdef CONFIG_SEPARATE_IRQ_SP
	lrw    r0, g_top_irqstack
	mtcr   r0, cr<15, 1>

	mfcr   r0, cr<31, 0>
	bseti  r0, 14
	mtcr   r0, cr<31, 0>

#ifdef CONFIG_STACK_GUARD
	lrw    r0, g_intstackbase
	mtcr   r0, cr<7, 4>
	lrw    r0, g_top_irqstack
	mtcr   r0, cr<6, 4>
#endif /* CONFIG_STACK_GUARD */
#endif /* CONFIG_SEPARATE_IRQ_SP */

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

#ifdef CONFIG_HAVE_VIC
	lrw    r0, VIC_TSPR
	movi   r1, 0xb00
	stw    r1, (r0)
#endif

	bsr	main
	.size	Reset_Handler, . - Reset_Handler

__exit:
	bkpt

	.align  1
	.weak   Default_Handler
	.type   Default_Handler, %function
Default_Handler:
#ifndef CONFIG_KERNEL_NONE
	br      trap
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
	.macro	def_irq_handler	handler_name
	.weak	\handler_name
	.set	\handler_name, Default_Handler
	.endm

	def_irq_handler	CORET_IRQHandler
	def_irq_handler TIMA0_IRQHandler
	def_irq_handler TIMA1_IRQHandler
	def_irq_handler TIMB0_IRQHandler
	def_irq_handler TIMB1_IRQHandler
	def_irq_handler	USART0_IRQHandler
	def_irq_handler	USART1_IRQHandler
	def_irq_handler	USART2_IRQHandler
	def_irq_handler	USART3_IRQHandler
	def_irq_handler	GPIOA_IRQHandler
	def_irq_handler	GPIOB_IRQHandler
	def_irq_handler	I2C0_IRQHandler
	def_irq_handler	I2C1_IRQHandler
	def_irq_handler	SPI0_IRQHandler
	def_irq_handler	SPI1_IRQHandler
	def_irq_handler	RTC_IRQHandler
	def_irq_handler	WDT_IRQHandler
	def_irq_handler	PWM_IRQHandler
	def_irq_handler	DMAC_IRQHandler
	def_irq_handler AES_IRQHandler
	def_irq_handler SHA_IRQHandler

	.end
