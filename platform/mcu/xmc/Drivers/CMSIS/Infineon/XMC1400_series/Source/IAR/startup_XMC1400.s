;************************************************
;*
;* Part one of the system initialization code, contains low-level
;* initialization, plain thumb variant.
;*
;*********************************************************************************************************************
;* Copyright (c) 2015-2017, Infineon Technologies AG
;* All rights reserved.                        
;*                                             
;* Redistribution and use in source and binary forms, with or without modification,are permitted provided that the 
;* following conditions are met:   
;*                                                                              
;* Redistributions of source code must retain the above copyright notice, this list of conditions and the following 
;* disclaimer.                        
;* 
;* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following 
;* disclaimer in the documentation and/or other materials provided with the distribution.                       
;* 
;* Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote 
;* products derived from this software without specific prior written permission.                                           
;*                                                                              
;* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
;* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE  
;* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
;* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR  
;* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
;* WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
;* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                                                  
;*                                                                              
;* To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes with 
;* Infineon Technologies AG dave@infineon.com).                                                          
;*********************************************************************************************************************
;*
;******************* Version History **********************************************   
;
;  V1, May,  7,2015 JFT:a) Initial version, MCLK=8MHz, PCLK=16MHz
;  V2, Sep, 15,2017 JFT:a) JFT:Added option to select wait time before ASC BSL channel selection (WAIT_ASCBSL_ENTRY_SSW)
;     
;**********************************************************************************
;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; Cortex-M version
;

        MODULE  ?cstartup

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

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)
        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
        PUBLIC  __vector_table

        DATA
