/* File: startup_ARMCM3.S
 * Purpose: startup file for Cortex-M3 devices. Should use with
 *   GCC for ARM Embedded Processors
 * Version: V2.0
 * Date: 16 August 2013
 *
/* Copyright (c) 2011 - 2013 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/
	.syntax	unified
	.arch	armv7-m

	.section .stack
	.align	3
#ifdef __STACK_SIZE
	.equ	Stack_Size, __STACK_SIZE
#else
	.equ	Stack_Size, 0xc00
#endif
	.globl	__StackTop
	.globl	__StackLimit
__StackLimit:
	.space	Stack_Size
	.size	__StackLimit, . - __StackLimit
__StackTop:
	.size	__StackTop, . - __StackTop

	.section .heap
	.align	3
#ifdef __HEAP_SIZE
	.equ	Heap_Size, __HEAP_SIZE
#else
	.equ	Heap_Size, 0x1B000
#endif
	.globl	__HeapBase
	.globl	__HeapLimit
__HeapBase:
	.if	Heap_Size
	.space	Heap_Size
	.endif
	.size	__HeapBase, . - __HeapBase
__HeapLimit:
	.size	__HeapLimit, . - __HeapLimit

	.section .isr_vector
	.align	2
	.globl	__isr_vector
__isr_vector:
	.word	__StackTop            /* Top of Stack */
	.word	Reset_Handler         /* Reset Handler */
	.word	NMI_Handler           /* NMI Handler */
	.word	HardFault_Handler     /* Hard Fault Handler */
	.word	MemManage_Handler     /* MPU Fault Handler */
	.word	BusFault_Handler      /* Bus Fault Handler */
	.word	UsageFault_Handler    /* Usage Fault Handler */
	.word	0                     /* Reserved */
	.word	0                     /* Reserved */
	.word	0                     /* Reserved */
	.word	0                     /* Reserved */
	.word	SVC_Handler           /* SVCall Handler */
	.word	DebugMon_Handler      /* Debug Monitor Handler */
	.word	0                     /* Reserved */
	.word	PendSV_Handler        /* PendSV Handler */
	.word	OS_CPU_SysTickHandler /* SysTick Handler */

	/* External interrupts */
	.word   SDIO_RX_IRQHandler        
    .word   SDIO_TX_IRQHandler        
    .word   SDIO_RX_CMD_IRQHandler    
    .word   SDIO_TX_CMD_IRQHandler    
    .word   tls_wl_mac_isr          
    .word   Default_Handler           
    .word   tls_wl_rx_isr           
    .word   tls_wl_mgmt_tx_isr        
    .word   tls_wl_data_tx_isr
    .word   PMU_TIMER1_IRQHandler     
    .word   PMU_TIMER0_IRQHandler     
    .word   PMU_GPIO_WAKE_IRQHandler  
    .word   PMU_SDIO_WAKE_IRQHandler  
    .word   DMA_Channel0_IRQHandler   
    .word   DMA_Channel1_IRQHandler   
    .word   DMA_Channel2_IRQHandler   
    .word   DMA_Channel3_IRQHandler   
    .word   DMA_Channel4_7_IRQHandler 
    .word   DMA_BRUST_IRQHandler      
    .word   I2C_IRQHandler
    .word   ADC_IRQHandler
    .word   SPI_LS_IRQHandler      
    .word   SPI_HS_IRQHandler      
    .word   UART0_IRQHandler       
    .word   UART1_IRQHandler       
    .word   GPIOA_IRQHandler       
    .word   TIM0_IRQHandler
    .word   TIM1_IRQHandler        
    .word   TIM2_IRQHandler        
    .word   TIM3_IRQHandler        
    .word   TIM4_IRQHandler        
    .word   TIM5_IRQHandler        
    .word   WDG_IRQHandler         
    .word   PMU_IRQHandler         
    .word   FLASH_IRQHandler       
    .word   PWM_IRQHandler         
    .word   I2S_IRQHandler         
    .word   PMU_RTC_IRQHandler     
    .word   RSA_IRQHandler         
    .word   CRYPTION_IRQHandler    
    .word   GPIOB_IRQHandler       
    .word   UART2_IRQHandler       

	.size	__isr_vector, . - __isr_vector

	.text
	.thumb
	.thumb_func
	.align	2
	.globl	Reset_Handler
	.type	Reset_Handler, %function
