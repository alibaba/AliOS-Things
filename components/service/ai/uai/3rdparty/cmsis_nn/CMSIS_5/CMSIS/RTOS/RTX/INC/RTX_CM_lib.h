/*----------------------------------------------------------------------------
 *      CMSIS-RTOS  -  RTX
 *----------------------------------------------------------------------------
 *      Name:    RTX_CM_LIB.H
 *      Purpose: RTX Kernel System Configuration
 *      Rev.:    V4.82
 *----------------------------------------------------------------------------
 *
 * Copyright (c) 1999-2009 KEIL, 2009-2019 ARM Germany GmbH. All rights reserved.
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

 *---------------------------------------------------------------------------*/

#if   defined (__CC_ARM)
#pragma O3
#define __USED __attribute__((used))
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#define __USED __attribute__((used))
#elif defined (__GNUC__)
#pragma GCC optimize ("O3")
#define __USED __attribute__((used))
#elif defined (__ICCARM__)
#define __USED __root
#endif


/*----------------------------------------------------------------------------
 *      Definitions
 *---------------------------------------------------------------------------*/

#define _declare_box(pool,size,cnt)  uint32_t pool[(((size)+3)/4)*(cnt) + 3]
#define _declare_box8(pool,size,cnt) uint64_t pool[(((size)+7)/8)*(cnt) + 2]

#define OS_TCB_SIZE     52
#define OS_TMR_SIZE     8

#if (( defined(__CC_ARM)                                          || \
      (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))) && \
      !defined(__MICROLIB))

typedef void    *OS_ID;
typedef uint32_t OS_TID;
typedef uint32_t OS_MUT[4];
typedef uint32_t OS_RESULT;

#define runtask_id()    rt_tsk_self()
#define mutex_init(m)   rt_mut_init(m)
#define mutex_wait(m)   os_mut_wait(m,0xFFFFU)
#define mutex_rel(m)    os_mut_release(m)

extern uint8_t   os_running;
extern OS_TID    rt_tsk_self    (void);
extern void      rt_mut_init    (OS_ID mutex);
extern OS_RESULT rt_mut_release (OS_ID mutex);
extern OS_RESULT rt_mut_wait    (OS_ID mutex, uint16_t timeout);

#if defined(__CC_ARM)
#define os_mut_wait(mutex,timeout) _os_mut_wait((uint32_t)rt_mut_wait,mutex,timeout)
#define os_mut_release(mutex)      _os_mut_release((uint32_t)rt_mut_release,mutex)
OS_RESULT _os_mut_release (uint32_t p, OS_ID mutex)                   __svc_indirect(0);
OS_RESULT _os_mut_wait    (uint32_t p, OS_ID mutex, uint16_t timeout) __svc_indirect(0);
#else
__attribute__((always_inline))
static __inline OS_RESULT os_mut_release (OS_ID mutex) {
  register uint32_t __rf __asm("r12") = (uint32_t)rt_mut_release;
  register uint32_t __r0 __asm("r0")  = (uint32_t)mutex;
  __asm volatile                                                               \
  (                                                                            \
    "svc 0"                                                                    \
    :            "=r" (__r0)                                                   \
    : "r" (__rf), "r" (__r0)                                                   \
    : "r1", "r2"                                                               \
  );
  return (OS_RESULT)__r0;
}
__attribute__((always_inline))
static __inline OS_RESULT os_mut_wait (OS_ID mutex, uint16_t timeout) {
  register uint32_t __rf __asm("r12") = (uint32_t)rt_mut_wait;
  register uint32_t __r0 __asm("r0")  = (uint32_t)mutex;
  register uint32_t __r1 __asm("r1")  = (uint32_t)timeout;
  __asm volatile                                                               \
  (                                                                            \
    "svc 0"                                                                    \
    :            "=r" (__r0)                                                   \
    : "r" (__rf), "r" (__r0), "r" (__r1)                                       \
    : "r2"                                                                     \
  );
  return (OS_RESULT)__r0;
}
#endif

#endif


/*----------------------------------------------------------------------------
 *      Global Variables
 *---------------------------------------------------------------------------*/

#if (OS_TASKCNT == 0)
#error "Invalid number of concurrent running threads!"
#endif

#if (OS_PRIVCNT >= OS_TASKCNT)
#error "Too many threads with user-provided stack size!"
#endif

