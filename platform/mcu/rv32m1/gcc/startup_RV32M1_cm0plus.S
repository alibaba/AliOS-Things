/* ---------------------------------------------------------------------------------------*/
/*  @file:    startup_RV32M1_cm0plus.s                                               */
/*  @purpose: CMSIS Cortex-M0P Core Device Startup File                                   */
/*            RV32M1_cm0plus                                                         */
/*  @version: 1.0                                                                         */
/*  @date:    2016-11-2                                                                   */
/*  @build:   b170713                                                                     */
/* ---------------------------------------------------------------------------------------*/
/*                                                                                        */
/* Copyright 1997-2016 Freescale Semiconductor, Inc.                                      */
/* Copyright 2016-2017 NXP                                                                */
/* All rights reserved.                                                                   */
/*                                                                                        */
/* SPDX-License-Identifier: BSD-3-Clause                           */
/*****************************************************************************/
/* Version: GCC for ARM Embedded Processors                                  */
/*****************************************************************************/
    .syntax unified
    .arch armv6-m

    .section .isr_vector, "a"
    .align 2
    .globl __isr_vector
__isr_vector:
    .long   __StackTop                                      /* Top of Stack */
    .long   Reset_Handler                                   /* Reset Handler */
    .long   NMI_Handler                                     /* NMI Handler*/
    .long   HardFault_Handler                               /* Hard Fault Handler*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   SVC_Handler                                     /* SVCall Handler*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   PendSV_Handler                                  /* PendSV Handler*/
    .long   SysTick_Handler                                 /* SysTick Handler*/

                                                            /* External Interrupts*/
    .long   CTI1_IRQHandler                                 /* Cross Trigger Interface 1*/
    .long   DMA1_04_IRQHandler                              /* DMA1 channel 0/4 transfer complete*/
    .long   DMA1_15_IRQHandler                              /* DMA1 channel 1/5 transfer complete*/
    .long   DMA1_26_IRQHandler                              /* DMA1 channel 2/6 transfer complete*/
    .long   DMA1_37_IRQHandler                              /* DMA1 channel 3/7 transfer complete*/
    .long   DMA1_Error_IRQHandler                           /* DMA1 channel 0-7 error interrupt*/
    .long   CMC1_IRQHandler                                 /* Core Mode Controller 1*/
    .long   LLWU1_IRQHandler                                /* Low leakage wakeup 1*/
    .long   MUB_IRQHandler                                  /* MU Side B interrupt*/
    .long   WDOG1_IRQHandler                                /* WDOG1 interrupt*/
    .long   CAU3_Task_Complete_IRQHandler                   /* Cryptographic Acceleration Unit version 3 Task Complete*/
    .long   CAU3_Security_Violation_IRQHandler              /* Cryptographic Acceleration Unit version 3 Security Violation*/
    .long   TRNG_IRQHandler                                 /* TRNG interrupt*/
    .long   LPIT1_IRQHandler                                /* LPIT1 interrupt*/
    .long   LPTMR2_IRQHandler                               /* LPTMR2 interrupt*/
    .long   TPM3_IRQHandler                                 /* TPM3 single interrupt vector for all sources*/
    .long   LPI2C3_IRQHandler                               /* LPI2C3 interrupt*/
    .long   RF0_0_IRQHandler                                /* RF0 interrupt 0*/
    .long   RF0_1_IRQHandler                                /* RF0 interrupt 1*/
    .long   LPSPI3_IRQHandler                               /* LPSPI3 single interrupt vector for all sources*/
    .long   LPUART3_IRQHandler                              /* LPUART3 status and error*/
    .long   PORTE_IRQHandler                                /* PORTE Pin detect*/
    .long   LPCMP1_IRQHandler                               /* LPCMP1 interrupt*/
    .long   RTC_IRQHandler                                  /* RTC*/
    .long   INTMUX1_0_IRQHandler                            /* INTMUX1 channel0 interrupt*/
    .long   INTMUX1_1_IRQHandler                            /* INTMUX1 channel1 interrupt*/
    .long   INTMUX1_2_IRQHandler                            /* INTMUX1 channel2 interrupt*/
    .long   INTMUX1_3_IRQHandler                            /* INTMUX1 channel3 interrupt*/
    .long   INTMUX1_4_IRQHandler                            /* INTMUX1 channel4 interrupt*/
    .long   INTMUX1_5_IRQHandler                            /* INTMUX1 channel5 interrupt*/
    .long   INTMUX1_6_IRQHandler                            /* INTMUX1 channel6 interrupt*/
    .long   INTMUX1_7_IRQHandler                            /* INTMUX1 channel7 interrupt*/
    .long   EWM_IRQHandler                                  /* EWM interrupt  (INTMUX1 source IRQ0)*/
    .long   FTFE_Command_Complete_IRQHandler                /* FTFE interrupt  (INTMUX1 source IRQ1)*/
    .long   FTFE_Read_Collision_IRQHandler                  /* FTFE interrupt  (INTMUX1 source IRQ2)*/
    .long   SPM_IRQHandler                                  /* SPM (INTMUX1 source IRQ3)*/
    .long   SCG_IRQHandler                                  /* SCG interrupt (INTMUX1 source IRQ4)*/
    .long   LPIT0_IRQHandler                                /* LPIT0 interrupt (INTMUX1 source IRQ5)*/
    .long   LPTMR0_IRQHandler                               /* LPTMR0 interrupt (INTMUX1 source IRQ6)*/
    .long   LPTMR1_IRQHandler                               /* LPTMR1 interrupt (INTMUX1 source IRQ7)*/
    .long   TPM0_IRQHandler                                 /* TPM0 single interrupt vector for all sources (INTMUX1 source IRQ8)*/
    .long   TPM1_IRQHandler                                 /* TPM1 single interrupt vector for all sources (INTMUX1 source IRQ9)*/
    .long   TPM2_IRQHandler                                 /* TPM2 single interrupt vector for all sources (INTMUX1 source IRQ10)*/
    .long   EMVSIM0_IRQHandler                              /* EMVSIM0 interrupt (INTMUX1 source IRQ11)*/
    .long   FLEXIO0_IRQHandler                              /* FLEXIO0 (INTMUX1 source IRQ12)*/
    .long   LPI2C0_IRQHandler                               /* LPI2C0 interrupt (INTMUX1 source IRQ13)*/
    .long   LPI2C1_IRQHandler                               /* LPI2C1 interrupt (INTMUX1 source IRQ14)*/
    .long   LPI2C2_IRQHandler                               /* LPI2C2 interrupt (INTMUX1 source IRQ15)*/
    .long   I2S0_IRQHandler                                 /* I2S0 interrupt (INTMUX1 source IRQ16)*/
    .long   USDHC0_IRQHandler                               /* SDHC0 interrupt (INTMUX1 source IRQ17)*/
    .long   LPSPI0_IRQHandler                               /* LPSPI0 single interrupt vector for all sources (INTMUX1 source IRQ18)*/
    .long   LPSPI1_IRQHandler                               /* LPSPI1 single interrupt vector for all sources (INTMUX1 source IRQ19)*/
    .long   LPSPI2_IRQHandler                               /* LPSPI2 single interrupt vector for all sources (INTMUX1 source IRQ20)*/
    .long   LPUART0_IRQHandler                              /* LPUART0 status and error (INTMUX1 source IRQ21)*/
    .long   LPUART1_IRQHandler                              /* LPUART1 status and error (INTMUX1 source IRQ22)*/
    .long   LPUART2_IRQHandler                              /* LPUART2 status and error (INTMUX1 source IRQ23)*/
    .long   USB0_IRQHandler                                 /* USB0 interrupt (INTMUX1 source IRQ24)*/
    .long   PORTA_IRQHandler                                /* PORTA Pin detect (INTMUX1 source IRQ25)*/
    .long   PORTB_IRQHandler                                /* PORTB Pin detect (INTMUX1 source IRQ26)*/
    .long   PORTC_IRQHandler                                /* PORTC Pin detect (INTMUX1 source IRQ27)*/
    .long   PORTD_IRQHandler                                /* PORTD Pin detect (INTMUX1 source IRQ28)*/
    .long   ADC0_IRQHandler                                 /* LPADC0 interrupt (INTMUX1 source IRQ29)*/
    .long   LPCMP0_IRQHandler                               /* LPCMP0 interrupt (INTMUX1 source IRQ30)*/
    .long   LPDAC0_IRQHandler                               /* DAC0 interrupt (INTMUX1 source IRQ31)*/

    .size    __isr_vector, . - __isr_vector

    .text
    .thumb

