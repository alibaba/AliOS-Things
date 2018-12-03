/* ---------------------------------------------------------------------------------------*/
/*  @file:    startup_RV32M1_cm4.s                                                   */
/*  @purpose: CMSIS Cortex-M4 Core Device Startup File                                    */
/*            RV32M1_cm4                                                             */
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
    .arch armv7-m

    .section .isr_vector, "a"
    .align 2
    .globl __isr_vector
__isr_vector:
    .long   __StackTop                                      /* Top of Stack */
    .long   Reset_Handler                                   /* Reset Handler */
    .long   NMI_Handler                                     /* NMI Handler*/
    .long   HardFault_Handler                               /* Hard Fault Handler*/
    .long   MemManage_Handler                               /* MPU Fault Handler*/
    .long   BusFault_Handler                                /* Bus Fault Handler*/
    .long   UsageFault_Handler                              /* Usage Fault Handler*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   0                                               /* Reserved*/
    .long   SVC_Handler                                     /* SVCall Handler*/
    .long   DebugMon_Handler                                /* Debug Monitor Handler*/
    .long   0                                               /* Reserved*/
    .long   PendSV_Handler                                  /* PendSV Handler*/
    .long   SysTick_Handler                                 /* SysTick Handler*/

                                                            /* External Interrupts*/
    .long   CTI0_MCM0_IRQHandler                            /* Cross Trigger Interface 0 / Miscellaneous Control Module*/
    .long   DMA0_IRQHandler                                 /* DMA0 channel 0 transfer complete*/
    .long   DMA1_IRQHandler                                 /* DMA0 channel 1 transfer complete*/
    .long   DMA2_IRQHandler                                 /* DMA0 channel 2 transfer complete*/
    .long   DMA3_IRQHandler                                 /* DMA0 channel 3 transfer complete*/
    .long   DMA4_IRQHandler                                 /* DMA0 channel 4 transfer complete*/
    .long   DMA5_IRQHandler                                 /* DMA0 channel 5 transfer complete*/
    .long   DMA6_IRQHandler                                 /* DMA0 channel 6 transfer complete*/
    .long   DMA7_IRQHandler                                 /* DMA0 channel 7 transfer complete*/
    .long   DMA8_IRQHandler                                 /* DMA0 channel 8 transfer complete*/
    .long   DMA9_IRQHandler                                 /* DMA0 channel 9 transfer complete*/
    .long   DMA10_IRQHandler                                /* DMA0 channel 10 transfer complete*/
    .long   DMA11_IRQHandler                                /* DMA0 channel 11 transfer complete*/
    .long   DMA12_IRQHandler                                /* DMA0 channel 12 transfer complete*/
    .long   DMA13_IRQHandler                                /* DMA0 channel 13 transfer complete*/
    .long   DMA14_IRQHandler                                /* DMA0 channel 14 transfer complete*/
    .long   DMA15_IRQHandler                                /* DMA0 channel 15 transfer complete*/
    .long   DMA0_Error_IRQHandler                           /* DMA0 channel 0-15 error interrupt*/
    .long   CMC0_IRQHandler                                 /* Core Mode Controller 0*/
    .long   EWM_IRQHandler                                  /* EWM interrupt*/
    .long   FTFE_Command_Complete_IRQHandler                /* FTFE interrupt*/
    .long   FTFE_Read_Collision_IRQHandler                  /* FTFE interrupt*/
    .long   LLWU0_IRQHandler                                /* Low leakage wakeup 0*/
    .long   MUA_IRQHandler                                  /* MU Side A interrupt*/
    .long   SPM_IRQHandler                                  /* SPM*/
    .long   WDOG0_IRQHandler                                /* WDOG0 interrupt*/
    .long   SCG_IRQHandler                                  /* SCG interrupt*/
    .long   LPIT0_IRQHandler                                /* LPIT0 interrupt*/
    .long   RTC_IRQHandler                                  /* RTC Alarm interrupt*/
    .long   LPTMR0_IRQHandler                               /* LPTMR0 interrupt*/
    .long   LPTMR1_IRQHandler                               /* LPTMR1 interrupt*/
    .long   TPM0_IRQHandler                                 /* TPM0 single interrupt vector for all sources*/
    .long   TPM1_IRQHandler                                 /* TPM1 single interrupt vector for all sources*/
    .long   TPM2_IRQHandler                                 /* TPM2 single interrupt vector for all sources*/
    .long   EMVSIM0_IRQHandler                              /* EMVSIM0 interrupt*/
    .long   FLEXIO0_IRQHandler                              /* FLEXIO0*/
    .long   LPI2C0_IRQHandler                               /* LPI2C0 interrupt*/
    .long   LPI2C1_IRQHandler                               /* LPI2C1 interrupt*/
    .long   LPI2C2_IRQHandler                               /* LPI2C2 interrupt*/
    .long   I2S0_IRQHandler                                 /* I2S0 interrupt*/
    .long   USDHC0_IRQHandler                               /* SDHC0 interrupt*/
    .long   LPSPI0_IRQHandler                               /* LPSPI0 single interrupt vector for all sources*/
    .long   LPSPI1_IRQHandler                               /* LPSPI1 single interrupt vector for all sources*/
    .long   LPSPI2_IRQHandler                               /* LPSPI2 single interrupt vector for all sources*/
    .long   LPUART0_IRQHandler                              /* LPUART0 status and error*/
    .long   LPUART1_IRQHandler                              /* LPUART1 status and error*/
    .long   LPUART2_IRQHandler                              /* LPUART2 status and error*/
    .long   USB0_IRQHandler                                 /* USB0 interrupt*/
    .long   PORTA_IRQHandler                                /* PORTA Pin detect*/
    .long   PORTB_IRQHandler                                /* PORTB Pin detect*/
    .long   PORTC_IRQHandler                                /* PORTC Pin detect*/
    .long   PORTD_IRQHandler                                /* PORTD Pin detect*/
    .long   ADC0_IRQHandler                                 /* LPADC0 interrupt*/
    .long   LPCMP0_IRQHandler                               /* LPCMP0 interrupt*/
    .long   LPDAC0_IRQHandler                               /* DAC0 interrupt*/
    .long   CAU3_Task_Complete_IRQHandler                   /* Cryptographic Acceleration Unit version 3 Task Complete*/
    .long   CAU3_Security_Violation_IRQHandler              /* Cryptographic Acceleration Unit version 3 Security Violation*/
    .long   TRNG_IRQHandler                                 /* TRNG interrupt*/
    .long   LPIT1_IRQHandler                                /* LPIT1 interrupt*/
    .long   LPTMR2_IRQHandler                               /* LPTMR2 interrupt*/
    .long   TPM3_IRQHandler                                 /* TPM3 single interrupt vector for all sources*/
    .long   LPI2C3_IRQHandler                               /* LPI2C3 interrupt*/
    .long   LPSPI3_IRQHandler                               /* LPSPI3 single interrupt vector for all sources*/
    .long   LPUART3_IRQHandler                              /* LPUART3 status and error*/
    .long   PORTE_IRQHandler                                /* PORTE Pin detect*/
    .long   LPCMP1_IRQHandler                               /* LPCMP1 interrupt*/
    .long   DefaultISR                                      /* 82*/
    .long   DefaultISR                                      /* 83*/
    .long   DefaultISR                                      /* 84*/
    .long   DefaultISR                                      /* 85*/
    .long   DefaultISR                                      /* 86*/
    .long   DefaultISR                                      /* 87*/
    .long   DefaultISR                                      /* 88*/
    .long   DefaultISR                                      /* 89*/
    .long   DefaultISR                                      /* 90*/
    .long   DefaultISR                                      /* 91*/
    .long   DefaultISR                                      /* 92*/
    .long   DefaultISR                                      /* 93*/
    .long   DefaultISR                                      /* 94*/
    .long   DefaultISR                                      /* 95*/
    .long   DefaultISR                                      /* 96*/
    .long   DefaultISR                                      /* 97*/
    .long   DefaultISR                                      /* 98*/
    .long   DefaultISR                                      /* 99*/
    .long   DefaultISR                                      /* 100*/
    .long   DefaultISR                                      /* 101*/
    .long   DefaultISR                                      /* 102*/
    .long   DefaultISR                                      /* 103*/
    .long   DefaultISR                                      /* 104*/
    .long   DefaultISR                                      /* 105*/
    .long   DefaultISR                                      /* 106*/
    .long   DefaultISR                                      /* 107*/
    .long   DefaultISR                                      /* 108*/
    .long   DefaultISR                                      /* 109*/
    .long   DefaultISR                                      /* 110*/
    .long   DefaultISR                                      /* 111*/
    .long   DefaultISR                                      /* 112*/
    .long   DefaultISR                                      /* 113*/
    .long   DefaultISR                                      /* 114*/
    .long   DefaultISR                                      /* 115*/
    .long   DefaultISR                                      /* 116*/
    .long   DefaultISR                                      /* 117*/
    .long   DefaultISR                                      /* 118*/
    .long   DefaultISR                                      /* 119*/
    .long   DefaultISR                                      /* 120*/
    .long   DefaultISR                                      /* 121*/
    .long   DefaultISR                                      /* 122*/
    .long   DefaultISR                                      /* 123*/
    .long   DefaultISR                                      /* 124*/
    .long   DefaultISR                                      /* 125*/
    .long   DefaultISR                                      /* 126*/
    .long   DefaultISR                                      /* 127*/
    .long   DefaultISR                                      /* 128*/
    .long   DefaultISR                                      /* 129*/
    .long   DefaultISR                                      /* 130*/
    .long   DefaultISR                                      /* 131*/
    .long   DefaultISR                                      /* 132*/
    .long   DefaultISR                                      /* 133*/
    .long   DefaultISR                                      /* 134*/
    .long   DefaultISR                                      /* 135*/
    .long   DefaultISR                                      /* 136*/
    .long   DefaultISR                                      /* 137*/
    .long   DefaultISR                                      /* 138*/
    .long   DefaultISR                                      /* 139*/
    .long   DefaultISR                                      /* 140*/
    .long   DefaultISR                                      /* 141*/
    .long   DefaultISR                                      /* 142*/
    .long   DefaultISR                                      /* 143*/
    .long   DefaultISR                                      /* 144*/
    .long   DefaultISR                                      /* 145*/
    .long   DefaultISR                                      /* 146*/
    .long   DefaultISR                                      /* 147*/
    .long   DefaultISR                                      /* 148*/
    .long   DefaultISR                                      /* 149*/
    .long   DefaultISR                                      /* 150*/
    .long   DefaultISR                                      /* 151*/
    .long   DefaultISR                                      /* 152*/
    .long   DefaultISR                                      /* 153*/
    .long   DefaultISR                                      /* 154*/
    .long   DefaultISR                                      /* 155*/
    .long   DefaultISR                                      /* 156*/
    .long   DefaultISR                                      /* 157*/
    .long   DefaultISR                                      /* 158*/
    .long   DefaultISR                                      /* 159*/
    .long   DefaultISR                                      /* 160*/
    .long   DefaultISR                                      /* 161*/
    .long   DefaultISR                                      /* 162*/
    .long   DefaultISR                                      /* 163*/
    .long   DefaultISR                                      /* 164*/
    .long   DefaultISR                                      /* 165*/
    .long   DefaultISR                                      /* 166*/
    .long   DefaultISR                                      /* 167*/
    .long   DefaultISR                                      /* 168*/
    .long   DefaultISR                                      /* 169*/
    .long   DefaultISR                                      /* 170*/
    .long   DefaultISR                                      /* 171*/
    .long   DefaultISR                                      /* 172*/
    .long   DefaultISR                                      /* 173*/
    .long   DefaultISR                                      /* 174*/
    .long   DefaultISR                                      /* 175*/
    .long   DefaultISR                                      /* 176*/
    .long   DefaultISR                                      /* 177*/
    .long   DefaultISR                                      /* 178*/
    .long   DefaultISR                                      /* 179*/
    .long   DefaultISR                                      /* 180*/
    .long   DefaultISR                                      /* 181*/
    .long   DefaultISR                                      /* 182*/
    .long   DefaultISR                                      /* 183*/
    .long   DefaultISR                                      /* 184*/
    .long   DefaultISR                                      /* 185*/
    .long   DefaultISR                                      /* 186*/
    .long   DefaultISR                                      /* 187*/
    .long   DefaultISR                                      /* 188*/
    .long   DefaultISR                                      /* 189*/
    .long   DefaultISR                                      /* 190*/
    .long   DefaultISR                                      /* 191*/
    .long   DefaultISR                                      /* 192*/
    .long   DefaultISR                                      /* 193*/
    .long   DefaultISR                                      /* 194*/
    .long   DefaultISR                                      /* 195*/
    .long   DefaultISR                                      /* 196*/
    .long   DefaultISR                                      /* 197*/
    .long   DefaultISR                                      /* 198*/
    .long   DefaultISR                                      /* 199*/
    .long   DefaultISR                                      /* 200*/
    .long   DefaultISR                                      /* 201*/
    .long   DefaultISR                                      /* 202*/
    .long   DefaultISR                                      /* 203*/
    .long   DefaultISR                                      /* 204*/
    .long   DefaultISR                                      /* 205*/
    .long   DefaultISR                                      /* 206*/
    .long   DefaultISR                                      /* 207*/
    .long   DefaultISR                                      /* 208*/
    .long   DefaultISR                                      /* 209*/
    .long   DefaultISR                                      /* 210*/
    .long   DefaultISR                                      /* 211*/
    .long   DefaultISR                                      /* 212*/
    .long   DefaultISR                                      /* 213*/
    .long   DefaultISR                                      /* 214*/
    .long   DefaultISR                                      /* 215*/
    .long   DefaultISR                                      /* 216*/
    .long   DefaultISR                                      /* 217*/
    .long   DefaultISR                                      /* 218*/
    .long   DefaultISR                                      /* 219*/
    .long   DefaultISR                                      /* 220*/
    .long   DefaultISR                                      /* 221*/
    .long   DefaultISR                                      /* 222*/
    .long   DefaultISR                                      /* 223*/
    .long   DefaultISR                                      /* 224*/
    .long   DefaultISR                                      /* 225*/
    .long   DefaultISR                                      /* 226*/
    .long   DefaultISR                                      /* 227*/
    .long   DefaultISR                                      /* 228*/
    .long   DefaultISR                                      /* 229*/
    .long   DefaultISR                                      /* 230*/
    .long   DefaultISR                                      /* 231*/
    .long   DefaultISR                                      /* 232*/
    .long   DefaultISR                                      /* 233*/
    .long   DefaultISR                                      /* 234*/
    .long   DefaultISR                                      /* 235*/
    .long   DefaultISR                                      /* 236*/
    .long   DefaultISR                                      /* 237*/
    .long   DefaultISR                                      /* 238*/
    .long   DefaultISR                                      /* 239*/
    .long   DefaultISR                                      /* 240*/
    .long   DefaultISR                                      /* 241*/
    .long   DefaultISR                                      /* 242*/
    .long   DefaultISR                                      /* 243*/
    .long   DefaultISR                                      /* 244*/
    .long   DefaultISR                                      /* 245*/
    .long   DefaultISR                                      /* 246*/
    .long   DefaultISR                                      /* 247*/
    .long   DefaultISR                                      /* 248*/
    .long   DefaultISR                                      /* 249*/
    .long   DefaultISR                                      /* 250*/
    .long   DefaultISR                                      /* 251*/
    .long   DefaultISR                                      /* 252*/
    .long   DefaultISR                                      /* 253*/
    .long   DefaultISR                                      /* 254*/
    .long   0xFFFFFFFF                                      /*  Reserved for user TRIM value*/

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

