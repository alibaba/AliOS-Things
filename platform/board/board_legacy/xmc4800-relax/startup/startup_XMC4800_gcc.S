/*********************************************************************************************************************
 * @file     startup_XMC4800.S
 * @brief    CMSIS Core Device Startup File for Infineon XMC4800 Device Series
 * @version  V1.1
 * @date     05 Jan 2016
 *
 * @cond
 *********************************************************************************************************************
 * Copyright (c) 2015-2016, Infineon Technologies AG
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
 * V1.0,Sep, 03, 2015 JFT:Initial version
 * V1.1,Jan, 05, 2016 JFT:Fix .reset section attributes
 *
 * @endcond 
 */

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

    .section .reset, "a", %progbits
    
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

    /* Interrupt Handlers for Service Requests (SR) from XMC4800 Peripherals */
    Entry   SCU_0_IRQHandler            /* Handler name for SR SCU_0     */
    Entry   ERU0_0_IRQHandler           /* Handler name for SR ERU0_0    */
    Entry   ERU0_1_IRQHandler           /* Handler name for SR ERU0_1    */
    Entry   ERU0_2_IRQHandler           /* Handler name for SR ERU0_2    */
    Entry   ERU0_3_IRQHandler           /* Handler name for SR ERU0_3    */ 
    Entry   ERU1_0_IRQHandler           /* Handler name for SR ERU1_0    */
    Entry   ERU1_1_IRQHandler           /* Handler name for SR ERU1_1    */
    Entry   ERU1_2_IRQHandler           /* Handler name for SR ERU1_2    */
    Entry   ERU1_3_IRQHandler           /* Handler name for SR ERU1_3    */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    Entry   PMU0_0_IRQHandler           /* Handler name for SR PMU0_0    */
    .long   0                           /* Not Available                 */
    Entry   VADC0_C0_0_IRQHandler       /* Handler name for SR VADC0_C0_0  */
    Entry   VADC0_C0_1_IRQHandler       /* Handler name for SR VADC0_C0_1  */
    Entry   VADC0_C0_2_IRQHandler       /* Handler name for SR VADC0_C0_1  */
    Entry   VADC0_C0_3_IRQHandler       /* Handler name for SR VADC0_C0_3  */
    Entry   VADC0_G0_0_IRQHandler       /* Handler name for SR VADC0_G0_0  */
    Entry   VADC0_G0_1_IRQHandler       /* Handler name for SR VADC0_G0_1  */
    Entry   VADC0_G0_2_IRQHandler       /* Handler name for SR VADC0_G0_2  */
    Entry   VADC0_G0_3_IRQHandler       /* Handler name for SR VADC0_G0_3  */
    Entry   VADC0_G1_0_IRQHandler       /* Handler name for SR VADC0_G1_0  */
    Entry   VADC0_G1_1_IRQHandler       /* Handler name for SR VADC0_G1_1  */
    Entry   VADC0_G1_2_IRQHandler       /* Handler name for SR VADC0_G1_2  */
    Entry   VADC0_G1_3_IRQHandler       /* Handler name for SR VADC0_G1_3  */
    Entry   VADC0_G2_0_IRQHandler       /* Handler name for SR VADC0_G2_0  */
    Entry   VADC0_G2_1_IRQHandler       /* Handler name for SR VADC0_G2_1  */
    Entry   VADC0_G2_2_IRQHandler       /* Handler name for SR VADC0_G2_2  */
    Entry   VADC0_G2_3_IRQHandler       /* Handler name for SR VADC0_G2_3  */
    Entry   VADC0_G3_0_IRQHandler       /* Handler name for SR VADC0_G3_0  */
    Entry   VADC0_G3_1_IRQHandler       /* Handler name for SR VADC0_G3_1  */
    Entry   VADC0_G3_2_IRQHandler       /* Handler name for SR VADC0_G3_2  */
    Entry   VADC0_G3_3_IRQHandler       /* Handler name for SR VADC0_G3_3  */
    Entry   DSD0_0_IRQHandler           /* Handler name for SR DSD0_0    */
    Entry   DSD0_1_IRQHandler           /* Handler name for SR DSD0_1    */
    Entry   DSD0_2_IRQHandler           /* Handler name for SR DSD0_2    */
    Entry   DSD0_3_IRQHandler           /* Handler name for SR DSD0_3    */
    Entry   DSD0_4_IRQHandler           /* Handler name for SR DSD0_4    */
    Entry   DSD0_5_IRQHandler           /* Handler name for SR DSD0_5    */
    Entry   DSD0_6_IRQHandler           /* Handler name for SR DSD0_6    */
    Entry   DSD0_7_IRQHandler           /* Handler name for SR DSD0_7    */
    Entry   DAC0_0_IRQHandler           /* Handler name for SR DAC0_0    */
    Entry   DAC0_1_IRQHandler           /* Handler name for SR DAC0_0    */
    Entry   CCU40_0_IRQHandler          /* Handler name for SR CCU40_0   */
    Entry   CCU40_1_IRQHandler          /* Handler name for SR CCU40_1   */
    Entry   CCU40_2_IRQHandler          /* Handler name for SR CCU40_2   */
    Entry   CCU40_3_IRQHandler          /* Handler name for SR CCU40_3   */
    Entry   CCU41_0_IRQHandler          /* Handler name for SR CCU41_0   */
    Entry   CCU41_1_IRQHandler          /* Handler name for SR CCU41_1   */
    Entry   CCU41_2_IRQHandler          /* Handler name for SR CCU41_2   */
    Entry   CCU41_3_IRQHandler          /* Handler name for SR CCU41_3   */
    Entry   CCU42_0_IRQHandler          /* Handler name for SR CCU42_0   */
    Entry   CCU42_1_IRQHandler          /* Handler name for SR CCU42_1   */
    Entry   CCU42_2_IRQHandler          /* Handler name for SR CCU42_2   */
    Entry   CCU42_3_IRQHandler          /* Handler name for SR CCU42_3   */
    Entry   CCU43_0_IRQHandler          /* Handler name for SR CCU43_0   */
    Entry   CCU43_1_IRQHandler          /* Handler name for SR CCU43_1   */
    Entry   CCU43_2_IRQHandler          /* Handler name for SR CCU43_2   */
    Entry   CCU43_3_IRQHandler          /* Handler name for SR CCU43_3   */
    Entry   CCU80_0_IRQHandler          /* Handler name for SR CCU80_0   */
    Entry   CCU80_1_IRQHandler          /* Handler name for SR CCU80_1   */
    Entry   CCU80_2_IRQHandler          /* Handler name for SR CCU80_2   */
    Entry   CCU80_3_IRQHandler          /* Handler name for SR CCU80_3   */
    Entry   CCU81_0_IRQHandler          /* Handler name for SR CCU81_0   */
    Entry   CCU81_1_IRQHandler          /* Handler name for SR CCU81_1   */
    Entry   CCU81_2_IRQHandler          /* Handler name for SR CCU81_2   */
    Entry   CCU81_3_IRQHandler          /* Handler name for SR CCU81_3   */
    Entry   POSIF0_0_IRQHandler         /* Handler name for SR POSIF0_0  */
    Entry   POSIF0_1_IRQHandler         /* Handler name for SR POSIF0_1  */
    Entry   POSIF1_0_IRQHandler         /* Handler name for SR POSIF1_0  */
    Entry   POSIF1_1_IRQHandler         /* Handler name for SR POSIF1_1  */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    Entry   CAN0_0_IRQHandler           /* Handler name for SR CAN0_0    */
    Entry   CAN0_1_IRQHandler           /* Handler name for SR CAN0_1    */
    Entry   CAN0_2_IRQHandler           /* Handler name for SR CAN0_2    */
    Entry   CAN0_3_IRQHandler           /* Handler name for SR CAN0_3    */
    Entry   CAN0_4_IRQHandler           /* Handler name for SR CAN0_4    */
    Entry   CAN0_5_IRQHandler           /* Handler name for SR CAN0_5    */
    Entry   CAN0_6_IRQHandler           /* Handler name for SR CAN0_6    */
    Entry   CAN0_7_IRQHandler           /* Handler name for SR CAN0_7    */
    Entry   USIC0_0_IRQHandler          /* Handler name for SR USIC0_0   */
    Entry   USIC0_1_IRQHandler          /* Handler name for SR USIC0_1   */
    Entry   USIC0_2_IRQHandler          /* Handler name for SR USIC0_2   */
    Entry   USIC0_3_IRQHandler          /* Handler name for SR USIC0_3   */
    Entry   USIC0_4_IRQHandler          /* Handler name for SR USIC0_4   */
    Entry   USIC0_5_IRQHandler          /* Handler name for SR USIC0_5   */
    Entry   USIC1_0_IRQHandler          /* Handler name for SR USIC1_0   */
    Entry   USIC1_1_IRQHandler          /* Handler name for SR USIC1_1   */
    Entry   USIC1_2_IRQHandler          /* Handler name for SR USIC1_2   */
    Entry   USIC1_3_IRQHandler          /* Handler name for SR USIC1_3   */
    Entry   USIC1_4_IRQHandler          /* Handler name for SR USIC1_4   */
    Entry   USIC1_5_IRQHandler          /* Handler name for SR USIC1_5   */
    Entry   USIC2_0_IRQHandler          /* Handler name for SR USIC2_0   */
    Entry   USIC2_1_IRQHandler          /* Handler name for SR USIC2_1   */
    Entry   USIC2_2_IRQHandler          /* Handler name for SR USIC2_2   */
    Entry   USIC2_3_IRQHandler          /* Handler name for SR USIC2_3   */
    Entry   USIC2_4_IRQHandler          /* Handler name for SR USIC2_4   */
    Entry   USIC2_5_IRQHandler          /* Handler name for SR USIC2_5   */
    Entry   LEDTS0_0_IRQHandler         /* Handler name for SR LEDTS0_0  */
    .long   0                           /* Not Available                 */
    Entry   FCE0_0_IRQHandler           /* Handler name for SR FCE0_0    */
    Entry   GPDMA0_0_IRQHandler         /* Handler name for SR GPDMA0_0  */
    Entry   SDMMC0_0_IRQHandler         /* Handler name for SR SDMMC0_0  */
    Entry   USB0_0_IRQHandler           /* Handler name for SR USB0_0    */
    Entry   ETH0_0_IRQHandler           /* Handler name for SR ETH0_0    */
	Entry   ECAT0_0_IRQHandler          /* Handler name for SR ECAT0_0   */
    Entry   GPDMA1_0_IRQHandler         /* Handler name for SR GPDMA1_0  */
    .long   0                           /* Not Available                 */

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
    ldr sp,=__initial_sp

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
    .long USB_RAM_start, USB_RAM_size
    .long ETH_RAM_start, ETH_RAM_size
