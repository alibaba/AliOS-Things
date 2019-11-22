;/*****************************************************************************
; * @file:    startup_P130.s
; * @purpose: CMSIS Cortex-M3 Core Device Startup File
; *           for the Eastsoft Device Series 
; * @version: V1.00
; * @date:    07. October 2016
; *------- <<< Use Configuration Wizard in Context Menu >>> ------------------
; *
; * Copyright (C) 2016 ARM Limited. All rights reserved.
; * ARM Limited (ARM) is supplying this software for use with Cortex-M3
; * processor based microcontrollers.  This file can be freely distributed
; * within development tools that are supporting such ARM based processors.
; *
; * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
; * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
; *
; *****************************************************************************/


; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00002000

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

__Vectors    DCD    __initial_sp              ; Top of Stack
        DCD    Reset_Handler             ; Reset Handler
        DCD    NMI_Handler               ; NMI Handler
        DCD    HardFault_Handler         ; Hard Fault Handler
        DCD    MemManage_Handler         ; MPU Fault Handler
        DCD    BusFault_Handler          ; Bus Fault Handler
        DCD    UsageFault_Handler        ; Usage Fault Handler
        DCD    0                         ; Reserved
        DCD    0                         ; Reserved
        DCD    0                         ; Reserved
        DCD    0                         ; Reserved
        DCD    SVC_Handler               ; SVCall Handler
        DCD    DebugMon_Handler          ; Debug Monitor Handler
        DCD    0                         ; Reserved
        DCD    PendSV_Handler            ; PendSV Handler
        DCD    SysTick_Handler           ; SysTick Handler

        ; External Interrupts
        DCD    GPIO0_IRQHandler    ; 16: 
        DCD    GPIO1_IRQHandler    ; 17: 
        DCD    GPIO2_IRQHandler    ; 18: 
        DCD    GPIO3_IRQHandler    ; 19: 
        DCD    GPIO4_IRQHandler    ; 20: 
        DCD    GPIO5_IRQHandler    ; 21: 
        DCD    GPIO6_IRQHandler    ; 22: 
        DCD    GPIO7_IRQHandler    ; 23: 
        DCD    T16N0_IRQHandler    ; 24: 
        DCD    T16N1_IRQHandler    ; 25: 
        DCD    T16N2_IRQHandler    ; 26: 
        DCD    0                   ; 27: 
        DCD    T32N0_IRQHandler    ; 28: 
        DCD    T32N1_IRQHandler    ; 29:
        DCD    T32N2_IRQHandler    ; 30: 
        DCD    T32N3_IRQHandler    ; 31: 
        DCD    IWDT_IRQHandler     ; 32: 
        DCD    WWDT_IRQHandler     ; 33: 
        DCD    SPI2_IRQHandler     ; 34: 
        DCD    SPI3_IRQHandler     ; 35: 
        DCD    UART0_IRQHandler    ; 36: 
        DCD    UART1_IRQHandler    ; 37: 
        DCD    UART2_IRQHandler    ; 38: 
        DCD    UART3_IRQHandler    ; 39: 
        DCD    SPI0_IRQHandler     ; 40: 
        DCD    SPI1_IRQHandler     ; 41: 
        DCD    I2C0_IRQHandler     ; 42: 
        DCD    I2C1_IRQHandler     ; 43: 
        DCD    AES_IRQHandler      ; 44: 
        DCD    ADC_IRQHandler      ; 45: 
        DCD    Tsensor_IRQHandler  ; 46: 
        DCD    KINT_IRQHandler     ; 47: 
        DCD    PLCRX_IRQHandler    ; 48: 
        DCD    PLCTX_IRQHandler    ; 49: 
        DCD    PLCTIMERINT_IRQHandler         ; 50: 
        DCD    0                   ; 51: 
        DCD    DMACDONE_IRQHandler ; 52: 
        DCD    DMACRR_IRQHandler   ; 53: 
        DCD    ECC_IRQHandler      ; 54: 
        DCD    RNG_IRQHandler      ; 55: 
        DCD    Cache_IRQHandler    ; 56: 
        DCD     USB_IRQHandler      ; 57
        DCD     USB_BWERR_IRQHandler; 58
        DCD     ETHERNETINT_IRQHandler         ; 59
        DCD     LVD_IRQHandler      ; 60
        DCD     COMP_IRQHandler     ; 61
        

;                AREA    |.ARM.__at_0x00202000|, CODE, READONLY
;PID_BID         DCD     0x002AFFC5

;                AREA    |.ARM.__at_0x00204000|, CODE, READONLY
;XXXXXXX         DCD     0x00000001

        AREA    |.text|, CODE, READONLY
        
        
; Reset Handler

Reset_Handler  PROC
        EXPORT  Reset_Handler             [WEAK]

        IMPORT  __main
        LDR     R0, =__main
        BX      R0
        ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

NMI_Handler    PROC
        EXPORT  NMI_Handler               [WEAK]
        B       .
        ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler         [WEAK]
                B       .
                ENDP
MemManage_Handler\
                PROC
                EXPORT  MemManage_Handler         [WEAK]
                B       .
                ENDP