__vector_table
        DCD     sfe(CSTACK)
        DCD     Reset_Handler             ; Reset Handler
        DCD     0                         ; 0x8
        DCD     0                         ; 0xC
        DCD     CLKVAL1_SSW               ; 0x10 CLK_VAL1 - (CLKCR default)
        DCD     CLKVAL2_SSW               ; 0x14 CLK_VAL2 - (CGATCLR0 default)
        DCD     WAIT_ASCBSL_ENTRY_SSW     ; 0x18 WAIT_ASCBSL_ENTRY_SSW 

        SECTION .vect_table:CODE:ROOT(2)
        THUMB
        LDR     R0,=HardFault_Handler
        BX      R0
        LDR     R0,=Undef_Handler
        BX      R0
        LDR     R0,=Undef_Handler
        BX      R0
        LDR     R0,=Undef_Handler
        BX      R0
        LDR     R0,=Undef_Handler
        BX      R0
        LDR     R0,=Undef_Handler
        BX      R0
        LDR     R0,=Undef_Handler
        BX      R0
        LDR     R0,=Undef_Handler
        BX      R0
        LDR     R0,=SVC_Handler
        BX      R0
        LDR     R0,=Undef_Handler
        BX      R0
        LDR     R0,=Undef_Handler
        BX      R0
        LDR     R0,=PendSV_Handler
        BX      R0
        LDR     R0,=SysTick_Handler
        BX      R0

        ; External Interrupts
        LDR     R0,=IRQ0_Handler          ; SCU.SR0, CAN0.SR0, CCU40.SR0, SCU.SR0 | CAN0.SR0
        BX      R0
        LDR     R0,=IRQ1_Handler          ; SCU.SR1, CAN0.SR1, CCU80.SR0, SCU.SR1 | CAN0.SR1
        BX      R0
        LDR     R0,=IRQ2_Handler          ; SCU.SR2, CAN0.SR2, CCU80.SR1, SCU.SR2 | CAN0.SR2
        BX      R0
        LDR     R0,=IRQ3_Handler          ; ERU0.SR0, ERU1.SR0, CAN0.SR0, ERU0.SR0 | ERU1.SR0
        BX      R0
        LDR     R0,=IRQ4_Handler          ; ERU0.SR1, ERU1.SR1, CAN0.SR1, ERU0.SR1 | ERU1.SR1
        BX      R0
        LDR     R0,=IRQ5_Handler          ; ERU0.SR2, ERU1.SR2, CAN0.SR2, ERU0.SR2 | ERU1.SR2
        BX      R0
        LDR     R0,=IRQ6_Handler          ; ERU0.SR3, ERU1.SR3, CAN0.SR3, ERU0.SR3 | ERU1.SR3
        BX      R0
        LDR     R0,=IRQ7_Handler          ; MATH.SR0, CAN0.SR3, CCU40.SR1, MATH.SR0 | CAN0.SR3
        BX      R0
        LDR     R0,=IRQ8_Handler          ; LEDTS2.SR0, CCU40.SR0, CCU80.SR0, LEDTS2.SR0 | CCU40.SR0
        BX      R0
        LDR     R0,=IRQ9_Handler          ; USIC0.SR0, USIC1.SR0, ERU0.SR0, USIC0.SR0 | USIC1.SR0
        BX      R0
        LDR     R0,=IRQ10_Handler         ; USIC0.SR1, USIC1.SR1, ERU0.SR1, USIC0.SR1 | USIC1.SR1
        BX      R0
        LDR     R0,=IRQ11_Handler         ; USIC0.SR2, USIC1.SR2, ERU0.SR2, USIC0.SR2 | USIC1.SR2
        BX      R0
        LDR     R0,=IRQ12_Handler         ; USIC0.SR3, USIC1.SR3, ERU0.SR3, USIC0.SR3 | USIC1.SR3
        BX      R0
        LDR     R0,=IRQ13_Handler         ; USIC0.SR4, USIC1.SR4, CCU80.SR1, USIC0.SR4 | USIC1.SR4
        BX      R0
        LDR     R0,=IRQ14_Handler         ; USIC0.SR5, USIC1.SR5, POSIF0.SR0, USIC0.SR5 | USIC1.SR5
        BX      R0
        LDR     R0,=IRQ15_Handler         ; VADC0.C0SR0, USIC0.SR0, POSIF0.SR1, VADC0.C0SR0 | USIC0.SR0
        BX      R0
        LDR     R0,=IRQ16_Handler         ; VADC0.C0SR1, USIC0.SR1, CCU40.SR2, VADC0.C0SR1 | USIC0.SR1
        BX      R0
        LDR     R0,=IRQ17_Handler         ; VADC0.G0SR0, USIC0.SR2, CAN0.SR0, VADC0.G0SR0 | USIC0.SR2
        BX      R0
        LDR     R0,=IRQ18_Handler         ; VADC0.G0SR1, USIC0.SR3, CAN0.SR1, VADC0.G0SR1 | USIC0.SR3
        BX      R0
        LDR     R0,=IRQ19_Handler         ; VADC0.G1SR0, USIC0.SR4, CAN0.SR2, VADC0.G1SR0 | USIC0.SR4
        BX      R0
        LDR     R0,=IRQ20_Handler         ; VADC0.G1SR1, USIC0.SR5, CAN0.SR3, VADC0.G1SR1 | USIC0.SR5
        BX      R0
        LDR     R0,=IRQ21_Handler         ; CCU40.SR0, CCU41.SR0, USIC0.SR0, CCU40.SR0 | CCU41.SR0
        BX      R0
        LDR     R0,=IRQ22_Handler         ; CCU40.SR1, CCU41.SR1, USIC0.SR1, CCU40.SR1 | CCU41.SR1
        BX      R0
        LDR     R0,=IRQ23_Handler         ; CCU40.SR2, CCU41.SR2, USIC0.SR2, CCU40.SR2 | CCU41.SR2
        BX      R0
        LDR     R0,=IRQ24_Handler         ; CCU40.SR3, CCU41.SR3, USIC0.SR3, CCU40.SR3 | CCU41.SR3
        BX      R0
        LDR     R0,=IRQ25_Handler         ; CCU80.SR0, CCU81.SR0, USIC0.SR4, CCU80.SR0 | CCU81.SR0
        BX      R0
        LDR     R0,=IRQ26_Handler         ; CCU80.SR1, CCU81.SR1, USIC0.SR5, CCU80.SR1 | CCU81.SR1
        BX      R0
        LDR     R0,=IRQ27_Handler         ; POSIF0.SR0, POSIF1.SR0, CCU40.SR3, POSIF0.SR0 | POSIF1.SR0
        BX      R0
        LDR     R0,=IRQ28_Handler         ; POSIF0.SR1, POSIF1.SR1, ERU0.SR0, POSIF0.SR1 | POSIF1.SR1
        BX      R0
        LDR     R0,=IRQ29_Handler         ; LEDTS0.SR0, CCU40.SR1, ERU0.SR1, LEDTS0.SR0 | CCU40.SR1
        BX      R0
        LDR     R0,=IRQ30_Handler         ; LEDTS1.SR0, CCU40.SR2, ERU0.SR2, LEDTS1.SR0 | CCU40.SR2
        BX      R0
        LDR     R0,=IRQ31_Handler         ; BCCU0.SR0, CCU40.SR3, ERU0.SR3, BCCU0.SR0 | CCU40.SR3
        BX      R0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        EXTERN  SystemInit
        SECTION .text:CODE:NOROOT(2)

        THUMB

        PUBWEAK Reset_Handler
        SECTION .text:CODE:REORDER:NOROOT(2)
