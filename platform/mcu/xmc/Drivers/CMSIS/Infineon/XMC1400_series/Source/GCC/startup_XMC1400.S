/*********************************************************************************************************************
 * @file     startup_XMC1400.S
 * @brief    CMSIS Core Device Startup File for Infineon XMC1400 Device Series
 * @version  V1.2
 * @date     15 Sep 2017
 *
 * @cond
 *********************************************************************************************************************
 * Copyright (c) 2015-2017, Infineon Technologies AG
 * All rights reserved.                        
 *                                             
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the 
 * following conditions are met:   
 *                                                                              
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following 
 * disclaimer.                        
 * 
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following 
 * disclaimer in the documentation and/or other materials provided with the distribution.                       
 * 
 * Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote 
 * products derived from this software without specific prior written permission.                                           
 *                                                                              
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE  
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR  
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                                                  
 *                                                                              
 * To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes with 
 * Infineon Technologies AG dave@infineon.com).                                                          
 *********************************************************************************************************************
 *
 **************************** Change history ********************************
 * V1.0, Sep, 03, 2015 JFT:Initial version
 *                         MCLK=8MHz, PCLK=16MHz
 * V1.1, Jan, 05, 2016 JFT:Fix .reset section attributes
 * V1.2, Sep, 15, 2017 JFT:Added option to select wait time before ASC BSL channel selection (WAIT_ASCBSL_ENTRY_SSW)
 *
 * @endcond 
 */

/*****************************************************************************
 * <h> Clock system handling by SSW
 *  <h> CLK_VAL1 Configuration
 *   <o0.0..9>    FDIV Fractional Divider Selection <0-1023>
 *   <i> Deafult: 0. Fractional part of clock divider, MCLK = DCO1 / (IDIV + (FDIV / 1024))
 *   <o0.10..17>  IDIV Divider Selection <1-16>
 *   <i> Deafult: 6. Interger part of clock divider, MCLK = DCO1 / (IDIV + (FDIV / 1024) = 8MHz)
 *   <o0.18>      PCLKSEL PCLK Clock Select
 *                   <0=> PCLK = MCLK
 *                   <1=> PCLK = 2 x MCLK
 *   <i> Deafult: 2 x MCLK
 *   <o0.19..21>  RTCCLKSEL RTC Clock Select
 *                   <0=> 32.768kHz standby clock
 *                   <1=> 32.768kHz external clock from ERU0.IOUT0
 *                   <2=> 32.768kHz external clock from ACMP0.OUT
 *                   <3=> 32.768kHz external clock from ACMP1.OUT
 *                   <4=> 32.768kHz external clock from ACMP2.OUT
 *                   <5=> 32.768kHz XTAL clock via OSC_LP
 *                   <6=> Reserved
 *                   <7=> Reserved
 *   <i> Deafult: 32.768kHz standby clock
 *   <o0.31>      do not move CLK_VAL1 to SCU_CLKCR[0..19]
 * </h>
 *****************************************************************************/
#define CLKVAL1_SSW 0x00041800

/*****************************************************************************
 *  <h> CLK_VAL2 Configuration
 *    <o0.0>    disable VADC and SHS Gating
 *    <o0.1>    disable CCU80 Gating
 *    <o0.2>    disable CCU40 Gating
 *    <o0.3>    disable USIC0 Gating
 *    <o0.4>    disable BCCU0 Gating
 *    <o0.5>    disable LEDTS0 Gating
 *    <o0.6>    disable LEDTS1 Gating
 *    <o0.7>    disable POSIF0 Gating
 *    <o0.8>    disable MATH Gating
 *    <o0.9>    disable WDT Gating
 *    <o0.10>   disable RTC Gating
 *    <o0.16>   disable CCU81 Gating
 *    <o0.17>   disable CCU41 Gating 
 *    <o0.18>   disable USIC1 Gating
 *    <o0.19>   disable LEDTS2 Gating 
 *    <o0.20>   disable POSIF1 Gating
 *    <o0.21>   disable MCAN0 Gating
 *    <o0.31>   do not move CLK_VAL2 to SCU_CGATCLR0[0..10]
 *  </h>
 *****************************************************************************/
#define CLKVAL2_SSW 0x00000100

/*****************************************************************************
 *  <h> WAIT_ASCBSL_ENTRY Configuration
 *    <o0.0..30>    Wait time before ASC BSL channel selection (number of MCLK clock cycles)
 *    <o0.31>       Disable wait time before ASC BSL channel selection
 *  </h>
 *****************************************************************************/