#if 1
/* Here are two copies of loop implemenations. First one favors code size
 * and the second one favors performance. Default uses the first one.
 * Change to "#if 0" to use the second one */
.LC0:
    cmp     r2, r3
    ittt    lt
    ldrlt   r0, [r1], #4
    strlt   r0, [r2], #4
    blt    .LC0
#else
    subs    r3, r2
    ble    .LC1
.LC0:
    subs    r3, #4
    ldr    r0, [r1, r3]
    str    r0, [r2, r3]
    bgt    .LC0
.LC1:
#endif

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

    movs    r0, 0
.LC2:
    cmp     r1, r2
    itt    lt
    strlt   r0, [r1], #4
    blt    .LC2
#endif /* __STARTUP_CLEAR_BSS */

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
    b DefaultISR
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
    .weak DMA0_IRQHandler
    .type DMA0_IRQHandler, %function
DMA0_IRQHandler:
    ldr   r0,=DMA0_DriverIRQHandler
    bx    r0
    .size DMA0_IRQHandler, . - DMA0_IRQHandler

    .align 1
    .thumb_func
    .weak DMA1_IRQHandler
    .type DMA1_IRQHandler, %function
DMA1_IRQHandler:
    ldr   r0,=DMA1_DriverIRQHandler
    bx    r0
    .size DMA1_IRQHandler, . - DMA1_IRQHandler

    .align 1
    .thumb_func
    .weak DMA2_IRQHandler
    .type DMA2_IRQHandler, %function
