/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

  .syntax unified
#ifdef __CONFIG_CPU_CM4F
  .cpu cortex-m4
  .fpu softvfp
#else
  .cpu cortex-m3
#endif
  .thumb

.global g_pfnVectors
.global Default_Handler

/* start address for the initialization values of the .data section.
defined in linker script */
.word _sidata
/* start address for the .data section. defined in linker script */
.word _sdata
/* end address for the .data section. defined in linker script */
.word _edata
/* start address for the .bss section. defined in linker script */
.word _sbss
/* end address for the .bss section. defined in linker script */
.word _ebss

	.equ NVIC_SCR,                          (0xe000e000 + 0xd10)
	.equ NVIC_SYSTICK_CTRL,                 (0xe000e000 + 0x010)
	.equ NVIC_ICSR,                         (0xe000e000 + 0xd04)
	.equ GPRCM_CPUA_BOOT_FLAG,              (0x40040000 + 0x100)
	.equ GPRCM_CPUA_BOOT_ADDR,              (0x40040000 + 0x104)
	.equ GPRCM_CPUA_BOOT_ARG,               (0x40040000 + 0x108)
	.equ GPRCM_CPUA_BOOT_FLAG_MASK,         (3)
	.equ GPRCM_CPUA_BOOT_FLAG_WAKEUP,       (1)
	.equ GPRCM_SYSCLK1_CTRLS,               (0x40040000 + 0x024)
	.equ GPRCM_SYSCLK1_EN_DIV_MASK,         (0x8000000f)
	.equ GPRCM_SYSCLK1_LFCLK,               (0x00010000)

/*.equ  BootRAM, 0xF108F85F*/
/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called.
 * @param  None
 * @retval : None
*/

  .section .text.Reset_Handler
  .weak Reset_Handler
  .type Reset_Handler, %function
  .size Reset_Handler, .-Reset_Handler
Reset_Handler:
  ldr   r0, =_estack
  mov   sp, r0          /* set stack pointer */

  cpsie i
  cpsie f
  bl _start

LoopForever:
  b LoopForever


.size Reset_Handler, .-Reset_Handler

/**
 * @brief  This is the code that gets called when the processor receives an
 *         unexpected interrupt.  This simply enters an infinite loop, preserving
 *         the system state for examination by a debugger.
 *
 * @param  None
 * @retval : None
*/
  .extern exception_entry

    .section .cpu_text,"ax",%progbits
Default_Handler:
#ifndef __CONFIG_BOOTLOADER
  CPSID F
  TST LR, #0x04
  ITE EQ
  MRSEQ R0, MSP
  MRSNE R0, PSP
  STMDB.W R0!, {R4-R11}
  MRS R1, MSP
  MRS R2, PSP
  BL exception_entry
#endif
Infinite_Loop:
  b Infinite_Loop
  .size Default_Handler, .-Default_Handler

/******************************************************************************
*
* The minimal vector table for a Cortex M3.  Note that the proper constructs
* must be placed on this to ensure that it ends up at physical address
* 0x0000.0000.
*
******************************************************************************/
  .section .isr_vector,"a",%progbits
  .type g_pfnVectors, %object
  .size g_pfnVectors, .-g_pfnVectors


g_pfnVectors:

  .word _estack
  .word Reset_Handler
  .word NMI_Handler
  .word HardFault_Handler
  .word MemManage_Handler
  .word BusFault_Handler
  .word UsageFault_Handler
  .word 0
  .word 0
  .word 0
  .word 0
  .word SVC_Handler
  .word DebugMon_Handler
  .word 0
  .word PendSV_Handler
  .word SysTick_Handler

    /* External Interrupts */
  .word		DMA_IRQHandler				// 16
#ifndef __CONFIG_BOOTLOADER
  .word		GPIOA_IRQHandler
  .word		SDC_IRQHandler
  .word		MBOX_A_IRQHandler
  .word		UART0_IRQHandler			// 20
  .word		UART1_IRQHandler
  .word		SPI0_IRQHandler
  .word		SPI1_IRQHandler
  .word		TWI0_IRQHandler
  .word		TWI1_IRQHandler
  .word		WDG_IRQHandler
  .word		TIMER0_IRQHandler
  .word		TIMER1_IRQHandler
  .word		RTC_SecAlarm_IRQHandler
  .word		RTC_WDayAlarm_IRQHandler			// 30
  .word		CSI_IRQHandler
  .word		I2S_IRQHandler
  .word		PWM_ECT_IRQHandler
  .word		CE_IRQHandler
  .word		GPADC_IRQHandler
  .word		GPIOB_IRQHandler
  .word		DMIC_IRQHandler
  .word		IRRX_IRQHandler
  .word		IRTX_IRQHandler
  .word		MBOX_N_IRQHandler			// 40
  .word		0
  .word		0
  .word		N_UART_IRQHandler