/* Reset Handler */

    .thumb_func
    .align 2
    .globl   Reset_Handler
    .weak    Reset_Handler
    .type    Reset_Handler, %function
Reset_Handler:
    cpsid   i               /* Mask interrupts */
    .equ    VTOR, 0xE000ED08
    ldr     r0, =VTOR
    ldr     r1, =__isr_vector
    str     r1, [r0]
    ldr     r2, [r1]
    msr     msp, r2
#ifndef __NO_SYSTEM_INIT
    ldr   r0,=SystemInit
    blx   r0
#endif
/*     Loop to copy data from read only memory to RAM. The ranges
 *      of copy from/to are specified by following symbols evaluated in
 *      linker script.
 *      __etext: End of code section, i.e., begin of data sections to copy from.
 *      __data_start__/__data_end__: RAM address range that data should be
 *      copied to. Both must be aligned to 4 bytes boundary.  */

    ldr    r1, =__etext
    ldr    r2, =__data_start__
    ldr    r3, =__data_end__

    subs    r3, r2
    ble     .LC0

.LC1:
    subs    r3, 4
    ldr    r0, [r1,r3]
    str    r0, [r2,r3]
    bgt    .LC1
.LC0:

#ifdef __STARTUP_CLEAR_BSS
/*     This part of work usually is done in C library startup code. Otherwise,
 *     define this macro to enable it in this startup.
 *
 *     Loop to zero out BSS section, which uses following symbols
 *     in linker script:
 *      __bss_start__: start of BSS section. Must align to 4
 *      __bss_end__: end of BSS section. Must align to 4
 */
    ldr r1, =__bss_start__
    ldr r2, =__bss_end__

    subs    r2, r1
    ble .LC3

    movs    r0, 0
