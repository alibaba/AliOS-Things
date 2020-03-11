;*********************************************************************************************************************
;* @file     startup_XMC1400.s
;* @brief    CMSIS Core Device Startup File for Infineon XMC1400 Device Series
;* @version  V1.1
;* @date     15 Sep 2017
;*
;* @cond
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
;**************************** Change history ********************************
;* V1.0, Sep, 03, 2015 JFT:Initial version
;*                         MCLK=8MHz, PCLK=16MHz
;*
;* V1.1, Sep, 15, 2017 JFT:Added option to select wait time before ASC BSL channel selection (WAIT_ASCBSL_ENTRY_SSW)
;*
;* @endcond 
;*

; ------------------ <<< Use Configuration Wizard in Context Menu >>> ------------------
           
; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000200

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

;****************************************************************************
; <h> Clock system handling by SSW
;   <h> CLK_VAL1 Configuration
;    <o0.0..9>    FDIV Fractional Divider Selection <0-1023>
;    <i> Deafult: 0. Fractional part of clock divider, MCLK = DCO1 / (IDIV + (FDIV / 1024))
;    <o0.10..17>  IDIV Divider Selection <1-16>
;    <i> Deafult: 6. Interger part of clock divider, MCLK = DCO1 / (IDIV + (FDIV / 1024) = 8MHz)
;    <o0.18>      PCLKSEL PCLK Clock Select
;                    <0=> PCLK = MCLK
;                    <1=> PCLK = 2 x MCLK
;    <i> Deafult: 2 x MCLK
;    <o0.19..21>  RTCCLKSEL RTC Clock Select
;                    <0=> 32.768kHz standby clock
;                    <1=> 32.768kHz external clock from ERU0.IOUT0
;                    <2=> 32.768kHz external clock from ACMP0.OUT
;                    <3=> 32.768kHz external clock from ACMP1.OUT
;                    <4=> 32.768kHz external clock from ACMP2.OUT
;                    <5=> 32.768kHz XTAL clock via OSC_LP
;                    <6=> Reserved
;                    <7=> Reserved
;    <i> Deafult: 32.768kHz standby clock
;    <o0.31>      do not move CLK_VAL1 to SCU_CLKCR[0..19]
;  </h>
;*****************************************************************************
CLK_VAL1_Val    EQU     0x00041800

;*****************************************************************************
;  <h> CLK_VAL2 Configuration
;    <o0.0>    disable VADC and SHS Gating
;    <o0.1>    disable CCU80 Gating
;    <o0.2>    disable CCU40 Gating
;    <o0.3>    disable USIC0 Gating
;    <o0.4>    disable BCCU0 Gating
;    <o0.5>    disable LEDTS0 Gating
;    <o0.6>    disable LEDTS1 Gating
;    <o0.7>    disable POSIF0 Gating
;    <o0.8>    disable MATH Gating
;    <o0.9>    disable WDT Gating
;    <o0.10>   disable RTC Gating
;    <o0.16>   disable CCU81 Gating
;    <o0.17>   disable CCU41 Gating 
;    <o0.18>   disable USIC1 Gating
;    <o0.19>   disable LEDTS2 Gating 
;    <o0.20>   disable POSIF1 Gating
;    <o0.21>   disable MCAN0 Gating
;    <o0.31>   do not move CLK_VAL2 to SCU_CGATCLR0[0..10]
;  </h>
;*****************************************************************************
CLK_VAL2_Val    EQU     0x00000100

;*****************************************************************************
;*  <h> WAIT_ASCBSL_ENTRY Configuration
;*    <o0.0..30>    Wait time before ASC BSL channel selection (number of MCLK clock cycles)
;*    <o0.31>       Disable wait time before ASC BSL channel selection
;*  </h>
;*****************************************************************************
WAIT_ASCBSL_ENTRY_SSW_Val EQU 0x80000000
; </h>

                PRESERVE8
                THUMB

; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp              ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     CLK_VAL1_Val              ; CLK_VAL1
                DCD     CLK_VAL2_Val              ; CLK_VAL2
                DCD     WAIT_ASCBSL_ENTRY_SSW_Val ; WAIT_ASCBSL_ENTRY_SSW
__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
; Following code initializes the Veneers at address 0x20000000 with a "branch to itself"
; The real veneers will be copied later from the scatter loader before reaching main.
; This init code should handle an exception before the real veneers are copied.
SRAM_BASE            EQU     0x20000000
VENEER_INIT_CODE     EQU     0xE7FEBF00             ; NOP, B .

                LDR     R1, =SRAM_BASE
                LDR     R2, =VENEER_INIT_CODE                
                MOVS    R0, #48                     ; Veneer 0..47
Init_Veneers
                STR     R2, [R1]
                ADDS    R1, #4
                SUBS    R0, R0, #1
                BNE     Init_Veneers
                
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP

