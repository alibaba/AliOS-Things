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
 * Title:        SysTick_s.c   SysTick handler & timeout based watchdog 
 *
 * Version 1.0
 *    Initial Release
 *---------------------------------------------------------------------------*/

#include "SysTick_s.h"
#include "IncidentLog_s.h"
#include "Hardfault.h"
#include "RTE_Components.h"
#include CMSIS_device_header


#define TIMEOUT_VALUE (500)  /* ms */
static const uint32_t ExpectedToken = 0xAB54AB23;
uint32_t Seconds ;
static uint32_t Timeout ;


// Secure SysTick handler
void SysTick_Handler (void) {
  static unsigned int Milliseconds; 
  
  Milliseconds++ ;
  if (Milliseconds >= 1000) {
    Seconds++;
    Milliseconds = 0;
  }
  
  if (Timeout != 0) { 
    Timeout-- ;
    if (Timeout == 0) {
      LogIncident (IR_WDTEXP, 0, IS_SECURE);
      PerformReset ();
    }
  }
}


// Initialize Secure SysTick
// \param StartSeconds   setup seconds value
void InitWatchdog (uint32_t StartSeconds)  {
  Seconds = StartSeconds;
  Timeout = 0;
  SysTick_Config (SystemCoreClock / 1000);    /* 1 ms interval */
}


// Restart Watchdog
// \param Food    token obtained by StartWatchdog_s
__attribute__((cmse_nonsecure_entry))
void FeedWatchdog_s (uint32_t Food)  {
  if( Food == ExpectedToken ) {
    Timeout = TIMEOUT_VALUE;
  }
}


// Start Watchdog
// \return Food token expected by RestartWatchdog
__attribute__((cmse_nonsecure_entry))
uint32_t StartWatchdog_s (void)  {
  Timeout = TIMEOUT_VALUE;
  return (ExpectedToken);
}