#if (OS_TIMERS != 0)
#define OS_TASK_CNT (OS_TASKCNT + 1)
#define OS_PRIV_CNT (OS_PRIVCNT + 2)
#define OS_STACK_SZ (4*(OS_PRIVSTKSIZE+OS_MAINSTKSIZE+OS_TIMERSTKSZ))
#else
#define OS_TASK_CNT  OS_TASKCNT
#define OS_PRIV_CNT (OS_PRIVCNT + 1)
#define OS_STACK_SZ (4*(OS_PRIVSTKSIZE+OS_MAINSTKSIZE))
#endif

#ifndef OS_STKINIT
#define OS_STKINIT  0
#endif

extern uint16_t const os_maxtaskrun;
extern uint32_t const os_stackinfo;
extern uint32_t const os_rrobin;
extern uint32_t const os_trv;
extern uint8_t  const os_flags;

uint16_t const os_maxtaskrun = OS_TASK_CNT;
uint32_t const os_stackinfo  = (OS_STKINIT<<28) | (OS_STKCHECK<<24) | (OS_PRIV_CNT<<16) | (OS_STKSIZE*4);
uint32_t const os_rrobin     = (OS_ROBIN << 16) | OS_ROBINTOUT;
uint32_t const os_tickfreq   = OS_CLOCK;
uint16_t const os_tickus_i   = OS_CLOCK/1000000;
uint16_t const os_tickus_f   = (((uint64_t)(OS_CLOCK-1000000*(OS_CLOCK/1000000)))<<16)/1000000;
uint32_t const os_trv        = OS_TRV;
uint8_t  const os_flags      = OS_RUNPRIV;

/* Export following defines to uVision debugger. */
extern uint32_t const CMSIS_RTOS_API_Version;
__USED uint32_t const CMSIS_RTOS_API_Version = osCMSIS;
extern uint32_t const CMSIS_RTOS_RTX_Version;
__USED uint32_t const CMSIS_RTOS_RTX_Version = osCMSIS_RTX;
extern uint32_t const os_clockrate;
__USED uint32_t const os_clockrate = OS_TICK;
extern uint32_t const os_timernum;
__USED uint32_t const os_timernum  = 0U;

/* Memory pool for TCB allocation    */
extern
uint32_t       mp_tcb[];
_declare_box  (mp_tcb, OS_TCB_SIZE, OS_TASK_CNT);
extern
uint16_t const mp_tcb_size;
uint16_t const mp_tcb_size = sizeof(mp_tcb);

/* Memory pool for System stack allocation (+os_idle_demon). */
extern
uint64_t       mp_stk[];
_declare_box8 (mp_stk, OS_STKSIZE*4, OS_TASK_CNT-OS_PRIV_CNT+1);
extern
uint32_t const mp_stk_size;
uint32_t const mp_stk_size = sizeof(mp_stk);

/* Memory pool for user specified stack allocation (+main, +timer) */
extern
uint64_t       os_stack_mem[];
uint64_t       os_stack_mem[2+OS_PRIV_CNT+(OS_STACK_SZ/8)];
extern
uint32_t const os_stack_sz;
uint32_t const os_stack_sz = sizeof(os_stack_mem);

#ifndef OS_FIFOSZ
#define OS_FIFOSZ       16
#endif

/* Fifo Queue buffer for ISR requests.*/
extern
uint32_t       os_fifo[];
uint32_t       os_fifo[OS_FIFOSZ*2+1];
extern
uint8_t  const os_fifo_size;
uint8_t  const os_fifo_size = OS_FIFOSZ;

/* An array of Active task pointers. */
extern
void *os_active_TCB[];
void *os_active_TCB[OS_TASK_CNT];

/* User Timers Resources */
#if (OS_TIMERS != 0)
extern void osTimerThread (void const *argument);
extern const osThreadDef_t os_thread_def_osTimerThread;
osThreadDef(osTimerThread, (osPriority)(OS_TIMERPRIO-3), 1, 4*OS_TIMERSTKSZ);
extern
osThreadId osThreadId_osTimerThread;
osThreadId osThreadId_osTimerThread;
extern uint32_t os_messageQ_q_osTimerMessageQ[];
extern const osMessageQDef_t os_messageQ_def_osTimerMessageQ;
osMessageQDef(osTimerMessageQ, OS_TIMERCBQS, void *);
extern
osMessageQId osMessageQId_osTimerMessageQ;
osMessageQId osMessageQId_osTimerMessageQ;
#else
extern
const osThreadDef_t os_thread_def_osTimerThread;
const osThreadDef_t os_thread_def_osTimerThread = { NULL, osPriorityNormal, 0U, 0U };
extern
osThreadId osThreadId_osTimerThread;
osThreadId osThreadId_osTimerThread;
extern uint32_t os_messageQ_q_osTimerMessageQ[];
extern const osMessageQDef_t os_messageQ_def_osTimerMessageQ;
osMessageQDef(osTimerMessageQ, 0U, void *);
extern
osMessageQId osMessageQId_osTimerMessageQ;
osMessageQId osMessageQId_osTimerMessageQ;
#endif

