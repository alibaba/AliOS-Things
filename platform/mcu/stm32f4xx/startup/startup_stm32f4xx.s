/**
  ******************************************************************************
  * @file      startup_stm32f4xx.s
  * @author    MCD Application Team
  * @version   V1.3.0
  * @date      17-February-2017
  * @brief     STM32L475xx devices vector table for GCC toolchain.
  *            This module performs:
  *                - Set the initial SP
  *                - Set the initial PC == Reset_Handler,
  *                - Set the vector table entries with the exceptions ISR address,
  *                - Configure the clock system  
  *                - Branches to main in the C library (which eventually
  *                  calls main()).
  *            After Reset the Cortex-M4 processor is in Thread mode,
  *            priority is Privileged, and the Stack is set to Main.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

  .syntax unified
	.cpu cortex-m4
	.fpu softvfp
	.thumb

.global	g_pfnVectors
.globl NMI_Handler
.globl HardFault_Handler
.globl MemManage_Handler
.globl BusFault_Handler
.globl UsageFault_Handler
.globl SVC_Handler
.globl DebugMon_Handler
.globl PendSV_Handler
.globl SysTick_Handler
.globl WWDG_IRQHandler
.globl PVD_IRQHandler
.globl TAMP_STAMP_IRQHandler
.globl RTC_WKUP_irq
.globl FLASH_IRQHandler
.globl RCC_IRQHandler
.globl EXTI0_IRQHandler
.globl EXTI1_IRQHandler
.globl EXTI2_IRQHandler
.globl EXTI3_IRQHandler
.globl EXTI4_IRQHandler
.globl DMA1_Stream0_IRQHandler
.globl DMA1_Stream1_IRQHandler
.globl DMA1_Stream2_IRQHandler
.globl DMA1_Stream3_IRQHandler
.globl DMA1_Stream4_IRQHandler
.globl DMA1_Stream5_IRQHandler
.globl DMA1_Stream6_IRQHandler
.globl ADC_IRQHandler
.globl CAN1_TX_IRQHandler
.globl CAN1_RX0_IRQHandler
.globl CAN1_RX1_IRQHandler
.globl CAN1_SCE_IRQHandler
.globl EXTI9_5_IRQHandler
.globl TIM1_BRK_TIM9_IRQHandler
.globl TIM1_UP_TIM10_IRQHandler
.globl TIM1_TRG_COM_TIM11_IRQHandler
.globl TIM1_CC_IRQHandler
.globl TIM2_IRQHandler
.globl TIM3_IRQHandler
.globl TIM4_IRQHandler
.globl I2C1_EV_IRQHandler
.globl I2C1_ER_IRQHandler
.globl I2C2_EV_IRQHandler
.globl I2C2_ER_IRQHandler
.globl SPI1_IRQHandler
.globl SPI2_IRQHandler
.globl USART1_IRQHandler
.globl USART2_IRQHandler
.globl USART3_IRQHandler
.globl EXTI15_10_IRQHandler
.globl RTC_Alarm_IRQHandler
.globl OTG_FS_WKUP_IRQHandler
.globl TIM8_BRK_TIM12_IRQHandler
.globl TIM8_UP_TIM13_IRQHandler
.globl TIM8_TRG_COM_TIM14_IRQHandler
.globl TIM8_CC_IRQHandler
.globl DMA1_Stream7_IRQHandler
.globl FMC_IRQHandler
.globl SDIO_IRQHandler
.globl TIM5_IRQHandler
.globl SPI3_IRQHandler
.globl DMA2_Stream0_IRQHandler      
.globl DMA2_Stream1_IRQHandler      
.globl DMA2_Stream2_IRQHandler      
.globl DMA2_Stream3_IRQHandler      
.globl DMA2_Stream4_IRQHandler 
.globl OTG_FS_IRQHandler
.globl DMA2_Stream5_IRQHandler
.globl DMA2_Stream6_IRQHandler
.globl DMA2_Stream7_IRQHandler
.globl USART6_IRQHandler
.globl I2C3_EV_IRQHandler
.globl I2C3_ER_IRQHandler
.globl FPU_IRQHandler                                                        
.globl SPI4_IRQHandler              
.globl SPI5_IRQHandler                                                                                                                                                                                      
.globl QUADSPI_IRQHandler                                                                   
.globl FMPI2C1_EV_IRQHandler        
.globl FMPI2C1_ER_IRQHandler   
 
/* start address for the initialization values of the .data section.
defined in linker script */
.word	_sidata
/* start address for the .data section. defined in linker script */
.word	_sdata
/* end address for the .data section. defined in linker script */
.word	_edata
/* start address for the .bss section. defined in linker script */
.word	_sbss
/* end address for the .bss section. defined in linker script */
.word	_ebss

.equ  BootRAM,        0xF1E0F85F
/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called.
 * @param  None
 * @retval : None
*/

    .section	.text.Reset_Handler
	.weak	Reset_Handler
	.type	Reset_Handler, %function
Reset_Handler:
  ldr   sp, =_estack    /* Atollic update: set stack pointer */

/* Copy the data segment initializers from flash to SRAM */
  movs	r1, #0
  b	LoopCopyDataInit