#else
  .word		0 //GPIOA_IRQHandler
  .word		0 //SDC_IRQHandler
  .word		0 //MBOX_A_IRQHandler
  .word		UART0_IRQHandler		// 20
  .word		UART1_IRQHandler
  .word		SPI0_IRQHandler
  .word		0 //SPI1_IRQHandler
  .word		0 //TWI0_IRQHandler
  .word		0 //TWI1_IRQHandler
  .word		0 //WDG_IRQHandler
  .word		0 //TIMER0_IRQHandler
  .word		0 //TIMER1_IRQHandler
  .word		0 //RTC_SecAlarm_IRQHandler
  .word		0 //RTC_WDayAlarm_IRQHandler	// 30
  .word		0 //CSI_IRQHandler
  .word		0 //I2S_IRQHandler
  .word		0 //PWM_ECT_IRQHandler
  .word		0 //CE_IRQHandler
  .word		0 //GPADC_IRQHandler
  .word		0 //GPIOB_IRQHandler
  .word		0 //DMIC_IRQHandler
  .word		0 //IRRX_IRQHandler
  .word		0 //IRTX_IRQHandler
  .word		0 //MBOX_N_IRQHandler 		// 40
  .word		0
  .word		0
  .word		0
#endif
  .word		0
  .word		0
  .word		0
  .word		0
  .word		0
  .word		0
  .word		0							// 50
  .word		0
  .word		0
  .word		0
  .word		0
  .word		0
  .word		0
  .word		0
  .word		0
  .word		0
  .word		0							// 60
  .word		0
  .word		0
  .word		0


/*******************************************************************************
*
* Provide weak aliases for each Exception handler to the Default_Handler.
* As they are weak aliases, any function with the same name will override
* this definition.
*
*******************************************************************************/

  .weak NMI_Handler
  .thumb_set NMI_Handler,Default_Handler

  .weak HardFault_Handler
  .thumb_set HardFault_Handler,Default_Handler

  .weak MemManage_Handler
  .thumb_set MemManage_Handler,Default_Handler

  .weak BusFault_Handler
  .thumb_set BusFault_Handler,Default_Handler

  .weak UsageFault_Handler
  .thumb_set UsageFault_Handler,Default_Handler

  .weak SVC_Handler
  .thumb_set SVC_Handler,Default_Handler

  .weak DebugMon_Handler
  .thumb_set DebugMon_Handler,Default_Handler

//  .weak PendSV_Handler
//  .thumb_set PendSV_Handler,Default_Handler