/* Legacy RTX User Timers not used */
extern
uint32_t       os_tmr; 
uint32_t       os_tmr = 0U; 
extern
uint32_t const *m_tmr;
uint32_t const *m_tmr = NULL;
extern
uint16_t const mp_tmr_size;
uint16_t const mp_tmr_size = 0U;

#if (( defined(__CC_ARM)                                          || \
      (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))) && \
      !defined(__MICROLIB))
/* A memory space for arm standard library. */
static uint32_t std_libspace[OS_TASK_CNT][96/4];
static OS_MUT   std_libmutex[OS_MUTEXCNT];
static uint32_t nr_mutex;
extern void  *__libspace_start;
#endif


/*----------------------------------------------------------------------------
 *      RTX Optimizations (empty functions)
 *---------------------------------------------------------------------------*/

#if OS_ROBIN == 0
extern
void rt_init_robin (void);
void rt_init_robin (void) {;}
extern
void rt_chk_robin  (void);
void rt_chk_robin  (void) {;}
#endif

#if OS_STKCHECK == 0
extern
void rt_stk_check  (void);
void rt_stk_check  (void) {;}
#endif


/*----------------------------------------------------------------------------
 *      Standard Library multithreading interface
 *---------------------------------------------------------------------------*/

#if (( defined(__CC_ARM)                                          || \
      (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))) && \
      !defined(__MICROLIB))

/*--------------------------- __user_perthread_libspace ---------------------*/

void *__user_perthread_libspace (void);
void *__user_perthread_libspace (void) {
  /* Provide a separate libspace for each task. */
  uint32_t idx;

  idx = (os_running != 0U) ? runtask_id () : 0U;
  if (idx == 0U) {
    /* RTX not running yet. */
    return (&__libspace_start);
  }
  return ((void *)&std_libspace[idx-1]);
}

/*--------------------------- _mutex_initialize -----------------------------*/

int _mutex_initialize (OS_ID *mutex);
int _mutex_initialize (OS_ID *mutex) {
  /* Allocate and initialize a system mutex. */

  if (nr_mutex >= OS_MUTEXCNT) {
    /* If you are here, you need to increase the number OS_MUTEXCNT. */
    for (;;);
  }
  *mutex = &std_libmutex[nr_mutex++];
  mutex_init (*mutex);
  return (1);
}


/*--------------------------- _mutex_acquire --------------------------------*/

__attribute__((used))
void _mutex_acquire (OS_ID *mutex);
void _mutex_acquire (OS_ID *mutex) {
  /* Acquire a system mutex, lock stdlib resources. */
  if (os_running) {
    /* RTX running, acquire a mutex. */
    mutex_wait (*mutex);
  }
}


/*--------------------------- _mutex_release --------------------------------*/

__attribute__((used))
void _mutex_release (OS_ID *mutex);
void _mutex_release (OS_ID *mutex) {
  /* Release a system mutex, unlock stdlib resources. */
  if (os_running) {
    /* RTX running, release a mutex. */
    mutex_rel (*mutex);
  }
}

#endif


/*----------------------------------------------------------------------------
 *      ARMCC6 Wrappers for ARMCC5 Binary
 *---------------------------------------------------------------------------*/

#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)

typedef uint32_t __attribute__((vector_size(8)))  vect64_t;

#undef osSignalWait

__attribute__((pcs("aapcs")))
vect64_t  osSignalWait (int32_t signals, uint32_t millisec);

osEvent __osSignalWait (int32_t signals, uint32_t millisec) {
  vect64_t v;
  osEvent  e;
  
  v = osSignalWait(signals, millisec);
  e.status  = v[0];
  e.value.v = v[1];

  return e;
}

#undef osMessageGet

__attribute__((pcs("aapcs")))
vect64_t  osMessageGet (osMessageQId queue_id, uint32_t millisec);

osEvent __osMessageGet (osMessageQId queue_id, uint32_t millisec) {
  vect64_t v;
  osEvent  e;
  
  v = osMessageGet(queue_id, millisec);
  e.status  = v[0];
  e.value.v = v[1];

  return e;
}

