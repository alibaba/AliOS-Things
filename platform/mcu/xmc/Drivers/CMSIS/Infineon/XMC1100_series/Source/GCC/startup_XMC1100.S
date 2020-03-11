/*********************************************************************************************************************
 * @file     startup_XMC1100.S
 * @brief    CMSIS Core Device Startup File for Infineon XMC1100 Device Series
 * @version  V1.15
 * @date     05 Jan 2016
 *
 * @cond
 *********************************************************************************************************************
 * Copyright (c) 2012-2016, Infineon Technologies AG
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
 * V1.0, Oct, 02, 2012 PKB:Startup file for XMC1  
 * V1.1, Oct, 19, 2012 PKB:ERU and MATH interrupt handlers  
 * V1.2, Nov, 02, 2012 PKB:Renamed AllowPLLInitByStartup to AllowClkInitByStartup  
 * V1.3, Dec, 11, 2012 PKB:Attributes of .XmcVeneerCode section changed  
 * V1.4, Dec, 13, 2012 PKB:Removed unwanted interrupts/veneers  
 * V1.5, Jan, 26, 2013 PKB:Corrected the SSW related entries  
 * V1.6, Feb, 13, 2013 PKB:Relative path to Device_Data.h  
 * V1.7, Feb, 19, 2013 PKB:Included XMC1100_SCU.inc
 * V1.8, Jan, 24, 2014 PKB:Removed AllowClkInitStartup and DAVE Extended init
 * V1.9, Feb, 05, 2014 PKB:Removed redundant alignment code from copy+clear funcs
 * V1.10, Feb, 14, 2014 PKB:Added software_init_hook and hardware_init_hook
 * V1.11, May, 06, 2014 JFT:__COPY_FLASH2RAM to initialize ram 
 *                          Added ram_code section initialization
 * V1.12, Sep, 29, 2014 JFT:One single default handler
 *                          Device_Data.h not included, user may use CLKVAL1_SSW
 *                          and CLKVAL2_SSW.
 *                          software_init_hook and hardware_init_hook removed
 *                          Misc optimizations
 * V1.13, Dec, 11,2014 JFT:Default clocking changed, MCLK=32MHz and PCLK=64MHz
 * V1.14, Sep, 03,2015 JFT:SSW default clocking changed, MCLK=8MHz and PCLK=16MHz avoid problems with BMI tool timeout
 * V1.15, Jan, 05,2016 JFT:Fix .reset section attributes
 *
 * @endcond 
 */

/*****************************************************************************
 * <h> Clock system handling by SSW
 *   <h> CLK_VAL1 Configuration
 *    <o0.0..7>    FDIV Fractional Divider Selection
 *    <i> Deafult: 0. Fractional part of clock divider, MCLK = DCO1 / (2 x (IDIV + (FDIV / 256)))
 *    <o0.8..15>   IDIV Divider Selection (limited to 1-16)
 *                    <0=> Divider is bypassed
 *                    <1=> MCLK = 32 MHz
 *                    <2=> MCLK = 16 MHz
 *                    <3=> MCLK = 10.67 MHz
 *                    <4=> MCLK = 8 MHz
 *                    <254=> MCLK = 126 kHz
 *                    <255=> MCLK = 125.5 kHz
 *    <i> Deafult: 4. Interger part of clock divider, MCLK = DCO1 / (2 x (IDIV + (FDIV / 256))) = 8MHz
 *    <o0.16>      PCLKSEL PCLK Clock Select
 *                    <0=> PCLK = MCLK
 *                    <1=> PCLK = 2 x MCLK
 *    <i> Deafult: 2 x MCLK
 *    <o0.17..19>  RTCCLKSEL RTC Clock Select
 *                    <0=> 32.768kHz standby clock
 *                    <1=> 32.768kHz external clock from ERU0.IOUT0
 *                    <2=> 32.768kHz external clock from ACMP0.OUT
 *                    <3=> 32.768kHz external clock from ACMP1.OUT
 *                    <4=> 32.768kHz external clock from ACMP2.OUT
 *                    <5=> Reserved
 *                    <6=> Reserved
 *                    <7=> Reserved
 *    <i> Deafult: 32.768kHz standby clock 
 *    <o0.31>      do not move CLK_VAL1 to SCU_CLKCR[0..19]
 *  </h>
 *****************************************************************************/
#define CLKVAL1_SSW 0x00010400

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
 *    <o0.31>   do not move CLK_VAL2 to SCU_CGATCLR0[0..10]
 *  </h>
 *****************************************************************************/