DMA2_IRQHandler:
    ldr   r0,=DMA2_DriverIRQHandler
    bx    r0
    .size DMA2_IRQHandler, . - DMA2_IRQHandler

    .align 1
    .thumb_func
    .weak DMA3_IRQHandler
    .type DMA3_IRQHandler, %function
DMA3_IRQHandler:
    ldr   r0,=DMA3_DriverIRQHandler
    bx    r0
    .size DMA3_IRQHandler, . - DMA3_IRQHandler

    .align 1
    .thumb_func
    .weak DMA4_IRQHandler
    .type DMA4_IRQHandler, %function
DMA4_IRQHandler:
    ldr   r0,=DMA4_DriverIRQHandler
    bx    r0
    .size DMA4_IRQHandler, . - DMA4_IRQHandler

    .align 1
    .thumb_func
    .weak DMA5_IRQHandler
    .type DMA5_IRQHandler, %function
DMA5_IRQHandler:
    ldr   r0,=DMA5_DriverIRQHandler
    bx    r0
    .size DMA5_IRQHandler, . - DMA5_IRQHandler

    .align 1
    .thumb_func
    .weak DMA6_IRQHandler
    .type DMA6_IRQHandler, %function
DMA6_IRQHandler:
    ldr   r0,=DMA6_DriverIRQHandler
    bx    r0
    .size DMA6_IRQHandler, . - DMA6_IRQHandler

    .align 1
    .thumb_func
    .weak DMA7_IRQHandler
    .type DMA7_IRQHandler, %function