//  .weak SysTick_Handler
//  .thumb_set SysTick_Handler,Default_Handler


  .weak      DMA_IRQHandler
  .thumb_set DMA_IRQHandler,Default_Handler

  .weak      GPIOA_IRQHandler
  .thumb_set GPIOA_IRQHandler,Default_Handler

  .weak      SDC_IRQHandler
  .thumb_set SDC_IRQHandler,Default_Handler

  .weak      MBOX_A_IRQHandler
  .thumb_set MBOX_A_IRQHandler,Default_Handler

  .weak      MBOX_N_IRQHandler
  .thumb_set MBOX_N_IRQHandler,Default_Handler

  .weak      UART0_IRQHandler
  .thumb_set UART0_IRQHandler,Default_Handler

  .weak      UART1_IRQHandler
  .thumb_set UART1_IRQHandler,Default_Handler

  .weak      SPI0_IRQHandler
  .thumb_set SPI0_IRQHandler,Default_Handler

  .weak      SPI1_IRQHandler
  .thumb_set SPI1_IRQHandler,Default_Handler

  .weak      TWI0_IRQHandler
  .thumb_set TWI0_IRQHandler,Default_Handler

  .weak      TWI1_IRQHandler
  .thumb_set TWI1_IRQHandler,Default_Handler

  .weak      WDG_IRQHandler
  .thumb_set WDG_IRQHandler,Default_Handler

  .weak      TIMER0_IRQHandler
  .thumb_set TIMER0_IRQHandler,Default_Handler

  .weak      TIMER1_IRQHandler
  .thumb_set TIMER1_IRQHandler,Default_Handler

  .weak      RTC_SecAlarm_IRQHandler
  .thumb_set RTC_SecAlarm_IRQHandler,Default_Handler

  .weak      RTC_WDayAlarm_IRQHandler
  .thumb_set RTC_WDayAlarm_IRQHandler,Default_Handler

  .weak      CSI_IRQHandler
  .thumb_set CSI_IRQHandler,Default_Handler

  .weak      I2S_IRQHandler
  .thumb_set I2S_IRQHandler,Default_Handler

  .weak      PWM_ECT_IRQHandler
  .thumb_set PWM_ECT_IRQHandler,Default_Handler

  .weak      CE_IRQHandler
  .thumb_set CE_IRQHandler,Default_Handler

  .weak      GPADC_IRQHandler
  .thumb_set GPADC_IRQHandler,Default_Handler

  .weak      GPIOC_IRQHandler
  .thumb_set GPIOC_IRQHandler,Default_Handler

  .weak      DMIC_IRQHandler
  .thumb_set DMIC_IRQHandler,Default_Handler

  .weak      IRRX_IRQHandler
  .thumb_set IRRX_IRQHandler,Default_Handler

  .weak      IRTX_IRQHandler
  .thumb_set IRTX_IRQHandler,Default_Handler

  .weak      N_UART_IRQHandler
  .thumb_set N_UART_IRQHandler,Default_Handler

  .weak      N_SPI_IRQHandler
  .thumb_set N_SPI_IRQHandler,Default_Handler

  .weak      N_WDT_IRQHandler
  .thumb_set N_WDT_IRQHandler,Default_Handler

  .weak      N_TIMER0_IRQHandler
  .thumb_set N_TIMER0_IRQHandler,Default_Handler

  .weak      N_TIMER1_IRQHandler
  .thumb_set N_TIMER1_IRQHandler,Default_Handler

  .weak      N_SDC_IRQHandler
  .thumb_set N_SDC_IRQHandler,Default_Handler

  .weak      N_WIFIC_IRQHandler
  .thumb_set N_WIFIC_IRQHandler,Default_Handler

  .weak      WKUP_TIMER0_IRQHandler
  .thumb_set WKUP_TIMER0_IRQHandler,Default_Handler

  .weak      WKUP_TIMER1_IRQHandler
  .thumb_set WKUP_TIMER1_IRQHandler,Default_Handler

  /*------------------ void __cpu_sleep(int nouse) ------------------------*/
#ifndef __CONFIG_BOOTLOADER

  .thumb_func
  .section .cpu_text
  .type	__cpu_sleep, %function
  .global __cpu_sleep

__cpu_sleep:
  .fnstart
  .cantunwind

  PUSH {R0-R12, LR}

  /* clear pending and disable systick */
  LDR R2, =NVIC_SYSTICK_CTRL
  LDR R5, [R2]
  MOV R3, R5
  BIC R3, #0x03
  STR R3, [R2]

  LDR R2, =NVIC_ICSR
  LDR R1, [R2]
  AND R1, #0x4000000
  ASR R1, #1
  STR R1, [R2]

  /* switch to 24M/div */
  LDR R1, =GPRCM_SYSCLK1_CTRLS
  LDR R0, [R1]
  BIC R0, R0, #0x30000
  ORR R0, R0, #0x10000
  STR R0, [R1]
  DSB
  ISB

  WFI
  WFI
  WFI

  /* switch cpu clk to pll */
  LDR R1, =GPRCM_SYSCLK1_CTRLS
  LDR R0, [R1]
  BIC R0, R0, #0x30000
  ORR R0, R0, #0x20000
  STR R0, [R1]
  DSB
  ISB

  /* enable systick */
  LDR R2, =NVIC_SYSTICK_CTRL
  STR R5, [R2]

  POP {R0-R12, PC}

  .fnend
  .size   __cpu_sleep, .-__cpu_sleep


  /*------------------ void __cpu_suspend(int nouse) ------------------------*/
  .global check_wakeup_irqs

  .thumb_func
  .section .cpu_text
  .type   __cpu_suspend, %function
  .global __cpu_suspend