#define WAIT_ASCBSL_ENTRY_SSW 0x80000000

/* A couple of macros to ease definition of the various handlers */
.macro Insert_InterruptHandler Interrupt
    .weak \Interrupt\()_Handler
    .thumb_set \Interrupt\()_Handler, Default_Handler
.endm

.macro Insert_InterruptVeener Interrupt
    .globl \Interrupt\()_Veener
\Interrupt\()_Veener:
    LDR R0, =\Interrupt\()_Handler
    BX  R0
.endm

/* ================== START OF VECTOR TABLE DEFINITION ====================== */
/* Vector Table - This is indirectly branched to through the veneers */
    .syntax unified   
    .cpu cortex-m0

    .section .reset, "a", %progbits
    
 	.align 2
    
    .globl  __Vectors
    .type   __Vectors, %object
__Vectors:
    .long   __initial_sp                /* Top of Stack                 */
    .long   Reset_Handler               /* Reset Handler                */
/* 
 * All entries below are redundant for M0, but are retained because they can
 * in the future be directly ported to M0 Plus devices.
 */
    .long   0                           /* Reserved                     */
    .long   HardFault_Handler           /* Hard Fault Handler           */
    .long   CLKVAL1_SSW                 /* Reserved                     */
    .long   CLKVAL2_SSW                 /* Reserved                     */
    .long   WAIT_ASCBSL_ENTRY_SSW       /* Reserved                     */
#ifdef RETAIN_VECTOR_TABLE
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    .long   SVC_Handler                 /* SVCall Handler               */
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    .long   PendSV_Handler              /* PendSV Handler               */
    .long   SysTick_Handler             /* SysTick Handler              */

    /* Interrupt Handlers for Service Requests (SR) from XMC1400 Peripherals */
    .long   IRQ0_Handler				/* SCU.SR0, CAN0.SR0, CCU40.SR0, SCU.SR0 | CAN0.SR0 */
    .long   IRQ1_Handler				/* SCU.SR1, CAN0.SR1, CCU80.SR0, SCU.SR1 | CAN0.SR1 */
    .long   IRQ2_Handler				/* SCU.SR2, CAN0.SR2, CCU80.SR1, SCU.SR2 | CAN0.SR2 */
    .long   IRQ3_Handler				/* ERU0.SR0, ERU1.SR0, CAN0.SR0, ERU0.SR0 | ERU1.SR0 */
    .long   IRQ4_Handler                /* ERU0.SR1, ERU1.SR1, CAN0.SR1, ERU0.SR1 | ERU1.SR1 */
    .long   IRQ5_Handler                /* ERU0.SR2, ERU1.SR2, CAN0.SR2, ERU0.SR2 | ERU1.SR2 */
    .long   IRQ6_Handler                /* ERU0.SR3, ERU1.SR3, CAN0.SR3, ERU0.SR3 | ERU1.SR3 */
    .long   IRQ7_Handler                /* MATH.SR0, CAN0.SR3, CCU40.SR1, MATH.SR0 | CAN0.SR3 */
    .long   IRQ8_Handler                /* LEDTS2.SR0, CCU40.SR0, CCU80.SR0, LEDTS2.SR0 | CCU40.SR0 */
    .long   IRQ9_Handler                /* USIC0.SR0, USIC1.SR0, ERU0.SR0, USIC0.SR0 | USIC1.SR0 */
    .long   IRQ10_Handler               /* USIC0.SR1, USIC1.SR1, ERU0.SR1, USIC0.SR1 | USIC1.SR1 */
    .long   IRQ11_Handler               /* USIC0.SR2, USIC1.SR2, ERU0.SR2, USIC0.SR2 | USIC1.SR2 */
    .long   IRQ12_Handler               /* USIC0.SR3, USIC1.SR3, ERU0.SR3, USIC0.SR3 | USIC1.SR3 */
    .long   IRQ13_Handler               /* USIC0.SR4, USIC1.SR4, CCU80.SR1, USIC0.SR4 | USIC1.SR4 */
    .long   IRQ14_Handler               /* USIC0.SR5, USIC1.SR5, POSIF0.SR0, USIC0.SR5 | USIC1.SR5 */
    .long   IRQ15_Handler               /* VADC0.C0SR0, USIC0.SR0, POSIF0.SR1, VADC0.C0SR0 | USIC0.SR0 */
    .long   IRQ16_Handler			    /* VADC0.C0SR1, USIC0.SR1, CCU40.SR2, VADC0.C0SR1 | USIC0.SR1 */
    .long   IRQ17_Handler               /* VADC0.G0SR0, USIC0.SR2, CAN0.SR0, VADC0.G0SR0 | USIC0.SR2 */
    .long   IRQ18_Handler               /* VADC0.G0SR1, USIC0.SR3, CAN0.SR1, VADC0.G0SR1 | USIC0.SR3 */
    .long   IRQ19_Handler               /* VADC0.G1SR0, USIC0.SR4, CAN0.SR2, VADC0.G1SR0 | USIC0.SR4 */
    .long   IRQ20_Handler               /* VADC0.G1SR1, USIC0.SR5, CAN0.SR3, VADC0.G1SR1 | USIC0.SR5 */
    .long   IRQ21_Handler               /* CCU40.SR0, CCU41.SR0, USIC0.SR0, CCU40.SR0 | CCU41.SR0 */
    .long   IRQ22_Handler               /* CCU40.SR1, CCU41.SR1, USIC0.SR1, CCU40.SR1 | CCU41.SR1 */
    .long   IRQ23_Handler               /* CCU40.SR2, CCU41.SR2, USIC0.SR2, CCU40.SR2 | CCU41.SR2 */
    .long   IRQ24_Handler               /* CCU40.SR3, CCU41.SR3, USIC0.SR3, CCU40.SR3 | CCU41.SR3 */
    .long   IRQ25_Handler               /* CCU80.SR0, CCU81.SR0, USIC0.SR4, CCU80.SR0 | CCU81.SR0 */
    .long   IRQ26_Handler               /* CCU80.SR1, CCU81.SR1, USIC0.SR5, CCU80.SR1 | CCU81.SR1 */
    .long   IRQ27_Handler               /* POSIF0.SR0, POSIF1.SR0, CCU40.SR3, POSIF0.SR0 | POSIF1.SR0 */
    .long   IRQ28_Handler               /* POSIF0.SR1, POSIF1.SR1, ERU0.SR0, POSIF0.SR1 | POSIF1.SR1 */
    .long   IRQ29_Handler               /* LEDTS0.SR0, CCU40.SR1, ERU0.SR1, LEDTS0.SR0 | CCU40.SR1 */
    .long   IRQ30_Handler               /* LEDTS1.SR0, CCU40.SR2, ERU0.SR2, LEDTS1.SR0 | CCU40.SR2 */
    .long   IRQ31_Handler               /* BCCU0.SR0, CCU40.SR3, ERU0.SR3, BCCU0.SR0 | CCU40.SR3 */