DMA7_IRQHandler:
    ldr   r0,=DMA7_DriverIRQHandler
    bx    r0
    .size DMA7_IRQHandler, . - DMA7_IRQHandler

    .align 1
    .thumb_func
    .weak DMA8_IRQHandler
    .type DMA8_IRQHandler, %function
DMA8_IRQHandler:
    ldr   r0,=DMA8_DriverIRQHandler
    bx    r0
    .size DMA8_IRQHandler, . - DMA8_IRQHandler

    .align 1
    .thumb_func
    .weak DMA9_IRQHandler
    .type DMA9_IRQHandler, %function
DMA9_IRQHandler:
    ldr   r0,=DMA9_DriverIRQHandler
    bx    r0
    .size DMA9_IRQHandler, . - DMA9_IRQHandler

    .align 1
    .thumb_func
    .weak DMA10_IRQHandler
    .type DMA10_IRQHandler, %function
DMA10_IRQHandler:
    ldr   r0,=DMA10_DriverIRQHandler
    bx    r0
    .size DMA10_IRQHandler, . - DMA10_IRQHandler

    .align 1
    .thumb_func
    .weak DMA11_IRQHandler
    .type DMA11_IRQHandler, %function
DMA11_IRQHandler:
    ldr   r0,=DMA11_DriverIRQHandler
    bx    r0
    .size DMA11_IRQHandler, . - DMA11_IRQHandler

    .align 1
    .thumb_func
    .weak DMA12_IRQHandler
    .type DMA12_IRQHandler, %function