Reset_Handler
        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0

        PUBWEAK Undef_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
Undef_Handler
        B Undef_Handler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HardFault_Handler
        B HardFault_Handler


        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler
        B SVC_Handler


        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler
        B PendSV_Handler


        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SysTick_Handler
        B SysTick_Handler


        PUBWEAK IRQ0_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ0_Handler
        B IRQ0_Handler

        PUBWEAK IRQ1_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ1_Handler
        B IRQ1_Handler


        PUBWEAK IRQ2_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ2_Handler
        B IRQ2_Handler


        PUBWEAK IRQ3_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ3_Handler
        B IRQ3_Handler


        PUBWEAK IRQ4_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ4_Handler
        B IRQ4_Handler


        PUBWEAK IRQ5_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ5_Handler
        B IRQ5_Handler


        PUBWEAK IRQ6_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ6_Handler
        B IRQ6_Handler


        PUBWEAK IRQ7_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ7_Handler
        B IRQ7_Handler


        PUBWEAK IRQ8_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ8_Handler
        B IRQ8_Handler


        PUBWEAK IRQ9_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ9_Handler
        B IRQ9_Handler


        PUBWEAK IRQ10_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ10_Handler
        B IRQ10_Handler


        PUBWEAK IRQ11_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ11_Handler
        B IRQ11_Handler


        PUBWEAK IRQ12_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ12_Handler
        B IRQ12_Handler


        PUBWEAK IRQ13_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ13_Handler
        B IRQ13_Handler


        PUBWEAK IRQ14_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ14_Handler
        B IRQ14_Handler


        PUBWEAK IRQ15_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ15_Handler
        B IRQ15_Handler


        PUBWEAK IRQ16_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ16_Handler
        B IRQ16_Handler


        PUBWEAK IRQ17_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ17_Handler
        B IRQ17_Handler

		
        PUBWEAK IRQ18_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ18_Handler
        B IRQ18_Handler

		
        PUBWEAK IRQ19_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ19_Handler
        B IRQ19_Handler

		
        PUBWEAK IRQ20_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ20_Handler
        B IRQ20_Handler

		
        PUBWEAK IRQ21_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ21_Handler
        B IRQ21_Handler
		
		
        PUBWEAK IRQ22_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ22_Handler
        B IRQ22_Handler

		
        PUBWEAK IRQ23_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ23_Handler
        B IRQ23_Handler

		
        PUBWEAK IRQ24_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ24_Handler
        B IRQ24_Handler

		
        PUBWEAK IRQ25_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ25_Handler
        B IRQ25_Handler

		
        PUBWEAK IRQ26_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ26_Handler
        B IRQ26_Handler

		
        PUBWEAK IRQ27_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ27_Handler
        B IRQ27_Handler

		
        PUBWEAK IRQ28_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ28_Handler
        B IRQ28_Handler

		
        PUBWEAK IRQ29_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ29_Handler
        B IRQ29_Handler

		
        PUBWEAK IRQ30_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ30_Handler
        B IRQ30_Handler

		
        PUBWEAK IRQ31_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
IRQ31_Handler
        B IRQ31_Handler

        END