__cpu_suspend:
  .fnstart
  .cantunwind

  PUSH {R0-R12, LR}
  ISB

  LDR R0, =GPRCM_CPUA_BOOT_ARG
  ISB
  LDR R1, [R0]
  MRS R0, MSP
  ISB
  STR R0, [R1]

  MRS R0, PSP
  ISB
  STR R0, [R1, #4]

  MRS R0, PRIMASK
  STR R0, [R1, #12]

  MRS R0, FAULTMASK
  STR R0, [R1, #16]

  MRS R0, BASEPRI
  STR R0, [R1, #20]

  MRS R0, CONTROL
  STR R0, [R1, #24]

  /* set deepsleep mode */
  LDR R0, =0x14
  LDR R1, =NVIC_SCR
  ISB
  STR R0, [R1]

  /* set bootflag */
  LDR R0, =0x429b0001
  LDR R1, =GPRCM_CPUA_BOOT_FLAG
  ISB
  STR R0, [R1]

  /* set resume address in thumb state */
  LDR R0, =resume
  ORR.W R0, R0, #1
  LDR R1, =GPRCM_CPUA_BOOT_ADDR
  ISB
  STR R0, [R1]

  /* switch to 24M/div */
  LDR R1, =GPRCM_SYSCLK1_CTRLS
  ISB
  LDR R0, [R1]
  BIC R0, R0, #0x30000
  ORR R0, R0, #0x10000
  STR R0, [R1]
  DSB
  ISB
  NOP
  NOP
  NOP

  /* the WFE instruction will cause two kinds of CPU actions:
   * 1. EVNET_REGISTER = 1, WFE will clear the EVENT_REGISTER and the
   *	 CPU executes the next instruction.
   * 2. EVENT_REGISTER = 0, WFE will make the CPU go to SLEEP state.
   */
  /* first time executing WFE instruction, there are some different
   * situations as follows:
   * 1. if there are interrupts pending and be cleared already,
   *	 the WFE will only clear the CPU EVENT_REGISTER.
   * 2. if there are new interrupts pending after ar400_deepsleep_lock
   *	 operation, the WFE will only clear the CPU EVENT_REGISTER.
   * 3. if the SEV/NMI/DEBUG events coming before now, WFE will only
   *	 clear the CPU EVENT_REGISTER.
   * 4. if there are no SEV/NMI/DEBUG events before and no interrupts
   *	 pending too, WFE wil make the CPU go to the SLEEP state.
   */
  WFE

  /* read the NVIC SET_PENDING_REGISTER to check whether there are
   *  any new pending interrupts after ar400_deepsleep_lock operation
   *  which make the first WFE executing failed.
   * 1. If ther are some new pending interrupts, jump to the RESUME_ENTRY
   *	 and abandon the next WFE execution.
   * 2. If there is no new pending interrupts, we execute WFE instruction
   *	 twice to ensure the CPU goes to SLEEP state successfully.
   */
  BL check_wakeup_irqs
  CMP R0, #0
  BNE resume
  ISB
  NOP
  NOP
  NOP

  WFE
  NOP

resume:
  /* switch cpu clk to pll */
  LDR R1, =GPRCM_SYSCLK1_CTRLS
  ISB
  LDR R0, [R1]
  BIC R0, R0, #0x30000
  ORR R0, R0, #0x20000
  STR R0, [R1]
  DSB
  ISB

  /* needn't remove address */

  /* remove bootflag */
  LDR R0, =0x429b0000
  LDR R1, =GPRCM_CPUA_BOOT_FLAG
  ISB
  STR R0, [R1]

  /* set normal mode */
  MOV R0, 0
  LDR R1, =NVIC_SCR
  ISB
  STR R0, [R1]

  /* restore cpu contex */
  LDR R0, =GPRCM_CPUA_BOOT_ARG
  ISB
  LDR R1, [R0]

  LDR R0, [R1]
  MSR MSP, R0
  ISB

  LDR R0, [R1,#4]
  MSR PSP, R0
  ISB

  LDR R0, [R1, #12]
  MSR PRIMASK, R0

  LDR R0, [R1, #16]
  MSR FAULTMASK, R0

  LDR R0, [R1, #20]
  MSR BASEPRI, R0

  LDR R0, [R1, #24]
  MSR CONTROL, R0
  ISB
  NOP

  POP {R0-R12, PC}
  NOP

  .fnend
  .size   __cpu_suspend, .-__cpu_suspend

#endif