.LC2:
    str r0, [r1, r2]
    subs    r2, 4
    bge .LC2
.LC3:
#endif
    cpsie   i               /* Unmask interrupts */
#ifndef __START
#define __START _start
#endif
#ifndef __ATOLLIC__
    ldr   r0,=__START
    blx   r0
#else
    ldr   r0,=__libc_init_array
    blx   r0
    ldr   r0,=main
    bx    r0
#endif
    .pool
    .size Reset_Handler, . - Reset_Handler

    .align  1
    .thumb_func
    .weak DefaultISR
    .type DefaultISR, %function
DefaultISR:
    ldr r0, =DefaultISR
    bx r0
    .size DefaultISR, . - DefaultISR

    .align 1
    .thumb_func
    .weak NMI_Handler
    .type NMI_Handler, %function
NMI_Handler:
    ldr   r0,=NMI_Handler
    bx    r0
    .size NMI_Handler, . - NMI_Handler

    .align 1
    .thumb_func
    .weak HardFault_Handler
    .type HardFault_Handler, %function
HardFault_Handler:
    ldr   r0,=HardFault_Handler
    bx    r0
    .size HardFault_Handler, . - HardFault_Handler

    .align 1
    .thumb_func
    .weak SVC_Handler
    .type SVC_Handler, %function
SVC_Handler:
    ldr   r0,=SVC_Handler
    bx    r0
    .size SVC_Handler, . - SVC_Handler

    .align 1
    .thumb_func
    .weak PendSV_Handler
    .type PendSV_Handler, %function
PendSV_Handler:
    ldr   r0,=PendSV_Handler
    bx    r0
    .size PendSV_Handler, . - PendSV_Handler

    .align 1
    .thumb_func
    .weak SysTick_Handler
    .type SysTick_Handler, %function
SysTick_Handler:
    ldr   r0,=SysTick_Handler
    bx    r0
    .size SysTick_Handler, . - SysTick_Handler

    .align 1
    .thumb_func
    .weak DMA1_04_IRQHandler
    .type DMA1_04_IRQHandler, %function
DMA1_04_IRQHandler:
    ldr   r0,=DMA1_04_DriverIRQHandler
    bx    r0
    .size DMA1_04_IRQHandler, . - DMA1_04_IRQHandler

    .align 1
    .thumb_func
    .weak DMA1_15_IRQHandler
    .type DMA1_15_IRQHandler, %function