DMA12_IRQHandler:
    ldr   r0,=DMA12_DriverIRQHandler
    bx    r0
    .size DMA12_IRQHandler, . - DMA12_IRQHandler

    .align 1
    .thumb_func
    .weak DMA13_IRQHandler
    .type DMA13_IRQHandler, %function
DMA13_IRQHandler:
    ldr   r0,=DMA13_DriverIRQHandler
    bx    r0
    .size DMA13_IRQHandler, . - DMA13_IRQHandler

    .align 1
    .thumb_func
    .weak DMA14_IRQHandler
    .type DMA14_IRQHandler, %function
DMA14_IRQHandler:
    ldr   r0,=DMA14_DriverIRQHandler
    bx    r0
    .size DMA14_IRQHandler, . - DMA14_IRQHandler

    .align 1
    .thumb_func
    .weak DMA15_IRQHandler
    .type DMA15_IRQHandler, %function
DMA15_IRQHandler:
    ldr   r0,=DMA15_DriverIRQHandler
    bx    r0
    .size DMA15_IRQHandler, . - DMA15_IRQHandler

    .align 1
    .thumb_func
    .weak DMA0_Error_IRQHandler
    .type DMA0_Error_IRQHandler, %function
DMA0_Error_IRQHandler:
    ldr   r0,=DMA0_Error_DriverIRQHandler
    bx    r0
    .size DMA0_Error_IRQHandler, . - DMA0_Error_IRQHandler

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


