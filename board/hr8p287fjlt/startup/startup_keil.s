;*******************************************************************************
; *file       : startup_8p287.s
; *description: Hr8p287 Device Startup File 
; *author     : Eastsoft MCU Software Team
; *version    : V0.01
; *data       : 01/18/2016
;
; *Copyright (C) 2016 Shanghai Eastsoft Microelectronics Co., Ltd.
; *
; * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
; * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
;*******************************************************************************


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

Heap_Size       EQU     0x00000000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors

__Vectors       DCD     __initial_sp              ; Top of Stack
                DCD     Reset_IRQHandler             ; Reset Handler
                DCD     NMI_IRQHandler               ; NMI Handler
                DCD     HardFault_IRQHandler         ; HardFault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     SVC_IRQHandler               ; SVCall Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     PendSV_Handler            ; PendSV Handler
                DCD     SysTick_IRQHandler           ; SysTick Handler

                ; External Interrupts
                DCD     PINT0_IRQHandler          ;16, PINT0 IRQHandler
                DCD     PINT1_IRQHandler          ;17, PINT1 IRQHandler
                DCD     PINT2_IRQHandler          ;18, PINT2 IRQHandler
                DCD     PINT3_IRQHandler          ;19, PINT3 IRQHandler
                DCD     PINT4_IRQHandler          ;20, PINT4 IRQHandler
                DCD     PINT5_IRQHandler          ;21, PINT5 IRQHandler
                DCD     PINT6_IRQHandler          ;22, PINT6 IRQHandler
                DCD     PINT7_IRQHandler          ;23, PINT7 IRQHandler
                DCD     T16N0_IRQHandler          ;24, T16N0 IRQHandler
                DCD     T16N1_IRQHandler          ;25, T16N1 IRQHandler
                DCD     T16N2_IRQHandler          ;26, T16N2 IRQHandler
                DCD     T16N3_IRQHandler          ;27, T16N3 IRQHandler
                DCD     T32N0_IRQHandler          ;28, T32N0 IRQHandler
                DCD     T32N1_IRQHandler          ;29, T32N1 IRQHandler
                DCD     T32N2_IRQHandler          ;30, T32N2 IRQHandler
                DCD     0                         ;31, Reserved
                DCD     WDT_IRQHandler            ;32, WDT IRQHandler
                DCD     RTC_IRQHandler            ;33, RTC IRQHandler
                DCD     0                         ;34, Reserved
                DCD     ADC_IRQHandler            ;35, ADC IRQHandler
                DCD     LCD_IRQHandler            ;36, LCD IRQHandler
                DCD     LVD_IRQHandler            ;37, LVD IRQHandler
                DCD     RTCHW_IRQHandler          ;38, RTCHW IRQHandler
                DCD     UART0_IRQHandler          ;39, UART0 IRQHandler
                DCD     UART1_IRQHandler          ;40, UART1 IRQHandler
                DCD     UART2_IRQHandler          ;41, UART2 IRQHandler
                DCD     UART3_IRQHandler          ;42, UART3 IRQHandler
                DCD     EUART0_IRQHandler         ;43, EUART0 IRQHandler 
                DCD     EUART1_IRQHandler         ;44, EUART1 IRQHandler
                DCD     SPI_IRQHandler            ;45  SPI IRQHandler
                DCD     IIC_IRQHandler            ;46, IIC IRQHandler
                DCD     0                         ;47, Reserved


                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_IRQHandler   PROC
                EXPORT  Reset_IRQHandler             [WEAK]
                IMPORT  __main
                IMPORT  SystemInit 					
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP

; Dummy Exception IRQHandlers (infinite loops which can be modified)                

NMI_IRQHandler     PROC
                EXPORT  NMI_IRQHandler               [WEAK]
                B       .
                ENDP
HardFault_IRQHandler	PROC
                EXPORT  HardFault_IRQHandler         [WEAK]
                B       .
                ENDP
SVC_IRQHandler     PROC
                EXPORT  SVC_IRQHandler               [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler            [WEAK]
                B       .
                ENDP
SysTick_IRQHandler PROC
                EXPORT  SysTick_IRQHandler           [WEAK]
                B       .
                ENDP

Default_IRQHandler PROC



                EXPORT  PINT0_IRQHandler    [WEAK]
                EXPORT  PINT1_IRQHandler    [WEAK]
                EXPORT  PINT2_IRQHandler    [WEAK]
                EXPORT  PINT3_IRQHandler    [WEAK]
                EXPORT  PINT4_IRQHandler    [WEAK]
                EXPORT  PINT5_IRQHandler    [WEAK]
                EXPORT  PINT6_IRQHandler    [WEAK]
                EXPORT  PINT7_IRQHandler    [WEAK]
                EXPORT  T16N0_IRQHandler    [WEAK]
                EXPORT  T16N1_IRQHandler    [WEAK]
                EXPORT  T16N2_IRQHandler    [WEAK]
                EXPORT  T16N3_IRQHandler    [WEAK]
                EXPORT  T32N0_IRQHandler    [WEAK]
                EXPORT  T32N1_IRQHandler    [WEAK]
                EXPORT  T32N2_IRQHandler    [WEAK]
                EXPORT  WDT_IRQHandler      [WEAK]
                EXPORT  RTC_IRQHandler      [WEAK]
                EXPORT  ADC_IRQHandler      [WEAK]
                EXPORT  LCD_IRQHandler      [WEAK]
                EXPORT  LVD_IRQHandler      [WEAK]
                EXPORT  RTCHW_IRQHandler    [WEAK]
                EXPORT  UART0_IRQHandler    [WEAK]
                EXPORT  UART1_IRQHandler    [WEAK]
                EXPORT  UART2_IRQHandler    [WEAK]
                EXPORT  UART3_IRQHandler    [WEAK]
                EXPORT  EUART0_IRQHandler   [WEAK]
                EXPORT  EUART1_IRQHandler   [WEAK]
                EXPORT  SPI_IRQHandler      [WEAK]
                EXPORT  IIC_IRQHandler      [WEAK]




PINT0_IRQHandler
PINT1_IRQHandler
PINT2_IRQHandler
PINT3_IRQHandler
PINT4_IRQHandler
PINT5_IRQHandler
PINT6_IRQHandler
PINT7_IRQHandler
T16N0_IRQHandler
T16N1_IRQHandler
T16N2_IRQHandler
T16N3_IRQHandler
T32N0_IRQHandler
T32N1_IRQHandler
T32N2_IRQHandler
WDT_IRQHandler
RTC_IRQHandler
ADC_IRQHandler
LCD_IRQHandler
LVD_IRQHandler
RTCHW_IRQHandler
UART0_IRQHandler
UART1_IRQHandler
UART2_IRQHandler
UART3_IRQHandler
EUART0_IRQHandler
EUART1_IRQHandler
SPI_IRQHandler
IIC_IRQHandler


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
__user_initial_stackheap

                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR

                ALIGN

                ENDIF

                END

