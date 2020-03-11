/*
 * Copyright (c) 2013-2016 ARM Limited. All rights reserved.
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
 *
 * ----------------------------------------------------------------------
 *
 * Project:      ARMv8-M System Recovery demo
 * Title:        hardfault.c   Hardfault handler with incident log and restart
 *
 * Version 1.0
 *    Initial Release
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include CMSIS_device_header
#include "IncidentLog_s.h"


// \brief perform a system reset to restart the application
__NO_RETURN void PerformReset (void)    {
  __DSB( ) ;
  SCB->AIRCR = ( SCB->AIRCR & ~SCB_AIRCR_VECTKEY_Msk ) |
               ( 0x05FAUL << SCB_AIRCR_VECTKEY_Pos ) | SCB_AIRCR_SYSRESETREQ_Msk ;

	// code should never reach this (however FVP Model does not reset!)  
  while(1)   {
    __NOP() ;
  }
}


static volatile uint32_t stacked_r0;
static volatile uint32_t stacked_r1;
static volatile uint32_t stacked_r2;
static volatile uint32_t stacked_r3;
static volatile uint32_t stacked_r12;
static volatile uint32_t stacked_lr;
static volatile uint32_t stacked_pc;
static volatile uint32_t stacked_psr;

// \brief Hardfault handler in C.
// \param[in] hardfault_args address of stack frame
void hard_fault_handler_c (uint32_t *hardfault_args) {
  IncidentReason_t Reason;
  uint8_t Flags ;
  
  stacked_r0  = hardfault_args[0];
  stacked_r1  = hardfault_args[1];
  stacked_r2  = hardfault_args[2];
  stacked_r3  = hardfault_args[3];
  
  stacked_r12 = hardfault_args[4];
  stacked_lr  = hardfault_args[5];
  stacked_pc  = hardfault_args[6];
  stacked_psr = hardfault_args[7];

  /* log the incident */
  Flags = 0 ;
  if (SAU->SFSR & SAU_SFSR_INVEP_Msk)  {
    /* SecureFault, invalid Secure state entry point */
    Reason = IR_INVEP ;
    Flags |= IS_SECURE ;
  }
  else if (SCB->CFSR & SCB_CFSR_STKOF_Msk)  {
    /* UsageFault, stack overflow */ 
    Reason = IR_STKOF ;
    Flags |= IS_SECURE ;
  }
  else if (SCB->CFSR & SCB_CFSR_DIVBYZERO_Msk) {
    /* UsageFault, divide by zero when CCR.DIV_0_TRP is 1 */
    Reason = IR_DIVBY0 ;
    Flags |= IS_SECURE ;
  }
  /* AIRCR.BFHFNMINS  not set, so also non-secure faults end here */
  else if (SCB_NS->CFSR & SCB_CFSR_STKOF_Msk)  {
    /* UsageFault, stack overflow */ 
    Reason = IR_STKOF ;
  }
  else if (SCB_NS->CFSR & SCB_CFSR_DIVBYZERO_Msk)  {
    /* UsageFault, divide by zero when CCR.DIV_0_TRP is 1 */
    Reason = IR_DIVBY0 ;
  }
//todo handle SCB->SFSR AUVIOL
  else
  {
    Reason = IR_UNKNOW;
  }
  
  LogIncident (Reason, stacked_pc, Flags) ;
  PerformReset ();

// code should never reach this (Fast Model does not reset!)  
  while (1)  {
    __NOP( ) ;
  }
}


// \brief evaluate SP that was active before the exception
void HardFault_Handler (void) {
  __ASM volatile (
    "TST LR, #0x40\n"
    "BEQ from_nonsecure\n"
  "from_secure:\n"
    "TST LR, #0x04\n"
    "ITE EQ\n"
    "MRSEQ R0, MSP\n"
    "MRSNE R0, PSP\n"
    "B hard_fault_handler_c\n"
  "from_nonsecure:\n"
    "MRS R0, CONTROL_NS\n"
    "TST R0, #2\n"
    "ITE EQ\n"
    "MRSEQ R0, MSP_NS\n"
    "MRSNE R0, PSP_NS\n"
    "B hard_fault_handler_c\n"
  );
}