Default_Handler PROC

                EXPORT  HardFault_Handler       [WEAK]
                EXPORT  SVC_Handler             [WEAK]
                EXPORT  PendSV_Handler          [WEAK]
                EXPORT  SysTick_Handler         [WEAK]
                EXPORT  IRQ0_Handler            [WEAK]		; SCU.SR0, CAN0.SR0, CCU40.SR0, SCU.SR0 | CAN0.SR0
                EXPORT  IRQ1_Handler            [WEAK]      ; SCU.SR1, CAN0.SR1, CCU80.SR0, SCU.SR1 | CAN0.SR1
                EXPORT  IRQ2_Handler            [WEAK]      ; SCU.SR2, CAN0.SR2, CCU80.SR1, SCU.SR2 | CAN0.SR2
                EXPORT  IRQ3_Handler            [WEAK]      ; ERU0.SR0, ERU1.SR0, CAN0.SR0, ERU0.SR0 | ERU1.SR0
                EXPORT  IRQ4_Handler            [WEAK]      ; ERU0.SR1, ERU1.SR1, CAN0.SR1, ERU0.SR1 | ERU1.SR1
                EXPORT  IRQ5_Handler            [WEAK]      ; ERU0.SR2, ERU1.SR2, CAN0.SR2, ERU0.SR2 | ERU1.SR2
                EXPORT  IRQ6_Handler            [WEAK]      ; ERU0.SR3, ERU1.SR3, CAN0.SR3, ERU0.SR3 | ERU1.SR3
                EXPORT  IRQ7_Handler            [WEAK]      ; MATH.SR0, CAN0.SR3, CCU40.SR1, MATH.SR0 | CAN0.SR3
                EXPORT  IRQ8_Handler            [WEAK]      ; LEDTS2.SR0, CCU40.SR0, CCU80.SR0, LEDTS2.SR0 | CCU40.SR0
                EXPORT  IRQ9_Handler            [WEAK]      ; USIC0.SR0, USIC1.SR0, ERU0.SR0, USIC0.SR0 | USIC1.SR0
                EXPORT  IRQ10_Handler           [WEAK]      ; USIC0.SR1, USIC1.SR1, ERU0.SR1, USIC0.SR1 | USIC1.SR1
                EXPORT  IRQ11_Handler           [WEAK]      ; USIC0.SR2, USIC1.SR2, ERU0.SR2, USIC0.SR2 | USIC1.SR2
                EXPORT  IRQ12_Handler           [WEAK]      ; USIC0.SR3, USIC1.SR3, ERU0.SR3, USIC0.SR3 | USIC1.SR3
                EXPORT  IRQ13_Handler           [WEAK]      ; USIC0.SR4, USIC1.SR4, CCU80.SR1, USIC0.SR4 | USIC1.SR4
                EXPORT  IRQ14_Handler           [WEAK]      ; USIC0.SR5, USIC1.SR5, POSIF0.SR0, USIC0.SR5 | USIC1.SR5
                EXPORT  IRQ15_Handler           [WEAK]      ; VADC0.C0SR0, USIC0.SR0, POSIF0.SR1, VADC0.C0SR0 | USIC0.SR0
                EXPORT  IRQ16_Handler           [WEAK]      ; VADC0.C0SR1, USIC0.SR1, CCU40.SR2, VADC0.C0SR1 | USIC0.SR1
                EXPORT  IRQ17_Handler           [WEAK]      ; VADC0.G0SR0, USIC0.SR2, CAN0.SR0, VADC0.G0SR0 | USIC0.SR2
                EXPORT  IRQ18_Handler           [WEAK]      ; VADC0.G0SR1, USIC0.SR3, CAN0.SR1, VADC0.G0SR1 | USIC0.SR3
                EXPORT  IRQ19_Handler           [WEAK]      ; VADC0.G1SR0, USIC0.SR4, CAN0.SR2, VADC0.G1SR0 | USIC0.SR4
                EXPORT  IRQ20_Handler           [WEAK]      ; VADC0.G1SR1, USIC0.SR5, CAN0.SR3, VADC0.G1SR1 | USIC0.SR5
                EXPORT  IRQ21_Handler           [WEAK]      ; CCU40.SR0, CCU41.SR0, USIC0.SR0, CCU40.SR0 | CCU41.SR0
                EXPORT  IRQ22_Handler           [WEAK]      ; CCU40.SR1, CCU41.SR1, USIC0.SR1, CCU40.SR1 | CCU41.SR1
                EXPORT  IRQ23_Handler           [WEAK]      ; CCU40.SR2, CCU41.SR2, USIC0.SR2, CCU40.SR2 | CCU41.SR2
                EXPORT  IRQ24_Handler           [WEAK]      ; CCU40.SR3, CCU41.SR3, USIC0.SR3, CCU40.SR3 | CCU41.SR3
                EXPORT  IRQ25_Handler           [WEAK]      ; CCU80.SR0, CCU81.SR0, USIC0.SR4, CCU80.SR0 | CCU81.SR0
                EXPORT  IRQ26_Handler           [WEAK]      ; CCU80.SR1, CCU81.SR1, USIC0.SR5, CCU80.SR1 | CCU81.SR1
                EXPORT  IRQ27_Handler           [WEAK]      ; POSIF0.SR0, POSIF1.SR0, CCU40.SR3, POSIF0.SR0 | POSIF1.SR0
                EXPORT  IRQ28_Handler           [WEAK]      ; POSIF0.SR1, POSIF1.SR1, ERU0.SR0, POSIF0.SR1 | POSIF1.SR1
                EXPORT  IRQ29_Handler           [WEAK]      ; LEDTS0.SR0, CCU40.SR1, ERU0.SR1, LEDTS0.SR0 | CCU40.SR1
                EXPORT  IRQ30_Handler           [WEAK]      ; LEDTS1.SR0, CCU40.SR2, ERU0.SR2, LEDTS1.SR0 | CCU40.SR2
                EXPORT  IRQ31_Handler           [WEAK]      ; BCCU0.SR0, CCU40.SR3, ERU0.SR3, BCCU0.SR0 | CCU40.SR3