/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro def_irq_handler  handler_name
    .weak \handler_name
    .set  \handler_name, DefaultISR
    .endm

/* Exception Handlers */
    def_irq_handler    MemManage_Handler
    def_irq_handler    BusFault_Handler
    def_irq_handler    UsageFault_Handler
    def_irq_handler    DebugMon_Handler
    def_irq_handler    CTI0_MCM0_IRQHandler
    def_irq_handler    DMA0_DriverIRQHandler
    def_irq_handler    DMA1_DriverIRQHandler
    def_irq_handler    DMA2_DriverIRQHandler
    def_irq_handler    DMA3_DriverIRQHandler
    def_irq_handler    DMA4_DriverIRQHandler
    def_irq_handler    DMA5_DriverIRQHandler
    def_irq_handler    DMA6_DriverIRQHandler
    def_irq_handler    DMA7_DriverIRQHandler
    def_irq_handler    DMA8_DriverIRQHandler
    def_irq_handler    DMA9_DriverIRQHandler
    def_irq_handler    DMA10_DriverIRQHandler
    def_irq_handler    DMA11_DriverIRQHandler
    def_irq_handler    DMA12_DriverIRQHandler
    def_irq_handler    DMA13_DriverIRQHandler
    def_irq_handler    DMA14_DriverIRQHandler
    def_irq_handler    DMA15_DriverIRQHandler
    def_irq_handler    DMA0_Error_DriverIRQHandler
    def_irq_handler    CMC0_IRQHandler
    def_irq_handler    EWM_IRQHandler
    def_irq_handler    FTFE_Command_Complete_IRQHandler
    def_irq_handler    FTFE_Read_Collision_IRQHandler
    def_irq_handler    LLWU0_IRQHandler
    def_irq_handler    MUA_IRQHandler
    def_irq_handler    SPM_IRQHandler
    def_irq_handler    WDOG0_IRQHandler
    def_irq_handler    SCG_IRQHandler
    def_irq_handler    LPIT0_IRQHandler
    def_irq_handler    RTC_IRQHandler
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
    def_irq_handler    CAU3_Task_Complete_IRQHandler
    def_irq_handler    CAU3_Security_Violation_IRQHandler
    def_irq_handler    TRNG_IRQHandler
    def_irq_handler    LPIT1_IRQHandler
    def_irq_handler    LPTMR2_IRQHandler
    def_irq_handler    TPM3_IRQHandler
    def_irq_handler    LPI2C3_DriverIRQHandler
    def_irq_handler    LPSPI3_DriverIRQHandler
    def_irq_handler    LPUART3_DriverIRQHandler
    def_irq_handler    PORTE_IRQHandler
    def_irq_handler    LPCMP1_IRQHandler

    .end