DMA1_15_IRQHandler:
    ldr   r0,=DMA1_15_DriverIRQHandler
    bx    r0
    .size DMA1_15_IRQHandler, . - DMA1_15_IRQHandler

    .align 1
    .thumb_func
    .weak DMA1_26_IRQHandler
    .type DMA1_26_IRQHandler, %function
DMA1_26_IRQHandler:
    ldr   r0,=DMA1_26_DriverIRQHandler
    bx    r0
    .size DMA1_26_IRQHandler, . - DMA1_26_IRQHandler

    .align 1
    .thumb_func
    .weak DMA1_37_IRQHandler
    .type DMA1_37_IRQHandler, %function
DMA1_37_IRQHandler:
    ldr   r0,=DMA1_37_DriverIRQHandler
    bx    r0
    .size DMA1_37_IRQHandler, . - DMA1_37_IRQHandler

    .align 1
    .thumb_func
    .weak DMA1_Error_IRQHandler
    .type DMA1_Error_IRQHandler, %function
DMA1_Error_IRQHandler:
    ldr   r0,=DMA1_Error_DriverIRQHandler
    bx    r0
    .size DMA1_Error_IRQHandler, . - DMA1_Error_IRQHandler

    .align 1
    .thumb_func
    .weak LPI2C3_IRQHandler
    .type LPI2C3_IRQHandler, %function
LPI2C3_IRQHandler:
    ldr   r0,=LPI2C3_DriverIRQHandler
    bx    r0
    .size LPI2C3_IRQHandler, . - LPI2C3_IRQHandler

    .align 1
    .thumb_func
    .weak LPSPI3_IRQHandler
    .type LPSPI3_IRQHandler, %function
LPSPI3_IRQHandler:
    ldr   r0,=LPSPI3_DriverIRQHandler
    bx    r0
    .size LPSPI3_IRQHandler, . - LPSPI3_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART3_IRQHandler
    .type LPUART3_IRQHandler, %function
LPUART3_IRQHandler:
    ldr   r0,=LPUART3_DriverIRQHandler
    bx    r0
    .size LPUART3_IRQHandler, . - LPUART3_IRQHandler

    .align 1
    .thumb_func
    .weak INTMUX1_0_IRQHandler
    .type INTMUX1_0_IRQHandler, %function
INTMUX1_0_IRQHandler:
    ldr   r0,=INTMUX1_0_DriverIRQHandler
    bx    r0
    .size INTMUX1_0_IRQHandler, . - INTMUX1_0_IRQHandler

    .align 1
    .thumb_func
    .weak INTMUX1_1_IRQHandler
    .type INTMUX1_1_IRQHandler, %function
INTMUX1_1_IRQHandler:
    ldr   r0,=INTMUX1_1_DriverIRQHandler
    bx    r0
    .size INTMUX1_1_IRQHandler, . - INTMUX1_1_IRQHandler

    .align 1
    .thumb_func
    .weak INTMUX1_2_IRQHandler
    .type INTMUX1_2_IRQHandler, %function
INTMUX1_2_IRQHandler:
    ldr   r0,=INTMUX1_2_DriverIRQHandler
    bx    r0
    .size INTMUX1_2_IRQHandler, . - INTMUX1_2_IRQHandler

    .align 1
    .thumb_func
    .weak INTMUX1_3_IRQHandler
    .type INTMUX1_3_IRQHandler, %function
INTMUX1_3_IRQHandler:
    ldr   r0,=INTMUX1_3_DriverIRQHandler
    bx    r0
    .size INTMUX1_3_IRQHandler, . - INTMUX1_3_IRQHandler

    .align 1
    .thumb_func
    .weak INTMUX1_4_IRQHandler
    .type INTMUX1_4_IRQHandler, %function
INTMUX1_4_IRQHandler:
    ldr   r0,=INTMUX1_4_DriverIRQHandler
    bx    r0
    .size INTMUX1_4_IRQHandler, . - INTMUX1_4_IRQHandler

    .align 1
    .thumb_func
    .weak INTMUX1_5_IRQHandler
    .type INTMUX1_5_IRQHandler, %function
INTMUX1_5_IRQHandler:
    ldr   r0,=INTMUX1_5_DriverIRQHandler
    bx    r0
    .size INTMUX1_5_IRQHandler, . - INTMUX1_5_IRQHandler

    .align 1
    .thumb_func
    .weak INTMUX1_6_IRQHandler
    .type INTMUX1_6_IRQHandler, %function