#endif

    .size  __Vectors, . - __Vectors
/* ================== END OF VECTOR TABLE DEFINITION ======================= */

/* ================== START OF VECTOR ROUTINES ============================= */

    .thumb 
	.align 1
 
/* Reset Handler */
    .thumb_func 
    .globl  Reset_Handler
    .type   Reset_Handler, %function
Reset_Handler: 
/* Initialize interrupt veneer */
	ldr	r1, =eROData
	ldr	r2, =VeneerStart
	ldr	r3, =VeneerEnd
	bl  __copy_data

    ldr  r0, =SystemInit
    blx  r0
	
/* Initialize data */
	ldr	r1, =DataLoadAddr
	ldr	r2, =__data_start
	ldr	r3, =__data_end
	bl  __copy_data

/* RAM code */
	ldr	r1, =__ram_code_load
	ldr	r2, =__ram_code_start
	ldr	r3, =__ram_code_end
	bl  __copy_data

/*  Define __SKIP_BSS_CLEAR to disable zeroing uninitialzed data in startup.
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */
#ifndef __SKIP_BSS_CLEAR
	ldr	r1, =__bss_start
	ldr	r2, =__bss_end

	movs	r0, 0

	subs	r2, r1
	ble	.L_loop3_done

.L_loop3:
	subs	r2, #4
	str	r0, [r1, r2]
	bgt	.L_loop3
.L_loop3_done:
#endif /* __SKIP_BSS_CLEAR */

#ifndef __SKIP_LIBC_INIT_ARRAY
    ldr  r0, =__libc_init_array
    blx  r0
#endif

    ldr  r0, =main
    blx  r0

    .thumb_func
    .type __copy_data, %function
__copy_data:
/*  The ranges of copy from/to are specified by following symbols
 *    r1: start of the section to copy from.
 *    r2: start of the section to copy to
 *    r3: end of the section to copy to
 *
 *  All addresses must be aligned to 4 bytes boundary.
 *  Uses r0
 */
	subs	r3, r2
	ble	.L_loop_done