#define CLKVAL2_SSW 0x80000000

/* A macro to define vector table entries */
.macro Entry Handler
    .long \Handler
.endm

/* A couple of macros to ease definition of the various handlers */
.macro Insert_ExceptionHandler Handler_Func 
    .weak \Handler_Func
    .thumb_set \Handler_Func, Default_handler
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
    Entry   HardFault_Handler           /* Hard Fault Handler           */
    .long   CLKVAL1_SSW                 /* Reserved                     */
    .long   CLKVAL2_SSW                 /* Reserved                     */
#ifdef RETAIN_VECTOR_TABLE
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    Entry   SVC_Handler                 /* SVCall Handler               */
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    Entry   PendSV_Handler              /* PendSV Handler               */
    Entry   SysTick_Handler             /* SysTick Handler              */

    /* Interrupt Handlers for Service Requests (SR) from XMC1100 Peripherals */
    Entry   SCU_0_IRQHandler            /* Handler name for SR SCU_0     */
    Entry   SCU_1_IRQHandler            /* Handler name for SR SCU_1     */
    Entry   SCU_2_IRQHandler            /* Handler name for SR SCU_2     */
    Entry   ERU0_0_IRQHandler           /* Handler name for SR ERU0_0    */
    Entry   ERU0_1_IRQHandler           /* Handler name for SR ERU0_1    */
    Entry   ERU0_2_IRQHandler           /* Handler name for SR ERU0_2    */
    Entry   ERU0_3_IRQHandler           /* Handler name for SR ERU0_3    */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    Entry   USIC0_0_IRQHandler          /* Handler name for SR USIC0_0   */
    Entry   USIC0_1_IRQHandler          /* Handler name for SR USIC0_1   */
    Entry   USIC0_2_IRQHandler          /* Handler name for SR USIC0_2   */
    Entry   USIC0_3_IRQHandler          /* Handler name for SR USIC0_3   */
    Entry   USIC0_4_IRQHandler          /* Handler name for SR USIC0_4   */
    Entry   USIC0_5_IRQHandler          /* Handler name for SR USIC0_5   */
    Entry   VADC0_C0_0_IRQHandler       /* Handler name for SR VADC0_C0_0  */
    Entry   VADC0_C0_1_IRQHandler       /* Handler name for SR VADC0_C0_1  */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    Entry   CCU40_0_IRQHandler          /* Handler name for SR CCU40_0   */
    Entry   CCU40_1_IRQHandler          /* Handler name for SR CCU40_1   */
    Entry   CCU40_2_IRQHandler          /* Handler name for SR CCU40_2   */
    Entry   CCU40_3_IRQHandler          /* Handler name for SR CCU40_3   */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
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
Default_handler:
    b  .
    .size Default_handler, . - Default_handler

    Insert_ExceptionHandler HardFault_Handler
    Insert_ExceptionHandler SVC_Handler
    Insert_ExceptionHandler PendSV_Handler
    Insert_ExceptionHandler SysTick_Handler

    Insert_ExceptionHandler SCU_0_IRQHandler
    Insert_ExceptionHandler SCU_1_IRQHandler
    Insert_ExceptionHandler SCU_2_IRQHandler
    Insert_ExceptionHandler ERU0_0_IRQHandler
    Insert_ExceptionHandler ERU0_1_IRQHandler
    Insert_ExceptionHandler ERU0_2_IRQHandler
    Insert_ExceptionHandler ERU0_3_IRQHandler
    Insert_ExceptionHandler VADC0_C0_0_IRQHandler
    Insert_ExceptionHandler VADC0_C0_1_IRQHandler
    Insert_ExceptionHandler CCU40_0_IRQHandler
    Insert_ExceptionHandler CCU40_1_IRQHandler
    Insert_ExceptionHandler CCU40_2_IRQHandler
    Insert_ExceptionHandler CCU40_3_IRQHandler
    Insert_ExceptionHandler USIC0_0_IRQHandler
    Insert_ExceptionHandler USIC0_1_IRQHandler
    Insert_ExceptionHandler USIC0_2_IRQHandler
    Insert_ExceptionHandler USIC0_3_IRQHandler
    Insert_ExceptionHandler USIC0_4_IRQHandler
    Insert_ExceptionHandler USIC0_5_IRQHandler
   
/* ======================================================================== */

/* ==================VENEERS VENEERS VENEERS VENEERS VENEERS=============== */
    .section ".XmcVeneerCode","ax",%progbits
    
    .align 1
    
    .globl HardFault_Veneer