HardFault_Handler       
SVC_Handler             
PendSV_Handler          
SysTick_Handler         
IRQ0_Handler        
IRQ1_Handler        
IRQ2_Handler        
IRQ3_Handler       
IRQ4_Handler       
IRQ5_Handler       
IRQ6_Handler
IRQ7_Handler       
IRQ8_Handler      
IRQ9_Handler      
IRQ10_Handler      
IRQ11_Handler      
IRQ12_Handler      
IRQ13_Handler      
IRQ14_Handler   
IRQ15_Handler
IRQ16_Handler
IRQ17_Handler
IRQ18_Handler
IRQ19_Handler
IRQ20_Handler      
IRQ21_Handler      
IRQ22_Handler      
IRQ23_Handler
IRQ24_Handler
IRQ25_Handler
IRQ26_Handler
IRQ27_Handler
IRQ28_Handler
IRQ29_Handler
IRQ30_Handler
IRQ31_Handler
                B       .

                ENDP


                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap

__user_initial_stackheap PROC
                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR
                ENDP

                ALIGN

                ENDIF


;* ================== START OF INTERRUPT HANDLER VENEERS ==================== */
; Veneers are located to fix SRAM Address 0x2000'000C
                AREA    |.ARM.__at_0x2000000C|, CODE, READWRITE

; Each Veneer has exactly a length of 4 Byte

                MACRO
                JUMPTO $Handler
                LDR  R0, =$Handler
                BX   R0
                MEND

                JUMPTO   HardFault_Handler            
                DCD      0                            
                DCD      0                            
                DCD      0                            
                DCD      0                            
                DCD      0                            
                DCD      0                            
                DCD      0                            
                JUMPTO   SVC_Handler                  
                DCD      0                            
                DCD      0                            
                JUMPTO   PendSV_Handler               
                JUMPTO   SysTick_Handler              
                JUMPTO   IRQ0_Handler                 
                JUMPTO   IRQ1_Handler                 
                JUMPTO   IRQ2_Handler                 
                JUMPTO   IRQ3_Handler                 
                JUMPTO   IRQ4_Handler                 
                JUMPTO   IRQ5_Handler                 
                JUMPTO   IRQ6_Handler                 
                JUMPTO   IRQ7_Handler                 
                JUMPTO   IRQ8_Handler                 
                JUMPTO   IRQ9_Handler                 
                JUMPTO   IRQ10_Handler                
                JUMPTO   IRQ11_Handler                
                JUMPTO   IRQ12_Handler                
                JUMPTO   IRQ13_Handler                
                JUMPTO   IRQ14_Handler                
                JUMPTO   IRQ15_Handler                
                JUMPTO   IRQ16_Handler                
                JUMPTO   IRQ17_Handler                
                JUMPTO   IRQ18_Handler                
                JUMPTO   IRQ19_Handler                
                JUMPTO   IRQ20_Handler                
                JUMPTO   IRQ21_Handler                
                JUMPTO   IRQ22_Handler                
                JUMPTO   IRQ23_Handler                
                JUMPTO   IRQ24_Handler                
                JUMPTO   IRQ25_Handler                
                JUMPTO   IRQ26_Handler                
                JUMPTO   IRQ27_Handler                
                JUMPTO   IRQ28_Handler                
                JUMPTO   IRQ29_Handler                
                JUMPTO   IRQ30_Handler                
                JUMPTO   IRQ31_Handler                

                ALIGN

;* ================== END OF INTERRUPT HANDLER VENEERS ====================== */

                END