Reset_Handler:

/*  Firstly it copies data from read only memory to RAM.
 *
 *  The ranges of copy from/to are specified by following symbols
 *    __etext: LMA of start of the section to copy from. Usually end of text
 *    __data_start__: VMA of start of the section to copy to
 *    __data_end__: VMA of end of the section to copy to
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
	ldr	r1, =__etext
	ldr	r2, =__data_start__
	ldr	r3, =__data_end__

.L_loop1:
	cmp	r2, r3
	ittt	lt
	ldrlt	r0, [r1], #4
	strlt	r0, [r2], #4
	blt	.L_loop1

/*  Single BSS section scheme.
 *
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */
	ldr	r1, =__bss_start__
	ldr	r2, =__bss_end__

	movs	r0, 0
.L_loop2:
	cmp	r1, r2
	itt	lt
	strlt	r0, [r1], #4
	blt	.L_loop2

#ifndef __NO_SYSTEM_INIT
	 bl	SystemInit 
#endif

	bl	main

	.pool
	.size	Reset_Handler, . - Reset_Handler

	.align	1
	.thumb_func
	.weak	Default_Handler
	.type	Default_Handler, %function
Default_Handler:
	b	.
	.size	Default_Handler, . - Default_Handler

/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
	.macro	def_irq_handler	handler_name
	.weak	\handler_name
	.set	\handler_name, Default_Handler
	.endm

	def_irq_handler	NMI_Handler
	def_irq_handler	HardFault_Handler
	def_irq_handler	MemManage_Handler
	def_irq_handler	BusFault_Handler
	def_irq_handler	UsageFault_Handler
	def_irq_handler	SVC_Handler
	def_irq_handler	DebugMon_Handler
	def_irq_handler	PendSV_Handler
	def_irq_handler	OS_CPU_SysTickHandler
	
	def_irq_handler SDIO_RX_IRQHandler       
    def_irq_handler SDIO_TX_IRQHandler       
    def_irq_handler SDIO_RX_CMD_IRQHandler   
    def_irq_handler SDIO_TX_CMD_IRQHandler   
    def_irq_handler tls_wl_mac_isr           
    def_irq_handler tls_wl_rx_isr           
    def_irq_handler tls_wl_mgmt_tx_isr       
    def_irq_handler tls_wl_data_tx_isr       
    def_irq_handler PMU_TIMER1_IRQHandler    
    def_irq_handler PMU_TIMER0_IRQHandler    
    def_irq_handler PMU_GPIO_WAKE_IRQHandler 
    def_irq_handler PMU_SDIO_WAKE_IRQHandler 
    def_irq_handler DMA_Channel0_IRQHandler  
    def_irq_handler DMA_Channel1_IRQHandler  
    def_irq_handler DMA_Channel2_IRQHandler  
    def_irq_handler DMA_Channel3_IRQHandler  
    def_irq_handler DMA_Channel4_7_IRQHandler
    def_irq_handler DMA_BRUST_IRQHandler     
    def_irq_handler I2C_IRQHandler  		
    def_irq_handler ADC_IRQHandler 			
    def_irq_handler SPI_LS_IRQHandler        
    def_irq_handler SPI_HS_IRQHandler        
    def_irq_handler UART0_IRQHandler         
    def_irq_handler UART1_IRQHandler        
    def_irq_handler GPIOA_IRQHandler         
    def_irq_handler TIM0_IRQHandler    		
    def_irq_handler TIM1_IRQHandler         
    def_irq_handler TIM2_IRQHandler          
    def_irq_handler TIM3_IRQHandler          
    def_irq_handler TIM4_IRQHandler          
    def_irq_handler TIM5_IRQHandler         
    def_irq_handler WDG_IRQHandler        	 
    def_irq_handler PMU_IRQHandler         	
    def_irq_handler FLASH_IRQHandler         
    def_irq_handler PWM_IRQHandler           
    def_irq_handler I2S_IRQHandler           
    def_irq_handler PMU_RTC_IRQHandler       
    def_irq_handler RSA_IRQHandler          
    def_irq_handler CRYPTION_IRQHandler      
    def_irq_handler GPIOB_IRQHandler       
    def_irq_handler UART2_IRQHandler        

	.end
