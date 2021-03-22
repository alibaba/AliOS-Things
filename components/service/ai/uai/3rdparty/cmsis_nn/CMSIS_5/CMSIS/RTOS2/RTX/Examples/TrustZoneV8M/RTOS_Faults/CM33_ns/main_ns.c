/*
 * Copyright (c) 2013-2019 ARM Limited. All rights reserved.
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
 * main_ns.c      Non-secure main function - Security attacks demo
 *
 * Version 1.0
 *    Initial Release
 *---------------------------------------------------------------------------*/

#include "RTE_Components.h"
#include CMSIS_device_header

#include "..\CM33_s\interface.h"        // Interface API
#include "..\CM33_s\IncidentLog_s.h"
#include "..\CM33_s\SysTick_s.h"
#include "cmsis_os2.h"                  // ARM::CMSIS:RTOS2:Keil RTX5

static osStatus_t Status;

static osThreadId_t ThreadA_Id;
static osThreadId_t ThreadB_Id;
static osThreadId_t ThreadC_Id;
static osThreadId_t ThreadD_Id;

void ThreadA (void *argument);
void ThreadB (void *argument);
void ThreadC (void *argument);
void ThreadD (void *argument);


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
  } else {
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


/*
 * by creating a large array, PSPLIM will be exeeded
 * PSPLIM was setup by the RTOS according the thread's stack border
 */
void thread_stack_overflow (void);
void thread_stack_overflow (void) {
  volatile uint32_t foo [1024];
  uint32_t i;
  for (i=0; i<1024; i++)  {
    foo[i] = i+i;
  }
}  


/*
 * trying to call into secure memory directly is not allowed
 * address 0x1000 is in secure memory according SAU configuration
 */
void illegal_secure_call (void);
void illegal_secure_call (void)
{
  void (*FuncPointer) (void) = (void (*) (void)) 0x1000;
  FuncPointer ();
}


/*
 * an "example fault" caused ba a division by zero
 * only generates the fault when SCB_CCR_DIV_0_TRP is set
 */
void div_by_zero( void );
void div_by_zero( void )
{
  volatile unsigned int a, b, c;
  
  SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk;
  a = 1;
  b = 0;
  c = a / b;
}  


/*
 * try to let secure domain overwrite secure memory
 * secure application should detect this with buffer range checks
 */
void getdata_attack (void);
void getdata_attack (void) {
  /* provide pointer outsite of non-secure memory */
  GetIncidentLog_s ((IncidentLog_t *) (0x20200000 - 0x10));
}  


/*
 * simulate a broken non-secure application by not returning
 * a secure SysTick watchdog can be used to detect this inactivety
 */
void play_dead( void );
__NO_RETURN void play_dead( void )
{
  osKernelLock( );
  while( 1 )
  {
    __NOP( );
  }
}  


typedef struct  {
  void ( *TestFunc )( void );
  const char *TestName;
} TestCase_t;


// array of test cases with test functions
static const TestCase_t TestCases[] = {
  { illegal_secure_call,   "illegal secure call" },
  { thread_stack_overflow, "stack overflow" },
  { div_by_zero,           "div by zero" },
  { getdata_attack,        "getdata attack" },
  { play_dead,             "play dead" }
};


extern IncidentLog_t IncidentLogCopy;
IncidentLog_t IncidentLogCopy;

extern volatile uint32_t TestCase;
volatile uint32_t TestCase;


/*
 * Test case execution
 */
__NO_RETURN void ThreadD (void *argument) {
  uint32_t WatchdogToken;

  (void)argument;

  TestCase = 0xFFFFFFFF;  
  WatchdogToken = StartWatchdog_s ();                 /* start watchdog in secure mode */
  GetIncidentLog_s (&IncidentLogCopy);                /* get incident log and draw the table */

  while (1) {
    FeedWatchdog_s (WatchdogToken);
    osDelay (50);

    if (TestCase < (sizeof (TestCases) / sizeof(TestCases[0])))  {
      TestCases [TestCase].TestFunc ();               /* execute selected test */
    }
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
  ThreadD_Id = osThreadNew(ThreadD, NULL, &ThreadAttr);

  Status = osKernelStart();

  for (;;);
}