CopyDataInit:
	ldr	r3, =_sidata
	ldr	r3, [r3, r1]
	str	r3, [r0, r1]
	adds	r1, r1, #4

LoopCopyDataInit:
	ldr	r0, =_sdata
	ldr	r3, =_edata
	adds	r2, r0, r1
	cmp	r2, r3
	bcc	CopyDataInit
	ldr	r2, =_sbss
	b	LoopFillZerobss
/* Zero fill the bss segment. */
FillZerobss:
	movs	r3, #0
	str	r3, [r2], #4

LoopFillZerobss:
	ldr	r3, = _ebss
	cmp	r2, r3
	bcc	FillZerobss

/* Call static constructors */
#    bl __libc_init_array
/* Call the application's entry point.*/
	b	entry_main
/**  not need to enter loop
LoopForever:
    b LoopForever
**/    
.size	Reset_Handler, .-Reset_Handler

/******************************************************************************
*
* The minimal vector table for a Cortex-M4.  Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
*
******************************************************************************/
 	.section	.isr_vector,"a",%progbits
	.type	g_pfnVectors, %object
	.size	g_pfnVectors, .-g_pfnVectors


g_pfnVectors:
	.word	_estack
	.word	Reset_Handler
	.word	NMI_Handler
	.word	HardFault_Handler
	.word	MemManage_Handler
	.word	BusFault_Handler
	.word	UsageFault_Handler
	.word	0
	.word	0
	.word	0
	.word	0
	.word	SVC_Handler
	.word	DebugMon_Handler
	.word	0
	.word	PendSV_Handler
	.word	SysTick_Handler
	.word	WWDG_IRQHandler
	.word	PVD_IRQHandler
	.word	TAMP_STAMP_IRQHandler
	.word	RTC_WKUP_irq
	.word	FLASH_IRQHandler
	.word	RCC_IRQHandler
	.word	EXTI0_IRQHandler
	.word	EXTI1_IRQHandler
	.word	EXTI2_IRQHandler
	.word	EXTI3_IRQHandler
	.word	EXTI4_IRQHandler
	.word	DMA1_Stream0_IRQHandler
	.word	DMA1_Stream1_IRQHandler
	.word	DMA1_Stream2_IRQHandler
	.word	DMA1_Stream3_IRQHandler
	.word	DMA1_Stream4_IRQHandler
	.word	DMA1_Stream5_IRQHandler
	.word	DMA1_Stream6_IRQHandler
	.word	ADC_IRQHandler
	.word	CAN1_TX_IRQHandler
	.word	CAN1_RX0_IRQHandler
	.word	CAN1_RX1_IRQHandler
	.word	CAN1_SCE_IRQHandler
	.word	EXTI9_5_IRQHandler
	.word	TIM1_BRK_TIM9_IRQHandler
	.word	TIM1_UP_TIM10_IRQHandler
	.word	TIM1_TRG_COM_TIM11_IRQHandler
	.word	TIM1_CC_IRQHandler
	.word	TIM2_IRQHandler
	.word	TIM3_IRQHandler
	.word	TIM4_IRQHandler
	.word	I2C1_EV_IRQHandler
	.word	I2C1_ER_IRQHandler
	.word	I2C2_EV_IRQHandler
	.word	I2C2_ER_IRQHandler
	.word	SPI1_IRQHandler
	.word	SPI2_IRQHandler
	.word	USART1_IRQHandler
	.word	USART2_IRQHandler
	.word	USART3_IRQHandler
	.word	EXTI15_10_IRQHandler
	.word	RTC_Alarm_IRQHandler
	.word	OTG_FS_WKUP_IRQHandler
	.word	TIM8_BRK_TIM12_IRQHandler
	.word	TIM8_UP_TIM13_IRQHandler
	.word	TIM8_TRG_COM_TIM14_IRQHandler
	.word	TIM8_CC_IRQHandler
	.word	DMA1_Stream7_IRQHandler
	.word	FMC_IRQHandler
	.word	SDIO_IRQHandler
	.word	TIM5_IRQHandler
	.word	SPI3_IRQHandler
	.word	0
	.word	0
	.word	0
	.word	0
	.word	DMA2_Stream0_IRQHandler      
	.word	DMA2_Stream1_IRQHandler      
	.word	DMA2_Stream2_IRQHandler      
	.word	DMA2_Stream3_IRQHandler      
	.word	DMA2_Stream4_IRQHandler 
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	OTG_FS_IRQHandler
	.word	DMA2_Stream5_IRQHandler
	.word	DMA2_Stream6_IRQHandler
	.word	DMA2_Stream7_IRQHandler
	.word	USART6_IRQHandler
	.word	I2C3_EV_IRQHandler
	.word	I2C3_ER_IRQHandler
	.word	0
	.word	0
	.word	0
	.word	0
	.word   0
	.word   0
	.word	0
	.word	FPU_IRQHandler
	.word	0                            
	.word	0                            
	.word	SPI4_IRQHandler              
	.word	SPI5_IRQHandler              
	.word	0                            
	.word	0                            
	.word	0                            
	.word	0                            
	.word	0                            
	.word	0                            
	.word	QUADSPI_IRQHandler           
	.word	0                            
	.word	0                            
	.word	FMPI2C1_EV_IRQHandler        
	.word	FMPI2C1_ER_IRQHandler   

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
