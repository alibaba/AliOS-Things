/*********************************************************************************************************************
 * @file     startup_XMC4100.S
 * @brief    CMSIS Core Device Startup File for Infineon XMC4100 Device Series
 * @version  V1.0
 * @date     01 June 2016
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
 * V0.1,Sep, 13, 2012 ES : initial version
 * V0.2,Oct, 12, 2012 PKB: C++ support
 * V0.3,Jan, 26, 2013 PKB: Workaround for prefetch bug
 * V0.4,Jul, 29, 2013 PKB: AAPCS violation in V0.3 fixed
 * V0.5,Feb, 05, 2014 PKB: Removed redundant alignment code from copy+clear funcs   
 * V0.6,May, 05, 2014 JFT: Added ram_code section
 * V0.7,Nov, 25, 2014 JFT: CPU workaround disabled. Single default handler.
 *                         Removed DAVE3 dependency
 * V0.8,Jan, 05, 2016 JFT: Fix .reset section attributes
 * V0.9,March,04,2016 JFT: Fix weak definition of Veneers. 
 *                         Only relevant for AA, which needs ENABLE_PMU_CM_001_WORKAROUND 
 * V1.0,June ,01,2016 JFT: Rename ENABLE_CPU_CM_001_WORKAROUND to ENABLE_PMU_CM_001_WORKAROUND
 *                         Action required: If using AA step, use ENABLE_PMU_CM_001_WORKAROUND instead of ENABLE_CPU_CM_001_WORKAROUND
 * @endcond 
 */

/* ===========START : MACRO DEFINITION MACRO DEFINITION ================== */
 
.macro Entry Handler
#if defined(ENABLE_PMU_CM_001_WORKAROUND)
    .long \Handler\()_Veneer
#else
    .long \Handler
#endif
.endm

.macro Insert_ExceptionHandler Handler_Func 
    .weak \Handler_Func
    .thumb_set \Handler_Func, Default_Handler

#if defined(ENABLE_PMU_CM_001_WORKAROUND)  
    .weak \Handler_Func\()_Veneer
    .type \Handler_Func\()_Veneer, %function
\Handler_Func\()_Veneer:
    push {r0, lr}
    ldr  r0, =\Handler_Func
    blx  r0
    pop  {r0, pc}
    .size \Handler_Func\()_Veneer, . - \Handler_Func\()_Veneer
#endif 
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

    /* Interrupt Handlers for Service Requests (SR) from XMC4100 Peripherals */
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
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    Entry   DAC0_0_IRQHandler           /* Handler name for SR DAC0_0    */
    Entry   DAC0_1_IRQHandler           /* Handler name for SR DAC0_1    */
    Entry   CCU40_0_IRQHandler          /* Handler name for SR CCU40_0   */
    Entry   CCU40_1_IRQHandler          /* Handler name for SR CCU40_1   */
    Entry   CCU40_2_IRQHandler          /* Handler name for SR CCU40_2   */
    Entry   CCU40_3_IRQHandler          /* Handler name for SR CCU40_3   */
    Entry   CCU41_0_IRQHandler          /* Handler name for SR CCU41_0   */
    Entry   CCU41_1_IRQHandler          /* Handler name for SR CCU41_1   */
    Entry   CCU41_2_IRQHandler          /* Handler name for SR CCU41_2   */
    Entry   CCU41_3_IRQHandler          /* Handler name for SR CCU41_3   */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    Entry   CCU80_0_IRQHandler          /* Handler name for SR CCU80_0   */
    Entry   CCU80_1_IRQHandler          /* Handler name for SR CCU80_1   */
    Entry   CCU80_2_IRQHandler          /* Handler name for SR CCU80_2   */
    Entry   CCU80_3_IRQHandler          /* Handler name for SR CCU80_3   */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    Entry   POSIF0_0_IRQHandler         /* Handler name for SR POSIF0_0  */
    Entry   POSIF0_1_IRQHandler         /* Handler name for SR POSIF0_1  */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    Entry   HRPWM_0_IRQHandler          /* Handler name for SR HRPWM_0   */
    Entry   HRPWM_1_IRQHandler          /* Handler name for SR HRPWM_1   */
    Entry   HRPWM_2_IRQHandler          /* Handler name for SR HRPWM_2   */
    Entry   HRPWM_3_IRQHandler          /* Handler name for SR HRPWM_3   */
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
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    Entry   LEDTS0_0_IRQHandler         /* Handler name for SR LEDTS0_0  */
    .long   0                           /* Not Available                 */
    Entry   FCE0_0_IRQHandler           /* Handler name for SR FCE0_0    */
    Entry   GPDMA0_0_IRQHandler         /* Handler name for SR GPDMA0_0  */
    .long   0                           /* Not Available                 */
    Entry   USB0_0_IRQHandler           /* Handler name for SR USB0_0    */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
    .long   0                           /* Not Available                 */
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
    Insert_ExceptionHandler CCU80_0_IRQHandler
    Insert_ExceptionHandler CCU80_1_IRQHandler
    Insert_ExceptionHandler CCU80_2_IRQHandler
    Insert_ExceptionHandler CCU80_3_IRQHandler
    Insert_ExceptionHandler POSIF0_0_IRQHandler
    Insert_ExceptionHandler POSIF0_1_IRQHandler
    Insert_ExceptionHandler HRPWM_0_IRQHandler
    Insert_ExceptionHandler HRPWM_1_IRQHandler
    Insert_ExceptionHandler HRPWM_2_IRQHandler
    Insert_ExceptionHandler HRPWM_3_IRQHandler
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
    Insert_ExceptionHandler LEDTS0_0_IRQHandler
    Insert_ExceptionHandler FCE0_0_IRQHandler
    Insert_ExceptionHandler GPDMA0_0_IRQHandler
    Insert_ExceptionHandler USB0_0_IRQHandler
     
/* ============= END OF INTERRUPT HANDLER DEFINITION ====================== */

    .end
