/******************************************************************************
 * @file     startup_TLE986x.s
 * @brief    CMSIS Cortex-M3 Core Device Startup File for
 *           Infineon TLE986x Device Series
 * @version  V1.2
 * @date     20. July 2015
 *
 * Copyright (C) 2015 Infineon Technologies AG. All rights reserved.
 *
 *
 * @par
 * Infineon Technologies AG (Infineon) is supplying this software for use with 
 * Infineon's microcontrollers.  This file can be freely distributed
 * within development tools that are supporting such microcontrollers.
 *
 * @par
 * THIS SOFTWARE IS PROVIDED AS IS.  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/

/********************** Version History ***************************************
 * V1.1, 24. June 2015, Initial version
 * V1.2, 20. July 2015, Fixed stack pointer initializaton
 ******************************************************************************/

/* ===========START : MACRO DEFINITION MACRO DEFINITION ================== */
 
.macro Entry Handler
    .long \Handler
.endm

.macro Insert_ExceptionHandler Handler_Func 
    .weak \Handler_Func
    .thumb_set \Handler_Func, Default_Handler
.endm

/* =============END : MACRO DEFINITION MACRO DEFINITION ================== */

/* ================== START OF VECTOR TABLE DEFINITION ====================== */
/* Vector Table - This gets programed into VTOR register by onchip BootROM */
    .syntax unified

    .section .reset
    
	.align 2
    .globl  __Vectors
    .type   __Vectors, %object
__Vectors:
    .long   __initial_sp                /* Top of Stack                 */
    .long   Reset_Handler               /* Reset Handler                */

    Entry   NMI_Handler                 /* NMI Handler                  */
    Entry   HardFault_Handler           /* Hard Fault Handler           */
    Entry   MemManage_Handler           /* MPU Fault Handler            */
    Entry   BusFault_Handler            /* Bus Fault Handler            */
    Entry   UsageFault_Handler          /* Usage Fault Handler          */
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    .long   0                           /* Reserved                     */
    Entry   SVC_Handler                 /* SVCall Handler               */
    Entry   DebugMon_Handler            /* Debug Monitor Handler        */
    .long   0                           /* Reserved                     */
    Entry   PendSV_Handler              /* PendSV Handler               */
    Entry   SysTick_Handler             /* SysTick Handler              */

    /* Interrupt Handlers for Service Requests (SR) from Tle986x Peripherals */
    Entry   GPT1_IRQHandler             /* Handler name for GPT1   */
    Entry   GPT2_IRQHandler		        /* Handler name for GPT2   */
    Entry   ADC2_IRQHandler             /* Handler name for Measurement Unit, Timer3   */
    Entry   ADC1_IRQHandler		        /* Handler name for 10 bit ADC   */
    Entry   CCU6SR0_IRQHandler          /* Handler name for CCU0   */
    Entry   CCU6SR1_IRQHandler          /* Handler name for CCU1   */
    Entry   CCU6SR2_IRQHandler          /* Handler name for CCU2   */
    Entry   CCU6SR3_IRQHandler          /* Handler name for CCU3   */
    Entry   SSC1_IRQHandler 	        /* Handler name for SSC1   */
    Entry   SSC2_IRQHandler 	        /* Handler name for SSC2   */
    Entry   UART1_IRQHandler            /* Handler name for UART1   */
    Entry   UART2_IRQHandler	        /* Handler name for UART2   */
    Entry   EXINT0_IRQHandler           /* Handler name for EXINT0   */
    Entry   EXINT1_IRQHandler           /* Handler name for EXINT1   */
    Entry   BDRV_IRQHandler             /* Handler name for BDRV   */
    Entry   DMA_IRQHandler              /* Handler name for DMA  */


    .size  __Vectors, . - __Vectors
/* ================== END OF VECTOR TABLE DEFINITION ======================= */

/* ================== START OF VECTOR ROUTINES ============================= */

	.align	1
    .thumb

/* Reset Handler */
    .thumb_func
    .globl  Reset_Handler
    .type   Reset_Handler, %function
Reset_Handler:
    ldr r0,=__initial_sp
    msr msp, r0
    isb

#ifndef __SKIP_SYSTEM_INIT
    ldr  r0, =SystemInit
    blx  r0
#endif