.L_loop:
	subs	r3, #4
	ldr	r0, [r1,r3]
	str	r0, [r2,r3]
	bgt	.L_loop

.L_loop_done:
	bx  lr

	.pool
    .size   Reset_Handler,.-Reset_Handler
/* ======================================================================== */
/* ========== START OF EXCEPTION HANDLER DEFINITION ======================== */

	.align 1
    
    .thumb_func
    .weak Default_handler
    .type Default_handler, %function
Default_Handler:
    b  .
    .size Default_Handler, . - Default_Handler

    Insert_InterruptHandler HardFault
    Insert_InterruptHandler SVC
    Insert_InterruptHandler PendSV
    Insert_InterruptHandler SysTick

    Insert_InterruptHandler IRQ0
    Insert_InterruptHandler IRQ1
    Insert_InterruptHandler IRQ2
    Insert_InterruptHandler IRQ3
    Insert_InterruptHandler IRQ4
    Insert_InterruptHandler IRQ5
    Insert_InterruptHandler IRQ6
    Insert_InterruptHandler IRQ7
    Insert_InterruptHandler IRQ8
    Insert_InterruptHandler IRQ9
    Insert_InterruptHandler IRQ10
    Insert_InterruptHandler IRQ11
    Insert_InterruptHandler IRQ12
    Insert_InterruptHandler IRQ13
    Insert_InterruptHandler IRQ14
    Insert_InterruptHandler IRQ15
    Insert_InterruptHandler IRQ16
    Insert_InterruptHandler IRQ17
    Insert_InterruptHandler IRQ18
    Insert_InterruptHandler IRQ19
    Insert_InterruptHandler IRQ20
    Insert_InterruptHandler IRQ21
    Insert_InterruptHandler IRQ22
    Insert_InterruptHandler IRQ23
    Insert_InterruptHandler IRQ24
    Insert_InterruptHandler IRQ25
    Insert_InterruptHandler IRQ26
    Insert_InterruptHandler IRQ27
    Insert_InterruptHandler IRQ28
    Insert_InterruptHandler IRQ29
    Insert_InterruptHandler IRQ30
    Insert_InterruptHandler IRQ31
   
/* ======================================================================== */

/* ==================VENEERS VENEERS VENEERS VENEERS VENEERS=============== */
    .section ".XmcVeneerCode","ax",%progbits
    
    .align 1
    
	Insert_InterruptVeener HardFault
    .long 0
    .long 0
    .long 0
    .long 0
    .long 0
    .long 0
    .long 0
	Insert_InterruptVeener SVC
    .long 0
    .long 0
	Insert_InterruptVeener PendSV	
	Insert_InterruptVeener SysTick
	
	Insert_InterruptVeener IRQ0	
	Insert_InterruptVeener IRQ1	
	Insert_InterruptVeener IRQ2	
	Insert_InterruptVeener IRQ3	
	Insert_InterruptVeener IRQ4	
	Insert_InterruptVeener IRQ5	
	Insert_InterruptVeener IRQ6	
	Insert_InterruptVeener IRQ7	
	Insert_InterruptVeener IRQ8	
	Insert_InterruptVeener IRQ9	
	Insert_InterruptVeener IRQ10	
	Insert_InterruptVeener IRQ11	
	Insert_InterruptVeener IRQ12	
	Insert_InterruptVeener IRQ13	
	Insert_InterruptVeener IRQ14	
	Insert_InterruptVeener IRQ15	
	Insert_InterruptVeener IRQ16	
	Insert_InterruptVeener IRQ17	
	Insert_InterruptVeener IRQ18	
	Insert_InterruptVeener IRQ19	
	Insert_InterruptVeener IRQ20
	Insert_InterruptVeener IRQ21
	Insert_InterruptVeener IRQ22	
	Insert_InterruptVeener IRQ23	
	Insert_InterruptVeener IRQ24	
	Insert_InterruptVeener IRQ25	
	Insert_InterruptVeener IRQ26	
	Insert_InterruptVeener IRQ27	
	Insert_InterruptVeener IRQ28	
	Insert_InterruptVeener IRQ29	
	Insert_InterruptVeener IRQ30	
	Insert_InterruptVeener IRQ31	

/* ======================================================================== */
/* ======================================================================== */

/* ============= END OF INTERRUPT HANDLER DEFINITION ======================== */

    .end