#undef osMailGet

__attribute__((pcs("aapcs")))
vect64_t  osMailGet (osMailQId queue_id, uint32_t millisec);

osEvent __osMailGet (osMailQId queue_id, uint32_t millisec) {
  vect64_t v;
  osEvent  e;
  
  v = osMailGet(queue_id, millisec);
  e.status  = v[0];
  e.value.v = v[1];

  return e;
}

#endif

/*----------------------------------------------------------------------------
 *      RTX Startup
 *---------------------------------------------------------------------------*/

/* Main Thread definition */
extern int main (void);
extern
const osThreadDef_t os_thread_def_main;
const osThreadDef_t os_thread_def_main = {(os_pthread)main, osPriorityNormal, 1U, 4*OS_MAINSTKSIZE };


#if defined (__CC_ARM)

#ifdef __MICROLIB
__attribute__((section(".ARM.Collect$$$$000000FF")))
void _main_init (void);
void _main_init (void) {
  osKernelInitialize();
  osThreadCreate(&os_thread_def_main, NULL);
  osKernelStart();
  for (;;);
}
#else
__asm void _platform_post_lib_init (void) {

  IMPORT  os_thread_def_main
  IMPORT  osKernelInitialize
  IMPORT  osKernelStart
  IMPORT  osThreadCreate
  IMPORT  exit

  ADD     SP,#0x10
  BL      osKernelInitialize
  LDR     R0,=os_thread_def_main
  MOVS    R1,#0
  BL      osThreadCreate
  BL      osKernelStart
  BL      exit

  ALIGN
}
#endif

#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)

#ifdef __MICROLIB
__attribute__((noreturn, section(".ARM.Collect$$$$000000FF")))
void _main_init (void);
void _main_init (void) {
#else
__asm(" .global __ARM_use_no_argv\n");
__attribute__((noreturn))
void _platform_post_lib_init (void);
void _platform_post_lib_init (void) {
#endif
  osKernelInitialize();
  osThreadCreate(&os_thread_def_main, NULL);
  osKernelStart();
  for (;;);
}

#elif defined (__GNUC__)

#ifdef __CS3__

/* CS3 start_c routine.
 *
 * Copyright (c) 2006, 2007 CodeSourcery Inc
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */

#include "cs3.h"

extern void __libc_init_array (void);

__attribute ((noreturn)) void __cs3_start_c (void){
  unsigned regions = __cs3_region_num;
  const struct __cs3_region *rptr = __cs3_regions;

  /* Initialize memory */
  for (regions = __cs3_region_num, rptr = __cs3_regions; regions--; rptr++) {
    long long *src = (long long *)rptr->init;
    long long *dst = (long long *)rptr->data;
    unsigned limit = rptr->init_size;
    unsigned count;

    if (src != dst)
      for (count = 0; count != limit; count += sizeof (long long))
        *dst++ = *src++;
    else 
      dst = (long long *)((char *)dst + limit);
    limit = rptr->zero_size;
    for (count = 0; count != limit; count += sizeof (long long))
      *dst++ = 0;
  }

  /* Run initializers.  */
  __libc_init_array ();

  osKernelInitialize();
  osThreadCreate(&os_thread_def_main, NULL);
  osKernelStart();
  for (;;);
}

#else

__attribute__((naked)) void software_init_hook (void) {
  __asm (
    ".syntax unified\n"
    ".thumb\n"
    "movs r0,#0\n"
    "movs r1,#0\n"
    "mov  r4,r0\n"
    "mov  r5,r1\n"
    "ldr  r0,= __libc_fini_array\n"
    "bl   atexit\n"
    "bl   __libc_init_array\n"
    "mov  r0,r4\n"
    "mov  r1,r5\n"
    "bl   osKernelInitialize\n"
    "ldr  r0,=os_thread_def_main\n"
    "movs r1,#0\n"
    "bl   osThreadCreate\n"
    "bl   osKernelStart\n"
    "bl   exit\n"
  );
}

#endif

#elif defined (__ICCARM__)

extern int  __low_level_init(void);
extern void __iar_data_init3(void);
extern void exit(int arg);

__noreturn __stackless void __cmain(void) {
  int a;
  
  if (__low_level_init() != 0) {
    __iar_data_init3();
  }
  osKernelInitialize();
  osThreadCreate(&os_thread_def_main, NULL);
  a = osKernelStart();
  exit(a);
}

#endif

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