INTMUX1_6_IRQHandler:
    ldr   r0,=INTMUX1_6_DriverIRQHandler
    bx    r0
    .size INTMUX1_6_IRQHandler, . - INTMUX1_6_IRQHandler

    .align 1
    .thumb_func
    .weak INTMUX1_7_IRQHandler
    .type INTMUX1_7_IRQHandler, %function
INTMUX1_7_IRQHandler:
    ldr   r0,=INTMUX1_7_DriverIRQHandler
    bx    r0
    .size INTMUX1_7_IRQHandler, . - INTMUX1_7_IRQHandler

    .align 1
    .thumb_func
    .weak FLEXIO0_IRQHandler
    .type FLEXIO0_IRQHandler, %function
FLEXIO0_IRQHandler:
    ldr   r0,=FLEXIO0_DriverIRQHandler
    bx    r0
    .size FLEXIO0_IRQHandler, . - FLEXIO0_IRQHandler

    .align 1
    .thumb_func
    .weak LPI2C0_IRQHandler
    .type LPI2C0_IRQHandler, %function
LPI2C0_IRQHandler:
    ldr   r0,=LPI2C0_DriverIRQHandler
    bx    r0
    .size LPI2C0_IRQHandler, . - LPI2C0_IRQHandler

    .align 1
    .thumb_func
    .weak LPI2C1_IRQHandler
    .type LPI2C1_IRQHandler, %function
LPI2C1_IRQHandler:
    ldr   r0,=LPI2C1_DriverIRQHandler
    bx    r0
    .size LPI2C1_IRQHandler, . - LPI2C1_IRQHandler

    .align 1
    .thumb_func
    .weak LPI2C2_IRQHandler
    .type LPI2C2_IRQHandler, %function
LPI2C2_IRQHandler:
    ldr   r0,=LPI2C2_DriverIRQHandler
    bx    r0
    .size LPI2C2_IRQHandler, . - LPI2C2_IRQHandler

    .align 1
    .thumb_func
    .weak I2S0_IRQHandler
    .type I2S0_IRQHandler, %function
I2S0_IRQHandler:
    ldr   r0,=I2S0_DriverIRQHandler
    bx    r0
    .size I2S0_IRQHandler, . - I2S0_IRQHandler

    .align 1
    .thumb_func
    .weak USDHC0_IRQHandler
    .type USDHC0_IRQHandler, %function
USDHC0_IRQHandler:
    ldr   r0,=USDHC0_DriverIRQHandler
    bx    r0
    .size USDHC0_IRQHandler, . - USDHC0_IRQHandler

    .align 1
    .thumb_func
    .weak LPSPI0_IRQHandler
    .type LPSPI0_IRQHandler, %function
LPSPI0_IRQHandler:
    ldr   r0,=LPSPI0_DriverIRQHandler
    bx    r0
    .size LPSPI0_IRQHandler, . - LPSPI0_IRQHandler

    .align 1
    .thumb_func
    .weak LPSPI1_IRQHandler
    .type LPSPI1_IRQHandler, %function
LPSPI1_IRQHandler:
    ldr   r0,=LPSPI1_DriverIRQHandler
    bx    r0
    .size LPSPI1_IRQHandler, . - LPSPI1_IRQHandler

    .align 1
    .thumb_func
    .weak LPSPI2_IRQHandler
    .type LPSPI2_IRQHandler, %function
LPSPI2_IRQHandler:
    ldr   r0,=LPSPI2_DriverIRQHandler
    bx    r0
    .size LPSPI2_IRQHandler, . - LPSPI2_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART0_IRQHandler
    .type LPUART0_IRQHandler, %function
LPUART0_IRQHandler:
    ldr   r0,=LPUART0_DriverIRQHandler
    bx    r0
    .size LPUART0_IRQHandler, . - LPUART0_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART1_IRQHandler
    .type LPUART1_IRQHandler, %function
LPUART1_IRQHandler:
    ldr   r0,=LPUART1_DriverIRQHandler
    bx    r0
    .size LPUART1_IRQHandler, . - LPUART1_IRQHandler

    .align 1
    .thumb_func
    .weak LPUART2_IRQHandler
    .type LPUART2_IRQHandler, %function
