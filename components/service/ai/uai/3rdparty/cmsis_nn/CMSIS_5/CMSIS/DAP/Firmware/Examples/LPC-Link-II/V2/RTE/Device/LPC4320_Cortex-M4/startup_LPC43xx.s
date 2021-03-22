;/**************************************************************************//**
; * @file     LPC43xx.s
; * @brief    CMSIS Cortex-M4 Core Device Startup File for
; *           NXP LPC43xxDevice Series
; * @version  V1.00
; * @date     03. September 2013
; *
; * @note
; * Copyright (C) 2009-2013 ARM Limited. All rights reserved.
; *
; * @par
; * ARM Limited (ARM) is supplying this software for use with Cortex-M
; * processor based microcontrollers.  This file can be freely distributed
; * within development tools that are supporting such ARM based processors.
; *
; * @par
; * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
; * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
; *
; * <<< Use Configuration Wizard in Context Menu >>>   
; ******************************************************************************/

; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000200

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

Sign_Value      EQU     0x5A5A5A5A

__Vectors       DCD     __initial_sp                            ;  0  Top of Stack
                DCD     Reset_Handler                           ;  1  Reset Handler
                DCD     NMI_Handler                             ;  2  NMI Handler
                DCD     HardFault_Handler                       ;  3  Hard Fault Handler
                DCD     MemManage_Handler                       ;  4  MPU Fault Handler
                DCD     BusFault_Handler                        ;  5  Bus Fault Handler
                DCD     UsageFault_Handler                      ;  6  Usage Fault Handler
                DCD     Sign_Value                              ;  7  Reserved
                DCD     0                                       ;  8  Reserved
                DCD     0                                       ;  9  Reserved
                DCD     0                                       ; 10  Reserved
                DCD     SVC_Handler                             ; 11  SVCall Handler
                DCD     DebugMon_Handler                        ; 12  Debug Monitor Handler
                DCD     0                                       ; 13  Reserved
                DCD     PendSV_Handler                          ; 14  PendSV Handler
                DCD     SysTick_Handler                         ; 15  SysTick Handler

                ; External LPC43xx/M4 Interrupts
                DCD     DAC_IRQHandler                          ;  0  DAC interrupt
                DCD     M0APP_IRQHandler                        ;  1  Cortex-M0APP; Latched TXEV; for M4-M0APP communication
                DCD     DMA_IRQHandler                          ;  2  DMA interrupt
                DCD     0                                       ;  3  Reserved
                DCD     FLASHEEPROM_IRQHandler                  ;  4  flash bank A, flash bank B, EEPROM ORed interrupt
                DCD     ETHERNET_IRQHandler                     ;  5  Ethernet interrupt
                DCD     SDIO_IRQHandler                         ;  6  SD/MMC interrupt
                DCD     LCD_IRQHandler                          ;  7  LCD interrupt
                DCD     USB0_IRQHandler                         ;  8  OTG interrupt
                DCD     USB1_IRQHandler                         ;  9  USB1 interrupt
                DCD     SCT_IRQHandler                          ; 10  SCT combined interrupt
                DCD     RITIMER_IRQHandler                      ; 11  RI Timer interrupt
                DCD     TIMER0_IRQHandler                       ; 12  Timer 0 interrupt
                DCD     TIMER1_IRQHandler                       ; 13  Timer 1 interrupt
                DCD     TIMER2_IRQHandler                       ; 14  Timer 2 interrupt
                DCD     TIMER3_IRQHandler                       ; 15  Timer 3 interrupt
                DCD     MCPWM_IRQHandler                        ; 16  Motor control PWM interrupt
                DCD     ADC0_IRQHandler                         ; 17  ADC0 interrupt
                DCD     I2C0_IRQHandler                         ; 18  I2C0 interrupt
                DCD     I2C1_IRQHandler                         ; 19  I2C1 interrupt
                DCD     SPI_IRQHandler                          ; 20  SPI interrupt
                DCD     ADC1_IRQHandler                         ; 21  ADC1 interrupt
                DCD     SSP0_IRQHandler                         ; 22  SSP0 interrupt
                DCD     SSP1_IRQHandler                         ; 23  SSP1 interrupt
                DCD     USART0_IRQHandler                       ; 24  USART0 interrupt
                DCD     UART1_IRQHandler                        ; 25  Combined UART1, Modem interrupt
                DCD     USART2_IRQHandler                       ; 26  USART2 interrupt
                DCD     USART3_IRQHandler                       ; 27  Combined USART3, IrDA interrupt
                DCD     I2S0_IRQHandler                         ; 28  I2S0 interrupt
                DCD     I2S1_IRQHandler                         ; 29  I2S1 interrupt
                DCD     SPIFI_IRQHandler                        ; 30  SPISI interrupt
                DCD     SGPIO_IRQHandler                        ; 31  SGPIO interrupt
                DCD     PIN_INT0_IRQHandler                     ; 32  GPIO pin interrupt 0
                DCD     PIN_INT1_IRQHandler                     ; 33  GPIO pin interrupt 1
                DCD     PIN_INT2_IRQHandler                     ; 34  GPIO pin interrupt 2
                DCD     PIN_INT3_IRQHandler                     ; 35  GPIO pin interrupt 3
                DCD     PIN_INT4_IRQHandler                     ; 36  GPIO pin interrupt 4
                DCD     PIN_INT5_IRQHandler                     ; 37  GPIO pin interrupt 5
                DCD     PIN_INT6_IRQHandler                     ; 38  GPIO pin interrupt 6
                DCD     PIN_INT7_IRQHandler                     ; 39  GPIO pin interrupt 7
                DCD     GINT0_IRQHandler                        ; 40  GPIO global interrupt 0
                DCD     GINT1_IRQHandler                        ; 41  GPIO global interrupt 1
                DCD     EVENTROUTER_IRQHandler                  ; 42  Event router interrupt
                DCD     C_CAN1_IRQHandler                       ; 43  C_CAN1 interrupt
                DCD     0                                       ; 44  Reserved
                DCD     ADCHS_IRQHandler                        ; 45  ADCHS combined interrupt
                DCD     ATIMER_IRQHandler                       ; 46  Alarm timer interrupt
                DCD     RTC_IRQHandler                          ; 47  RTC interrupt
                DCD     0                                       ; 48  Reserved
                DCD     WWDT_IRQHandler                         ; 49  WWDT interrupt
                DCD     M0SUB_IRQHandler                        ; 50  TXEV instruction from the M0 subsystem core interrupt
                DCD     C_CAN0_IRQHandler                       ; 51  C_CAN0 interrupt
                DCD     QEI_IRQHandler                          ; 52  QEI interrupt