__zero_table_end__:
    
	.pool
    .size   Reset_Handler,.-Reset_Handler

/* ======================================================================== */
/* ========== START OF EXCEPTION HANDLER DEFINITION ======================== */

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

     Insert_ExceptionHandler SCU_0_IRQHandler
     Insert_ExceptionHandler ERU0_0_IRQHandler
     Insert_ExceptionHandler ERU0_1_IRQHandler
     Insert_ExceptionHandler ERU0_2_IRQHandler
     Insert_ExceptionHandler ERU0_3_IRQHandler
     Insert_ExceptionHandler ERU1_0_IRQHandler
     Insert_ExceptionHandler ERU1_1_IRQHandler
     Insert_ExceptionHandler ERU1_2_IRQHandler
     Insert_ExceptionHandler ERU1_3_IRQHandler
     Insert_ExceptionHandler PMU0_0_IRQHandler
     Insert_ExceptionHandler VADC0_C0_0_IRQHandler
     Insert_ExceptionHandler VADC0_C0_1_IRQHandler
     Insert_ExceptionHandler VADC0_C0_2_IRQHandler
     Insert_ExceptionHandler VADC0_C0_3_IRQHandler
     Insert_ExceptionHandler VADC0_G0_0_IRQHandler
     Insert_ExceptionHandler VADC0_G0_1_IRQHandler
     Insert_ExceptionHandler VADC0_G0_2_IRQHandler
     Insert_ExceptionHandler VADC0_G0_3_IRQHandler
     Insert_ExceptionHandler VADC0_G1_0_IRQHandler
     Insert_ExceptionHandler VADC0_G1_1_IRQHandler
     Insert_ExceptionHandler VADC0_G1_2_IRQHandler
     Insert_ExceptionHandler VADC0_G1_3_IRQHandler
     Insert_ExceptionHandler VADC0_G2_0_IRQHandler
     Insert_ExceptionHandler VADC0_G2_1_IRQHandler
     Insert_ExceptionHandler VADC0_G2_2_IRQHandler
     Insert_ExceptionHandler VADC0_G2_3_IRQHandler
     Insert_ExceptionHandler VADC0_G3_0_IRQHandler
     Insert_ExceptionHandler VADC0_G3_1_IRQHandler
     Insert_ExceptionHandler VADC0_G3_2_IRQHandler
     Insert_ExceptionHandler VADC0_G3_3_IRQHandler
     Insert_ExceptionHandler DSD0_0_IRQHandler
     Insert_ExceptionHandler DSD0_1_IRQHandler
     Insert_ExceptionHandler DSD0_2_IRQHandler
     Insert_ExceptionHandler DSD0_3_IRQHandler
     Insert_ExceptionHandler DSD0_4_IRQHandler
     Insert_ExceptionHandler DSD0_5_IRQHandler
     Insert_ExceptionHandler DSD0_6_IRQHandler
     Insert_ExceptionHandler DSD0_7_IRQHandler
     Insert_ExceptionHandler DAC0_0_IRQHandler
     Insert_ExceptionHandler DAC0_1_IRQHandler
     Insert_ExceptionHandler CCU40_0_IRQHandler
     Insert_ExceptionHandler CCU40_1_IRQHandler
     Insert_ExceptionHandler CCU40_2_IRQHandler
     Insert_ExceptionHandler CCU40_3_IRQHandler
     Insert_ExceptionHandler CCU41_0_IRQHandler
     Insert_ExceptionHandler CCU41_1_IRQHandler
     Insert_ExceptionHandler CCU41_2_IRQHandler
     Insert_ExceptionHandler CCU41_3_IRQHandler
     Insert_ExceptionHandler CCU42_0_IRQHandler
     Insert_ExceptionHandler CCU42_1_IRQHandler
     Insert_ExceptionHandler CCU42_2_IRQHandler
     Insert_ExceptionHandler CCU42_3_IRQHandler
     Insert_ExceptionHandler CCU43_0_IRQHandler
     Insert_ExceptionHandler CCU43_1_IRQHandler
     Insert_ExceptionHandler CCU43_2_IRQHandler
     Insert_ExceptionHandler CCU43_3_IRQHandler
     Insert_ExceptionHandler CCU80_0_IRQHandler
     Insert_ExceptionHandler CCU80_1_IRQHandler
     Insert_ExceptionHandler CCU80_2_IRQHandler
     Insert_ExceptionHandler CCU80_3_IRQHandler
     Insert_ExceptionHandler CCU81_0_IRQHandler
     Insert_ExceptionHandler CCU81_1_IRQHandler
     Insert_ExceptionHandler CCU81_2_IRQHandler
     Insert_ExceptionHandler CCU81_3_IRQHandler
     Insert_ExceptionHandler POSIF0_0_IRQHandler
     Insert_ExceptionHandler POSIF0_1_IRQHandler
     Insert_ExceptionHandler POSIF1_0_IRQHandler
     Insert_ExceptionHandler POSIF1_1_IRQHandler
     Insert_ExceptionHandler CAN0_0_IRQHandler
     Insert_ExceptionHandler CAN0_1_IRQHandler
     Insert_ExceptionHandler CAN0_2_IRQHandler
     Insert_ExceptionHandler CAN0_3_IRQHandler
     Insert_ExceptionHandler CAN0_4_IRQHandler
     Insert_ExceptionHandler CAN0_5_IRQHandler
     Insert_ExceptionHandler CAN0_6_IRQHandler
     Insert_ExceptionHandler CAN0_7_IRQHandler
     Insert_ExceptionHandler USIC0_0_IRQHandler
     Insert_ExceptionHandler USIC0_1_IRQHandler
     Insert_ExceptionHandler USIC0_2_IRQHandler
     Insert_ExceptionHandler USIC0_3_IRQHandler
     Insert_ExceptionHandler USIC0_4_IRQHandler
     Insert_ExceptionHandler USIC0_5_IRQHandler
     Insert_ExceptionHandler USIC1_0_IRQHandler
     Insert_ExceptionHandler USIC1_1_IRQHandler
     Insert_ExceptionHandler USIC1_2_IRQHandler
     Insert_ExceptionHandler USIC1_3_IRQHandler
     Insert_ExceptionHandler USIC1_4_IRQHandler
     Insert_ExceptionHandler USIC1_5_IRQHandler
     Insert_ExceptionHandler USIC2_0_IRQHandler
     Insert_ExceptionHandler USIC2_1_IRQHandler
     Insert_ExceptionHandler USIC2_2_IRQHandler
     Insert_ExceptionHandler USIC2_3_IRQHandler
     Insert_ExceptionHandler USIC2_4_IRQHandler
     Insert_ExceptionHandler USIC2_5_IRQHandler
     Insert_ExceptionHandler LEDTS0_0_IRQHandler
     Insert_ExceptionHandler FCE0_0_IRQHandler
     Insert_ExceptionHandler GPDMA0_0_IRQHandler
     Insert_ExceptionHandler SDMMC0_0_IRQHandler
     Insert_ExceptionHandler USB0_0_IRQHandler
     Insert_ExceptionHandler ETH0_0_IRQHandler
     Insert_ExceptionHandler ECAT0_0_IRQHandler	 
     Insert_ExceptionHandler GPDMA1_0_IRQHandler
     
/* ============= END OF INTERRUPT HANDLER DEFINITION ====================== */

    .end
