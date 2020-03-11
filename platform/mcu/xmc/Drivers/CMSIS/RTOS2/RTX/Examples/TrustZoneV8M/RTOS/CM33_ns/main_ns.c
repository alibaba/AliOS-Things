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
 * main_ns.c      Non-secure main function - RTOS demo
 *
 * Version 1.0
 *    Initial Release
 *---------------------------------------------------------------------------*/

#include "..\CM33_s\interface.h"        // Interface API
#include "cmsis_os2.h"                  // ARM::CMSIS:RTOS2:Keil RTX5

static osStatus_t Status;

static osThreadId_t ThreadA_Id;
static osThreadId_t ThreadB_Id;
static osThreadId_t ThreadC_Id;

void ThreadA (void *argument);
void ThreadB (void *argument);
void ThreadC (void *argument);


extern volatile int counterA;
extern volatile int counterB;
extern volatile int counterC;

volatile int counterA;
volatile int counterB;
volatile int counterC;


static int callbackA (int val)  {
  return (val);
}

__attribute__((noreturn))
void ThreadA (void *argument)  {
  (void)argument;

  for (;;) {
    counterA = func1 (counterA);
    counterA = func2 (callbackA, counterA);
    osDelay(2U);
  }
}

static int callbackB (int val)  {
  uint32_t flags;
  
  flags = osThreadFlagsWait (1U, osFlagsWaitAny, osWaitForever);
  if (flags == 1U)  {
    return (val+1);
  }  else {
    return (0);
  }
}


__attribute__((noreturn))
void ThreadB (void *argument)  {
  (void)argument;

  for (;;) {
    counterB = func1 (counterB);
    counterB = func2 (callbackB, counterB);
  }
}

__attribute__((noreturn))
void ThreadC (void *argument) {
  (void)argument;

  for (;;) {
    counterC = counterC + 1;
    if ((counterC % 0x10) == 0)  {
      osThreadFlagsSet (ThreadB_Id, 1);
    }
    osDelay(1U);
  }
}

static const osThreadAttr_t ThreadAttr = {
  .tz_module = 1U,                  // indicate calls to secure mode
};

int main (void) {

  Status = osKernelInitialize();

  ThreadA_Id = osThreadNew(ThreadA, NULL, &ThreadAttr);
  ThreadB_Id = osThreadNew(ThreadB, NULL, &ThreadAttr);
  ThreadC_Id = osThreadNew(ThreadC, NULL, NULL);

  Status = osKernelStart();

  for (;;);
}

