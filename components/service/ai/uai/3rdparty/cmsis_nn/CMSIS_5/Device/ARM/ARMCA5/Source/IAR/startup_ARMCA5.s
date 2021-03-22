/******************************************************************************
 * @file     startup_ARMCA9.s
 * @brief    CMSIS Device System Source File for ARM Cortex-A5 Device Series
 * @version  V1.00
 * @date     01 Nov 2017
 *
 * @note
 *
 ******************************************************************************/
/*
 * Copyright (c) 2009-2017 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

  MODULE  ?startup_ARMCA5

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
  PUBLIC  Reset_Handler
  PUBWEAK Undef_Handler
  PUBWEAK SVC_Handler
  PUBWEAK PAbt_Handler
  PUBWEAK DAbt_Handler
  PUBWEAK IRQ_Handler
  PUBWEAK FIQ_Handler

  SECTION SVC_STACK:DATA:NOROOT(3)
  SECTION IRQ_STACK:DATA:NOROOT(3)
  SECTION FIQ_STACK:DATA:NOROOT(3)
  SECTION ABT_STACK:DATA:NOROOT(3)
  SECTION UND_STACK:DATA:NOROOT(3)
  SECTION USR_STACK:DATA:NOROOT(3)

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector Table
 *----------------------------------------------------------------------------*/

  section RESET:CODE:NOROOT(2)
  PUBLIC  Vectors

Vectors:  
  LDR    PC, =Reset_Handler
  LDR    PC, =Undef_Handler
  LDR    PC, =SVC_Handler
  LDR    PC, =PAbt_Handler
  LDR    PC, =DAbt_Handler
  NOP
  LDR    PC, =IRQ_Handler
  LDR    PC, =FIQ_Handler


  section .text:CODE:NOROOT(4)

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
  *----------------------------------------------------------------------------*/
  EXTERN  SystemInit
  EXTERN  __iar_program_start

Reset_Handler:  

  // Mask interrupts
  CPSID   if

  // Put any cores other than 0 to sleep
  MRC     p15, 0, R0, c0, c0, 5
  ANDS    R0, R0, #3
goToSleep:
  WFINE
  BNE     goToSleep

  // Reset SCTLR Settings
  MRC     p15, 0, R0, c1, c0, 0 // Read CP15 System Control register
  BIC     R0, R0, #(0x1 << 12) // Clear I bit 12 to disable I Cache
  BIC     R0, R0, #(0x1 <<  2) // Clear C bit  2 to disable D Cache
  BIC     R0, R0, #0x1       // Clear M bit  0 to disable MMU
  BIC     R0, R0, #(0x1 << 11) // Clear Z bit 11 to disable branch prediction
  BIC     R0, R0, #(0x1 << 13) // Clear V bit 13 to disable hivecs
  MCR     p15, 0, R0, c1, c0, 0 // Write value back to CP15 System Control register
  ISB

  // Configure ACTLR
  MRC     p15, 0, r0, c1, c0, 1 // Read CP15 Auxiliary Control Register
  ORR     r0, r0, #(1 <<  1) // Enable L2 prefetch hint (UNK/WI since r4p1)
  MCR     p15, 0, r0, c1, c0, 1 // Write CP15 Auxiliary Control Register

  // Set Vector Base Address Register (VBAR) to point to this application's vector table
  LDR    R0, =Vectors
  MCR    p15, 0, R0, c12, c0, 0

  // Setup Stack for each exception mode
  CPS    #0x11
  LDR    SP, =SFE(FIQ_STACK)
  CPS    #0x12
  LDR    SP, =SFE(IRQ_STACK)
  CPS    #0x13
  LDR    SP, =SFE(SVC_STACK)
  CPS    #0x17
  LDR    SP, =SFE(ABT_STACK)
  CPS    #0x1B
  LDR    SP, =SFE(UND_STACK)
  CPS    #0x1F
  LDR    SP, =SFE(USR_STACK)

  // Call SystemInit
  BL     SystemInit

  // Unmask interrupts
  CPSIE  if

  // Call __iar_program_start
  BL     __iar_program_start

/*----------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
Undef_Handler:
SVC_Handler:
PAbt_Handler:
DAbt_Handler:
IRQ_Handler:
FIQ_Handler:
Default_Handler:
  B .

  END