HardFault_Veneer:
    LDR R0, =HardFault_Handler
    MOV PC,R0
    .long 0
    .long 0
    .long 0
    .long 0
    .long 0
    .long 0
    .long 0
    
/* ======================================================================== */
    .globl SVC_Veneer
SVC_Veneer:
    LDR R0, =SVC_Handler
    MOV PC,R0
    .long 0
    .long 0
/* ======================================================================== */
    .globl PendSV_Veneer
PendSV_Veneer:
    LDR R0, =PendSV_Handler
    MOV PC,R0
/* ======================================================================== */
    .globl SysTick_Veneer 
SysTick_Veneer:
    LDR R0, =SysTick_Handler
    MOV PC,R0
/* ======================================================================== */
    .globl SCU_0_Veneer 
SCU_0_Veneer:
    LDR R0, =SCU_0_IRQHandler
    MOV PC,R0
/* ======================================================================== */
    .globl SCU_1_Veneer 
SCU_1_Veneer:
    LDR R0, =SCU_1_IRQHandler
    MOV PC,R0
/* ======================================================================== */
    .globl SCU_2_Veneer
SCU_2_Veneer:
    LDR R0, =SCU_2_IRQHandler
    MOV PC,R0
/* ======================================================================== */
    .globl SCU_3_Veneer 
SCU_3_Veneer:
    LDR R0, =ERU0_0_IRQHandler
    MOV PC,R0
/* ======================================================================== */
    .globl SCU_4_Veneer 
SCU_4_Veneer:
    LDR R0, =ERU0_1_IRQHandler
    MOV PC,R0
/* ======================================================================== */
    .globl SCU_5_Veneer 
SCU_5_Veneer:
    LDR R0, =ERU0_2_IRQHandler
    MOV PC,R0
/* ======================================================================== */
    .globl SCU_6_Veneer 
SCU_6_Veneer:
    LDR R0, =ERU0_3_IRQHandler
    MOV PC,R0
    .long 0
    .long 0
/* ======================================================================== */
    .globl USIC0_0_Veneer
USIC0_0_Veneer:
    LDR R0, =USIC0_0_IRQHandler
    MOV PC,R0
/* ======================================================================== */
    .globl USIC0_1_Veneer
USIC0_1_Veneer:
    LDR R0, =USIC0_1_IRQHandler
    MOV PC,R0
/* ======================================================================== */
    .globl USIC0_2_Veneer
USIC0_2_Veneer:
    LDR R0, =USIC0_2_IRQHandler
    MOV PC,R0
/* ======================================================================== */
    .globl USIC0_3_Veneer
USIC0_3_Veneer:
    LDR R0, =USIC0_3_IRQHandler
    MOV PC,R0
/* ======================================================================== */
    .globl USIC0_4_Veneer
USIC0_4_Veneer:
    LDR R0, =USIC0_4_IRQHandler
    MOV PC,R0
/* ======================================================================== */
    .globl USIC0_5_Veneer
USIC0_5_Veneer:
    LDR R0, =USIC0_5_IRQHandler
    MOV PC,R0
/* ======================================================================== */
    .globl VADC0_C0_0_Veneer 
VADC0_C0_0_Veneer:
    LDR R0, =VADC0_C0_0_IRQHandler
    MOV PC,R0
/* ======================================================================== */
    .globl VADC0_C0_1_Veneer
VADC0_C0_1_Veneer:
    LDR R0, =VADC0_C0_1_IRQHandler
    MOV PC,R0
    .long 0
    .long 0
    .long 0
    .long 0
/* ======================================================================== */
    .globl CCU40_0_Veneer
CCU40_0_Veneer:
    LDR R0, =CCU40_0_IRQHandler
    MOV PC,R0
/* ======================================================================== */
    .globl CCU40_1_Veneer
CCU40_1_Veneer:
    LDR R0, =CCU40_1_IRQHandler
    MOV PC,R0
/* ======================================================================== */
    .globl CCU40_2_Veneer
CCU40_2_Veneer:
    LDR R0, =CCU40_2_IRQHandler
    MOV PC,R0
/* ======================================================================== */
    .globl CCU40_3_Veneer
CCU40_3_Veneer:
    LDR R0, =CCU40_3_IRQHandler
    MOV PC,R0
    .long 0
    .long 0
    .long 0
    .long 0
    .long 0
    .long 0
    .long 0

/* ======================================================================== */
/* ======================================================================== */

/* ============= END OF INTERRUPT HANDLER DEFINITION ======================== */

    .end