/* Initialize data
 *
 *  Between symbol address __copy_table_start__ and __copy_table_end__,
 *  there are array of triplets, each of which specify:
 *    offset 0: LMA of start of a section to copy from
 *    offset 4: VMA of start of a section to copy to
 *    offset 8: size of the section to copy. Must be multiply of 4
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
	ldr	r4, =__copy_table_start__
	ldr	r5, =__copy_table_end__

.L_loop0:
	cmp	r4, r5
	bge	.L_loop0_done
	ldr	r1, [r4]
	ldr	r2, [r4, #4]
	ldr	r3, [r4, #8]

.L_loop0_0:
	subs	r3, #4
	ittt	ge
	ldrge	r0, [r1, r3]
	strge	r0, [r2, r3]
	bge	.L_loop0_0

	adds	r4, #12
	b	.L_loop0

.L_loop0_done:

/* Zero initialized data 
 *  Between symbol address __zero_table_start__ and __zero_table_end__,
 *  there are array of tuples specifying:
 *    offset 0: Start of a BSS section
 *    offset 4: Size of this BSS section. Must be multiply of 4
 *
 *  Define __SKIP_BSS_CLEAR to disable zeroing uninitialzed data in startup.
 */    
#ifndef __SKIP_BSS_CLEAR
	ldr	r3, =__zero_table_start__
	ldr	r4, =__zero_table_end__

.L_loop2:
	cmp	r3, r4
	bge	.L_loop2_done
	ldr	r1, [r3]
	ldr	r2, [r3, #4]
	movs	r0, 0

.L_loop2_0:
	subs	r2, #4
	itt	ge
	strge	r0, [r1, r2]
	bge	.L_loop2_0

	adds	r3, #8
	b	.L_loop2
.L_loop2_done:    
#endif /* __SKIP_BSS_CLEAR */
   
#ifndef __SKIP_LIBC_INIT_ARRAY
    ldr  r0, =__libc_init_array
    blx  r0
#endif

    ldr  r0, =main
    blx  r0

.align 2
__copy_table_start__:
    .long __data_load, __data_start, __data_size
    .long __ram_code_load, __ram_code_start, __ram_code_size
__copy_table_end__:

__zero_table_start__:
    .long __bss_start, __bss_size
__zero_table_end__:
    
	.pool
    .size   Reset_Handler,.-Reset_Handler
    
/* ======================================================================== */
/* ========== START OF INTERRUPT HANDLER DEFINITION ======================== */

/* Default exception Handlers - Users may override this default functionality by
   defining handlers of the same name in their C code */
    .align	1
    .thumb_func
    .weak Default_Handler
    .type Default_Handler, %function
Default_Handler:
    b .
    .size Default_Handler, . - Default_Handler

     Insert_ExceptionHandler NMI_Handler
     Insert_ExceptionHandler HardFault_Handler
     Insert_ExceptionHandler MemManage_Handler
     Insert_ExceptionHandler BusFault_Handler
     Insert_ExceptionHandler UsageFault_Handler
     Insert_ExceptionHandler SVC_Handler
     Insert_ExceptionHandler DebugMon_Handler
     Insert_ExceptionHandler PendSV_Handler
     Insert_ExceptionHandler SysTick_Handler

     Insert_ExceptionHandler GPT1_IRQHandler
     Insert_ExceptionHandler GPT2_IRQHandler
     Insert_ExceptionHandler ADC2_IRQHandler
     Insert_ExceptionHandler ADC1_IRQHandler
     Insert_ExceptionHandler CCU6SR0_IRQHandler
     Insert_ExceptionHandler CCU6SR1_IRQHandler
     Insert_ExceptionHandler CCU6SR2_IRQHandler
     Insert_ExceptionHandler CCU6SR3_IRQHandler
     Insert_ExceptionHandler SSC1_IRQHandler
     Insert_ExceptionHandler SSC2_IRQHandler
     Insert_ExceptionHandler UART1_IRQHandler
     Insert_ExceptionHandler UART2_IRQHandler
     Insert_ExceptionHandler EXINT0_IRQHandler
     Insert_ExceptionHandler EXINT1_IRQHandler
     Insert_ExceptionHandler BDRV_IRQHandler
     Insert_ExceptionHandler DMA_IRQHandler

/* ============= END OF INTERRUPT HANDLER DEFINITION ====================== */

    .end
