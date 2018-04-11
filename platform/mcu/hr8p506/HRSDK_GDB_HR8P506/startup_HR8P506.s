;*******************************************************************************
; *file       : startup_HR8P506.s
; *description: HR8P506 Device Startup File
; *author     : Eastsoft MCU Software Team
; *version    : V0.01
; *data       : 1/28/2016
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

Heap_Size       EQU     0x00000200

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
                DCD     0                         ;29, Reserved
                DCD     0                         ;30, Reserved
                DCD     0                         ;31, Reserved
                DCD     WDT_IRQHandler            ;32, WDT IRQHandler
                DCD     RTC_IRQHandler            ;33, RTC IRQHandler
                DCD     KINT_IRQHandler           ;34, KINT IRQHandler
                DCD     ADC_IRQHandler            ;35, ADC IRQHandler
                DCD     0                         ;36, Reserved
                DCD     LVD_IRQHandler            ;37, LVD IRQHandler
                DCD     PLLLK_IRQHandler          ;38, PLLLK IRQHandler
                DCD     UART0_IRQHandler          ;39, UART0 IRQHandler
                DCD     UART1_IRQHandler          ;40, UART1 IRQHandler
                DCD     EUART0_IRQHandler         ;41, EUART0 IRQHandler
                DCD     0                         ;42, Reserved
                DCD     SPI0_IRQHandler           ;43, SPI0 IRQHandler
                DCD     SPI1_IRQHandler           ;44, SPI1 IRQHandler
                DCD     I2C0_IRQHandler           ;45, I2C0 IRQHandler
                DCD     0                         ;46, Reserved
                DCD     CCM_IRQHandler            ;47, CCM IRQHandler

                AREA    |.text|, CODE, READONLY
					
; Reset Handler

Reset_IRQHandler   PROC
                EXPORT  Reset_IRQHandler             [WEAK]
                IMPORT  __main
                IMPORT  SystemInit
               LDR     R0, =RegInit
               BLX     R0									
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
                EXPORT  WDT_IRQHandler      [WEAK]
                EXPORT  RTC_IRQHandler      [WEAK]
                EXPORT  KINT_IRQHandler     [WEAK]
                EXPORT  ADC_IRQHandler      [WEAK]
                EXPORT  LVD_IRQHandler      [WEAK]
                EXPORT  PLLLK_IRQHandler    [WEAK]
                EXPORT  UART0_IRQHandler    [WEAK]
                EXPORT  UART1_IRQHandler    [WEAK]
                EXPORT  EUART0_IRQHandler   [WEAK]
                EXPORT  SPI0_IRQHandler     [WEAK]
                EXPORT  SPI1_IRQHandler     [WEAK]
                EXPORT  I2C0_IRQHandler     [WEAK]
                EXPORT  CCM_IRQHandler      [WEAK]

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
WDT_IRQHandler
RTC_IRQHandler
KINT_IRQHandler
ADC_IRQHandler
LVD_IRQHandler
PLLLK_IRQHandler
UART0_IRQHandler
UART1_IRQHandler
EUART0_IRQHandler
SPI0_IRQHandler
SPI1_IRQHandler
I2C0_IRQHandler
CCM_IRQHandler

                B       .
				
                ENDP
	
	
;calibration value initial

				AREA    |.text|, CODE, READONLY					
				EXPORT  RegInit
		
RegInit   		PROC
				PUSH	{LR}
				NOP
				NOP
				NOP
				NOP
				NOP
				
				LDR     R0, =0x40000080  
				LDR     R1, =0xAA559669
				STR     R1, [R0]

				LDR     R0, =0x00100124   
				LDR     R7, =0x400000A4   
				BL      IP_CAL_LOAD

				LDR     R0, =0x00100128   
				LDR     R7, =0x400000A8  
				BL      IP_CAL_LOAD

				LDR     R0, =0x0010012C   
				LDR     R7, =0x4000009C   
				BL      IP_CAL_LOAD

				LDR     R0, =0x00100130  
				LDR     R7, =0x400000A0  
				BL      IP_CAL_LOAD

				LDR     R0, =0x40000080  
				LDR     R1, =0x00000000
				STR     R1, [R0]

				POP     {PC}


IP_CAL_LOAD
				PUSH    {LR}
				LDR     R2, =0xFFFFFFFF
				LDR     R1, [R0]
				CMP     R1, R2
				BNE     IP_CAL_VERIFY    
				LDR     R1, [R0]         
				CMP     R1, R2
				BNE     IP_CAL_VERIFY    

				B       IP_CAL_EMPTY     

IP_CAL_VERIFY
				LDR      R5, =0x00000003    
				LDR      R4, =0x000000FF
IP_CAL_VERIFY_LOOP
				MOVS     R2, R1
				ANDS     R2, R4         

				LSRS     R3, R1, #8
				ANDS     R3, R4
				ADDS     R2, R3        

				LSRS     R3, R1, #16
				ANDS     R3, R4
				ADDS     R2, R3        
				ANDS     R2, R4

				LSRS     R3, R1, #24   

				CMP      R2, R3        
				BEQ      IP_LOAD_OK
				SUBS     R5, #1
				BEQ      IP_CAL_EMPTY    
				LDR      R1, [R0]         
				B        IP_CAL_VERIFY_LOOP

IP_LOAD_OK
				STR      R1, [R7]

IP_CAL_EMPTY
				POP      {PC}

				ALIGN
				LTORG
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