;CRP address at offset 0x2FC relative to the BOOT Bank address
                IF      :LNOT::DEF:NO_CRP
                SPACE   (0x2FC - (. - __Vectors))
;                EXPORT  CRP_Key
CRP_Key         DCD     0xFFFFFFFF
;                       0xFFFFFFFF => CRP Disabled
;                       0x12345678 => CRP Level 1
;                       0x87654321 => CRP Level 2
;                       0x43218765 => CRP Level 3 (ARE YOU SURE?)
;                       0x4E697370 => NO ISP      (ARE YOU SURE?)
                ENDIF

                AREA    |.text|, CODE, READONLY

; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler           [WEAK]
                IMPORT  SystemInit
                IMPORT  __main
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP

; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler     PROC
                EXPORT  NMI_Handler                         [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler                   [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler                   [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler                    [WEAK]
                B       .
                ENDP
UsageFault_Handler\
                PROC
                EXPORT  UsageFault_Handler                  [WEAK]
                B       .
                ENDP
SVC_Handler     PROC
                EXPORT  SVC_Handler                         [WEAK]
                B       .
                ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler                    [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
                EXPORT  PendSV_Handler                      [WEAK]
                B       .
                ENDP
SysTick_Handler PROC
                EXPORT  SysTick_Handler                     [WEAK]
                B       .
                ENDP

Default_Handler PROC
                EXPORT  DAC_IRQHandler                      [WEAK]
                EXPORT  M0APP_IRQHandler                    [WEAK]
                EXPORT  DMA_IRQHandler                      [WEAK]
                EXPORT  FLASHEEPROM_IRQHandler              [WEAK]
                EXPORT  ETHERNET_IRQHandler                 [WEAK]
                EXPORT  SDIO_IRQHandler                     [WEAK]
                EXPORT  LCD_IRQHandler                      [WEAK]
                EXPORT  USB0_IRQHandler                     [WEAK]
                EXPORT  USB1_IRQHandler                     [WEAK]
                EXPORT  SCT_IRQHandler                      [WEAK]
                EXPORT  RITIMER_IRQHandler                  [WEAK]
                EXPORT  TIMER0_IRQHandler                   [WEAK]
                EXPORT  TIMER1_IRQHandler                   [WEAK]
                EXPORT  TIMER2_IRQHandler                   [WEAK]
                EXPORT  TIMER3_IRQHandler                   [WEAK]
                EXPORT  MCPWM_IRQHandler                    [WEAK]
                EXPORT  ADC0_IRQHandler                     [WEAK]
                EXPORT  I2C0_IRQHandler                     [WEAK]
                EXPORT  I2C1_IRQHandler                     [WEAK]
                EXPORT  SPI_IRQHandler                      [WEAK]
                EXPORT  ADC1_IRQHandler                     [WEAK]
                EXPORT  SSP0_IRQHandler                     [WEAK]
                EXPORT  SSP1_IRQHandler                     [WEAK]
                EXPORT  USART0_IRQHandler                   [WEAK]
                EXPORT  UART1_IRQHandler                    [WEAK]
                EXPORT  USART2_IRQHandler                   [WEAK]
                EXPORT  USART3_IRQHandler                   [WEAK]
                EXPORT  I2S0_IRQHandler                     [WEAK]
                EXPORT  I2S1_IRQHandler                     [WEAK]
                EXPORT  SPIFI_IRQHandler                    [WEAK]
                EXPORT  SGPIO_IRQHandler                    [WEAK]
                EXPORT  PIN_INT0_IRQHandler                 [WEAK]
                EXPORT  PIN_INT1_IRQHandler                 [WEAK]
                EXPORT  PIN_INT2_IRQHandler                 [WEAK]
                EXPORT  PIN_INT3_IRQHandler                 [WEAK]
                EXPORT  PIN_INT4_IRQHandler                 [WEAK]
                EXPORT  PIN_INT5_IRQHandler                 [WEAK]
                EXPORT  PIN_INT6_IRQHandler                 [WEAK]
                EXPORT  PIN_INT7_IRQHandler                 [WEAK]
                EXPORT  GINT0_IRQHandler                    [WEAK]
                EXPORT  GINT1_IRQHandler                    [WEAK]
                EXPORT  EVENTROUTER_IRQHandler              [WEAK]
                EXPORT  C_CAN1_IRQHandler                   [WEAK]
                EXPORT  ADCHS_IRQHandler                    [WEAK]
                EXPORT  ATIMER_IRQHandler                   [WEAK]
                EXPORT  RTC_IRQHandler                      [WEAK]
                EXPORT  WWDT_IRQHandler                     [WEAK]
                EXPORT  M0SUB_IRQHandler                    [WEAK]
                EXPORT  C_CAN0_IRQHandler                   [WEAK]
                EXPORT  QEI_IRQHandler                      [WEAK]

DAC_IRQHandler
M0APP_IRQHandler
DMA_IRQHandler
FLASHEEPROM_IRQHandler
ETHERNET_IRQHandler
SDIO_IRQHandler
LCD_IRQHandler
USB0_IRQHandler
USB1_IRQHandler
SCT_IRQHandler
RITIMER_IRQHandler
TIMER0_IRQHandler
TIMER1_IRQHandler
TIMER2_IRQHandler
TIMER3_IRQHandler
MCPWM_IRQHandler
ADC0_IRQHandler
I2C0_IRQHandler
I2C1_IRQHandler
SPI_IRQHandler
ADC1_IRQHandler
SSP0_IRQHandler
SSP1_IRQHandler
USART0_IRQHandler
UART1_IRQHandler
USART2_IRQHandler
USART3_IRQHandler
I2S0_IRQHandler
I2S1_IRQHandler
SPIFI_IRQHandler
SGPIO_IRQHandler
PIN_INT0_IRQHandler
PIN_INT1_IRQHandler
PIN_INT2_IRQHandler
PIN_INT3_IRQHandler
PIN_INT4_IRQHandler
PIN_INT5_IRQHandler
PIN_INT6_IRQHandler
PIN_INT7_IRQHandler
GINT0_IRQHandler
GINT1_IRQHandler
EVENTROUTER_IRQHandler
C_CAN1_IRQHandler
ADCHS_IRQHandler
ATIMER_IRQHandler
RTC_IRQHandler
WWDT_IRQHandler
M0SUB_IRQHandler
C_CAN0_IRQHandler
QEI_IRQHandler

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
