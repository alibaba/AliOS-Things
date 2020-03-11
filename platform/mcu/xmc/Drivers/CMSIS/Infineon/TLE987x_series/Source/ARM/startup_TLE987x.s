;*******************************************************************************
;* @file     startup_TLE987x.s
;* @brief    CMSIS Core Device Startup File for
;*           Infineon TLE987x Device Series
;* @version  V1.0
;* @date     September 2012
;*
;* Copyright (C) 2014 Infineon Technologies AG. All rights reserved.
;*
;*
;* @par
;* Infineon Technologies AG (Infineon) is supplying this software for use with 
;* Infineon's microcontrollers.  This file can be freely distributed
;* within development tools that are supporting such microcontrollers.
;*
;* @par
;* THIS SOFTWARE IS PROVIDED AS IS.  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
;* OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
;* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
;* ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
;* CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
;*
;******************************************************************************

;************************** Version History ************************************
; V1.0, Sep,  9, 2012 DM: initial version
;*******************************************************************************
Stack_Size      EQU     0x200

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp

; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

                 THUMB
                 PRESERVE8

                
;*******************************************************************************
; Fill-up the Vector Table entries with the exceptions ISR address
;*******************************************************************************
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size
                      
__Vectors       DCD     __initial_sp
                DCD     Reset_Handler
                DCD     NMI_Handler                ; NMI Handler
                DCD     HardFault_Handler          ; Hard Fault Handler
                DCD     MemManage_Handler          ; MPU Fault Handler
                DCD     BusFault_Handler           ; Bus Fault Handler
                DCD     UsageFault_Handler         ; Usage Fault Handler
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     0                          ; Reserved
                DCD     SVC_Handler                ; SVCall Handler
                DCD     DebugMon_Handler           ; Debug Monitor Handler
                DCD     0                          ; Reserved
                DCD     PendSV_Handler             ; PendSV Handler
                DCD     SysTick_Handler            ; SysTick Handler

				  ; External Interrupts
                DCD     GPT1_IRQHandler            ; 0:  GPT1
             	DCD     GPT2_IRQHandler			   ; 1:  GPT2
				DCD     ADC2_IRQHandler  		   ; 2:  Measurement Unit, Timer3
				DCD     ADC1_IRQHandler			   ; 3:  10 bit ADC
			    DCD     CCU6SR0_IRQHandler         ; 4:  CCU0
				DCD     CCU6SR1_IRQHandler         ; 5:  CCU1
				DCD     CCU6SR2_IRQHandler         ; 6:  CCU2
				DCD     CCU6SR3_IRQHandler         ; 7:  CCU3
				DCD     SSC1_IRQHandler 		   ; 8:  SSC1
				DCD     SSC2_IRQHandler 		   ; 9:  SSC2
				DCD     UART1_IRQHandler       	   ; 10: UART1
				DCD     UART2_IRQHandler		   ; 11: UART2
				DCD     EXINT0_IRQHandler          ; 12: EXINT0
				DCD     EXINT1_IRQHandler          ; 13: EXINT1
				DCD     BDRV_IRQHandler            ; 14: BDRV
				DCD     DMA_IRQHandler             ; 15: DMA
__Vectors_End

__Vectors_Size 	EQU 	__Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY

;-------------------------------------------------------------------------------
; Reset Handler
;
Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP

;-------------------------------------------------------------------------------
; Dummy Exception Handlers (infinite loops which can be modified)                

NMI_Handler     PROC
                EXPORT  NMI_Handler                [WEAK]
				B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler          [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler          [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler           [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler         [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler                [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler           [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler             [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler            [WEAK]
                B       .
                ENDP

Default_Handler PROC
                EXPORT  GPT1_IRQHandler            [WEAK]
                EXPORT  GPT2_IRQHandler            [WEAK]
				EXPORT  ADC2_IRQHandler			   [WEAK]
				EXPORT  ADC1_IRQHandler			   [WEAK]
   		        EXPORT  CCU6SR0_IRQHandler         [WEAK]
			    EXPORT  CCU6SR1_IRQHandler         [WEAK]
			    EXPORT  CCU6SR2_IRQHandler         [WEAK]
			    EXPORT  CCU6SR3_IRQHandler         [WEAK]
				EXPORT  SSC1_IRQHandler            [WEAK]
				EXPORT  SSC2_IRQHandler            [WEAK]
			    EXPORT  UART1_IRQHandler           [WEAK]
			    EXPORT  UART2_IRQHandler           [WEAK]
			    EXPORT  EXINT0_IRQHandler          [WEAK]
			    EXPORT  EXINT1_IRQHandler          [WEAK]
				EXPORT  BDRV_IRQHandler            [WEAK]
                EXPORT  DMA_IRQHandler             [WEAK]
GPT1_IRQHandler
GPT2_IRQHandler
ADC2_IRQHandler
ADC1_IRQHandler
CCU6SR0_IRQHandler
CCU6SR1_IRQHandler
CCU6SR2_IRQHandler
CCU6SR3_IRQHandler
SSC1_IRQHandler
SSC2_IRQHandler
UART1_IRQHandler
UART2_IRQHandler
EXINT0_IRQHandler
EXINT1_IRQHandler
BDRV_IRQHandler
DMA_IRQHandler
                B       .
                ENDP

;-------------------------------------------------------------------------------
; User Initial Stack & Heap

                IF      :DEF:__MICROLIB
                
                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit
                
                ELSE
                
                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap
__user_initial_stackheap

                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR

                ALIGN

                ENDIF

                 END