LPUART2_IRQHandler:
    ldr   r0,=LPUART2_DriverIRQHandler
    bx    r0
    .size LPUART2_IRQHandler, . - LPUART2_IRQHandler


/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro def_irq_handler  handler_name
    .weak \handler_name
    .set  \handler_name, DefaultISR
    .endm

/* Exception Handlers */
    def_irq_handler    CTI1_IRQHandler
    def_irq_handler    DMA1_04_DriverIRQHandler
    def_irq_handler    DMA1_15_DriverIRQHandler
    def_irq_handler    DMA1_26_DriverIRQHandler
    def_irq_handler    DMA1_37_DriverIRQHandler
    def_irq_handler    DMA1_Error_DriverIRQHandler
    def_irq_handler    CMC1_IRQHandler
    def_irq_handler    LLWU1_IRQHandler
    def_irq_handler    MUB_IRQHandler
    def_irq_handler    WDOG1_IRQHandler
    def_irq_handler    CAU3_Task_Complete_IRQHandler
    def_irq_handler    CAU3_Security_Violation_IRQHandler
    def_irq_handler    TRNG_IRQHandler
    def_irq_handler    LPIT1_IRQHandler
    def_irq_handler    LPTMR2_IRQHandler
    def_irq_handler    TPM3_IRQHandler
    def_irq_handler    LPI2C3_DriverIRQHandler
    def_irq_handler    RF0_0_IRQHandler
    def_irq_handler    RF0_1_IRQHandler
    def_irq_handler    LPSPI3_DriverIRQHandler
    def_irq_handler    LPUART3_DriverIRQHandler
    def_irq_handler    PORTE_IRQHandler
    def_irq_handler    LPCMP1_IRQHandler
    def_irq_handler    RTC_IRQHandler
    def_irq_handler    INTMUX1_0_DriverIRQHandler
    def_irq_handler    INTMUX1_1_DriverIRQHandler
    def_irq_handler    INTMUX1_2_DriverIRQHandler
    def_irq_handler    INTMUX1_3_DriverIRQHandler
    def_irq_handler    INTMUX1_4_DriverIRQHandler
    def_irq_handler    INTMUX1_5_DriverIRQHandler
    def_irq_handler    INTMUX1_6_DriverIRQHandler
    def_irq_handler    INTMUX1_7_DriverIRQHandler
    def_irq_handler    EWM_IRQHandler
    def_irq_handler    FTFE_Command_Complete_IRQHandler
    def_irq_handler    FTFE_Read_Collision_IRQHandler
    def_irq_handler    SPM_IRQHandler
    def_irq_handler    SCG_IRQHandler
    def_irq_handler    LPIT0_IRQHandler
    def_irq_handler    LPTMR0_IRQHandler
    def_irq_handler    LPTMR1_IRQHandler
    def_irq_handler    TPM0_IRQHandler
    def_irq_handler    TPM1_IRQHandler
    def_irq_handler    TPM2_IRQHandler
    def_irq_handler    EMVSIM0_IRQHandler
    def_irq_handler    FLEXIO0_DriverIRQHandler
    def_irq_handler    LPI2C0_DriverIRQHandler
    def_irq_handler    LPI2C1_DriverIRQHandler
    def_irq_handler    LPI2C2_DriverIRQHandler
    def_irq_handler    I2S0_DriverIRQHandler
    def_irq_handler    USDHC0_DriverIRQHandler
    def_irq_handler    LPSPI0_DriverIRQHandler
    def_irq_handler    LPSPI1_DriverIRQHandler
    def_irq_handler    LPSPI2_DriverIRQHandler
    def_irq_handler    LPUART0_DriverIRQHandler
    def_irq_handler    LPUART1_DriverIRQHandler
    def_irq_handler    LPUART2_DriverIRQHandler
    def_irq_handler    USB0_IRQHandler
    def_irq_handler    PORTA_IRQHandler
    def_irq_handler    PORTB_IRQHandler
    def_irq_handler    PORTC_IRQHandler
    def_irq_handler    PORTD_IRQHandler
    def_irq_handler    ADC0_IRQHandler
    def_irq_handler    LPCMP0_IRQHandler
    def_irq_handler    LPDAC0_IRQHandler

    .end
