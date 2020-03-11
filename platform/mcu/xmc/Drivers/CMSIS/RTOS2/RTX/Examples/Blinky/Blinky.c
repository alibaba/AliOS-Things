/* -------------------------------------------------------------------------- 
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
 *      Name:    BLinky.c
 *      Purpose: RTX example program
 *
 *---------------------------------------------------------------------------*/

#include <stdio.h>

#include "ARMCM3.h"                     // Device header
#include "cmsis_os2.h"                  // ARM::CMSIS:RTOS2:Keil RTX5


osThreadId_t tid_phaseA;                /* Thread id of thread: phase_a      */
osThreadId_t tid_phaseB;                /* Thread id of thread: phase_b      */
osThreadId_t tid_phaseC;                /* Thread id of thread: phase_c      */
osThreadId_t tid_phaseD;                /* Thread id of thread: phase_d      */
osThreadId_t tid_clock;                 /* Thread id of thread: clock        */

struct phases_t {
  int_fast8_t phaseA;
  int_fast8_t phaseB;
  int_fast8_t phaseC;
  int_fast8_t phaseD;
} g_phases;

osMutexId_t phases_mut_id;

/*----------------------------------------------------------------------------
 *      Switch LED on
 *---------------------------------------------------------------------------*/
void Switch_On (unsigned char led) {
  printf("LED On: #%d\n\r", led);
}

/*----------------------------------------------------------------------------
 *      Switch LED off
 *---------------------------------------------------------------------------*/
void Switch_Off (unsigned char led) {
  printf("LED Off: #%d\n\r", led);
}


/*----------------------------------------------------------------------------
 *      Function 'signal_func' called from multiple threads
 *---------------------------------------------------------------------------*/
void signal_func (osThreadId_t tid)  {
  osThreadFlagsSet(tid_clock, 0x0100);      /* set signal to clock thread    */
  osDelay(500);                             /* delay 500ms                   */
  osThreadFlagsSet(tid_clock, 0x0100);      /* set signal to clock thread    */
  osDelay(500);                             /* delay 500ms                   */
  osThreadFlagsSet(tid, 0x0001);            /* set signal to thread 'thread' */
  osDelay(500);                             /* delay 500ms                   */
}

/*----------------------------------------------------------------------------
 *      Thread 1 'phaseA': Phase A output
 *---------------------------------------------------------------------------*/
void phaseA (void *argument) {
  for (;;) {
    osThreadFlagsWait(0x0001, osFlagsWaitAny ,osWaitForever);    /* wait for an event flag 0x0001 */
    g_phases.phaseA = 1;
    signal_func(tid_phaseB);                                     /* call common signal function   */
    g_phases.phaseA = 0;
  }
}

/*----------------------------------------------------------------------------
 *      Thread 2 'phaseB': Phase B output
 *---------------------------------------------------------------------------*/
void phaseB (void *argument) {
  for (;;) {
    osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);    /* wait for an event flag 0x0001 */
    g_phases.phaseB = 1;
    signal_func(tid_phaseC);                /* call common signal function   */
    g_phases.phaseB = 0;
  }
}

/*----------------------------------------------------------------------------
 *      Thread 3 'phaseC': Phase C output
 *---------------------------------------------------------------------------*/
void phaseC (void *argument) {
  for (;;) {
    osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);    /* wait for an event flag 0x0001 */
    g_phases.phaseC = 1;
    signal_func(tid_phaseD);                /* call common signal function   */
    g_phases.phaseC = 0;
  }
}

/*----------------------------------------------------------------------------
 *      Thread 4 'phaseD': Phase D output
 *---------------------------------------------------------------------------*/
void phaseD (void *argument) {
  for (;;) {
    osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);    /* wait for an event flag 0x0001 */
    g_phases.phaseD = 1;
    signal_func(tid_phaseA);                /* call common signal function   */
    g_phases.phaseD = 0;
  }
}

/*----------------------------------------------------------------------------
 *      Thread 5 'clock': Signal Clock
 *---------------------------------------------------------------------------*/
void clock (void *argument) {
  for (;;) {
    osThreadFlagsWait(0x0100, osFlagsWaitAny, osWaitForever);    /* wait for an event flag 0x0100 */
    osDelay(80);                            /* delay  80ms                   */
  }
}

/*----------------------------------------------------------------------------
 *      Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
void app_main (void *argument) {

  tid_phaseA = osThreadNew(phaseA, NULL, NULL);
  tid_phaseB = osThreadNew(phaseB, NULL, NULL);
  tid_phaseC = osThreadNew(phaseC, NULL, NULL);
  tid_phaseD = osThreadNew(phaseD, NULL, NULL);
  tid_clock  = osThreadNew(clock,  NULL, NULL);

  osThreadFlagsSet(tid_phaseA, 0x0001);          /* set signal to phaseA thread   */

  osDelay(osWaitForever);
  while(1);
}

int main (void) {

  // System Initialization
  SystemCoreClockUpdate();
  // ...
  osKernelInitialize();                 // Initialize CMSIS-RTOS
  osThreadNew(app_main, NULL, NULL);    // Create application main thread
  if (osKernelGetState() == osKernelReady) {
    osKernelStart();                    // Start thread execution
  }

  while(1);
}