BusFault_Handler\
                PROC
                EXPORT  BusFault_Handler          [WEAK]
                B       .
                ENDP
UsageFault_Handler\
        PROC
        EXPORT  UsageFault_Handler        [WEAK]
        B       .
        ENDP
SVC_Handler    PROC
        EXPORT  SVC_Handler               [WEAK]
        B       .
        ENDP
DebugMon_Handler\
                PROC
                EXPORT  DebugMon_Handler          [WEAK]
                B       .
                ENDP
PendSV_Handler  PROC
        EXPORT  PendSV_Handler            [WEAK]
        B       .
        ENDP
SysTick_Handler  PROC
        EXPORT  SysTick_Handler           [WEAK]
        B       .
        ENDP

Default_Handler PROC
        
        EXPORT  GPIO0_IRQHandler       [WEAK]
        EXPORT  GPIO1_IRQHandler       [WEAK]
        EXPORT  GPIO2_IRQHandler       [WEAK]
        EXPORT  GPIO3_IRQHandler       [WEAK]
        EXPORT  GPIO4_IRQHandler       [WEAK]
        EXPORT  GPIO5_IRQHandler       [WEAK]
        EXPORT  GPIO6_IRQHandler       [WEAK]
        EXPORT  GPIO7_IRQHandler       [WEAK]
        EXPORT  T16N0_IRQHandler       [WEAK]
        EXPORT  T16N1_IRQHandler       [WEAK]
        EXPORT  T16N2_IRQHandler       [WEAK]
        EXPORT  T32N0_IRQHandler       [WEAK]
        EXPORT  T32N1_IRQHandler       [WEAK]
        EXPORT  T32N2_IRQHandler       [WEAK]
        EXPORT  T32N3_IRQHandler       [WEAK]
        EXPORT  IWDT_IRQHandler        [WEAK]
        EXPORT  WWDT_IRQHandler        [WEAK]
        EXPORT  UART0_IRQHandler       [WEAK]
        EXPORT  UART1_IRQHandler       [WEAK]
        EXPORT  UART2_IRQHandler       [WEAK]
        EXPORT  UART3_IRQHandler       [WEAK]
        EXPORT  SPI0_IRQHandler        [WEAK]
        EXPORT  SPI1_IRQHandler        [WEAK]
        EXPORT  I2C0_IRQHandler        [WEAK]
        EXPORT  I2C1_IRQHandler        [WEAK]
        EXPORT  AES_IRQHandler         [WEAK]
        EXPORT  ADC_IRQHandler         [WEAK]
        EXPORT  Tsensor_IRQHandler     [WEAK]
        EXPORT  KINT_IRQHandler        [WEAK]
        EXPORT  PLCRX_IRQHandler       [WEAK]
        EXPORT  PLCTX_IRQHandler       [WEAK]
        EXPORT  DMACDONE_IRQHandler    [WEAK]
        EXPORT  DMACRR_IRQHandler      [WEAK]
        EXPORT  ECC_IRQHandler         [WEAK]
        EXPORT  RNG_IRQHandler         [WEAK]  
        EXPORT  Cache_IRQHandler       [WEAK]
        EXPORT  USB_IRQHandler         [WEAK]
        EXPORT  USB_BWERR_IRQHandler   [WEAK]
        EXPORT  LVD_IRQHandler         [WEAK]
        EXPORT  COMP_IRQHandler        [WEAK]
        EXPORT  SPI2_IRQHandler        [WEAK]
        EXPORT  SPI3_IRQHandler        [WEAK]
                EXPORT  PLCTIMERINT_IRQHandler            [WEAK]
                EXPORT  ETHERNETINT_IRQHandler            [WEAK]

GPIO0_IRQHandler    
GPIO1_IRQHandler    
GPIO2_IRQHandler    
GPIO3_IRQHandler    
GPIO4_IRQHandler    
GPIO5_IRQHandler    
GPIO6_IRQHandler    
GPIO7_IRQHandler    
T16N0_IRQHandler    
T16N1_IRQHandler    
T16N2_IRQHandler                 
T32N0_IRQHandler    
T32N1_IRQHandler             
T32N2_IRQHandler    
T32N3_IRQHandler    
IWDT_IRQHandler     
WWDT_IRQHandler                      
UART0_IRQHandler    
UART1_IRQHandler    
UART2_IRQHandler    
UART3_IRQHandler    
SPI0_IRQHandler     
SPI1_IRQHandler     
I2C0_IRQHandler     
I2C1_IRQHandler     
AES_IRQHandler      
ADC_IRQHandler      
Tsensor_IRQHandler  
KINT_IRQHandler     
PLCRX_IRQHandler    
PLCTX_IRQHandler                      
DMACDONE_IRQHandler 
DMACRR_IRQHandler   
ECC_IRQHandler   
RNG_IRQHandler
Cache_IRQHandler    
USB_IRQHandler                
USB_BWERR_IRQHandler                
LVD_IRQHandler            
COMP_IRQHandler  
SPI2_IRQHandler
SPI3_IRQHandler
PLCTIMERINT_IRQHandler
ETHERNETINT_IRQHandler

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
