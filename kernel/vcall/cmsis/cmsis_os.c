/* ----------------------------------------------------------------------
 * $Date:        5. February 2013
 * $Revision:    V1.02
 *
 * Project:      CMSIS-RTOS API
 * Title:        cmsis_os.c
 *
 * Version 0.02
 *    Initial Proposal Phase
 * Version 0.03
 *    osKernelStart added, optional feature: main started as thread
 *    osSemaphores have standard behavior
 *    osTimerCreate does not start the timer, added osTimerStart
 *    osThreadPass is renamed to osThreadYield
 * Version 1.01
 *    Support for C++ interface
 *     - const attribute removed from the osXxxxDef_t typedef's
 *     - const attribute added to the osXxxxDef macros
 *    Added: osTimerDelete, osMutexDelete, osSemaphoreDelete
 *    Added: osKernelInitialize
 * Version 1.02
 *    Control functions for short timeouts in microsecond resolution:
 *    Added: osKernelSysTick, osKernelSysTickFrequency, osKernelSysTickMicroSec
 *    Removed: osSignalGet
 *
 *
 *----------------------------------------------------------------------------
 *
 * Portions Copyright � 2016 STMicroelectronics International N.V. All rights reserved.
 * Portions Copyright (c) 2013 ARM LIMITED
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of ARM  nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/**
 ******************************************************************************
 * @file    cmsis_os.c
 * @author  AliOS-Things Team
 * @date    10-Mar-2018
 * @brief   CMSIS-RTOS API implementation for AliOS-Things
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 ******************************************************************************
 */

#include <string.h>
#include <cmsis_os.h>

/*
 * ARM Compiler 4/5
 */
#if   defined ( __CC_ARM )

#define __ASM                 __asm
#define __INLINE              __inline
#define __STATIC_INLINE       static __inline

#include "cmsis_armcc.h"

/*
 * GNU Compiler
 */
#elif defined ( __GNUC__ )

#define __ASM                 __asm                                      /*!< asm keyword for GNU Compiler          */
#define __INLINE              inline                                     /*!< inline keyword for GNU Compiler       */
#define __STATIC_INLINE       static inline

#include "cmsis_gcc.h"

/*
 * IAR Compiler
 */
#elif defined ( __ICCARM__ )

#ifndef   __ASM
#define __ASM               __asm
#endif
#ifndef   __INLINE
#define __INLINE            inline
#endif
#ifndef   __STATIC_INLINE
#define __STATIC_INLINE     static inline
#endif

#include <cmsis_iar.h>
#endif

/* Convert from CMSIS type osPriority to Rhino priority number
osPriorityIdle          = -3,  ->  62 - ((-3) - (-3)) = 62    ///< priority: idle (lowest)
osPriorityLow           = -2,  ->  62 - ((-2) - (-3)) = 61    ///< priority: low
osPriorityBelowNormal   = -1,  ->  62 - ((-1) - (-3)) = 60    ///< priority: below normal
osPriorityNormal        =  0,  ->  62 - (( 0) - (-3)) = 59    ///< priority: normal (default)
osPriorityAboveNormal   = +1,  ->  62 - ((+1) - (-3)) = 58    ///< priority: above normal
osPriorityHigh          = +2,  ->  62 - ((+2) - (-3)) = 57    ///< priority: high
osPriorityRealtime      = +3,  ->  62 - ((+3) - (-3)) = 56    ///< priority: realtime (highest)
osPriorityError         =  0x84        ///< system cannot determine priority or thread has illegal priority
******************************************************************/
static uint8_t makeRhinoPriority (osPriority priority)
{
    uint8_t fpriority = RHINO_IDLE_PRI;

    if (priority != osPriorityError) {
        fpriority -= (priority - osPriorityIdle);
    }

    return fpriority;
}

/* Convert from Rhino priority number to CMSIS type osPriority */
static osPriority makeCmsisPriority (uint8_t fpriority)
{
    osPriority priority = osPriorityError;

    if ((RHINO_IDLE_PRI - fpriority) <= (osPriorityRealtime - osPriorityIdle)) {
        priority = (osPriority)((int)osPriorityIdle + (int)(RHINO_IDLE_PRI - fpriority));
    }

    return priority;
}


/*********************** Kernel Control Functions *****************************/
/**
* @brief  Initialize the RTOS Kernel for creating objects.
* @retval status code that indicates the execution status of the function.
* @note   MUST REMAIN UNCHANGED: \b osKernelInitialize shall be consistent in every CMSIS-RTOS.
*/
osStatus osKernelInitialize (void)
{
    krhino_init();

    return osOK;
}

/**
* @brief  Start the RTOS Kernel with executing the specified thread.
* @param  thread_def    thread definition referenced with \ref osThread.
* @param  argument      pointer that is passed to the thread function as start argument.
* @retval status code that indicates the execution status of the function
* @note   MUST REMAIN UNCHANGED: \b osKernelStart shall be consistent in every CMSIS-RTOS.
*/
osStatus osKernelStart (void)
{
    krhino_start();

    return osOK;
}

/**
* @brief  Check if the RTOS kernel is already started
* @param  None
* @retval (0) RTOS is not started
*         (1) RTOS is started
*         (-1) if this feature is disabled
* @note  MUST REMAIN UNCHANGED: \b osKernelRunning shall be consistent in every CMSIS-RTOS.
*/
int32_t osKernelRunning(void)
{
    if (g_sys_stat == RHINO_RUNNING) {
        return 1;
    } else {
        return 0;
    }
}

#if (defined (osFeature_SysTick)  &&  (osFeature_SysTick != 0))     // System Timer available
/**
* @brief  Get the value of the Kernel SysTick timer
* @param  None
* @retval None
* @note   MUST REMAIN UNCHANGED: \b osKernelSysTick shall be consistent in every CMSIS-RTOS.
*/
uint32_t osKernelSysTick(void)
{
    return (uint32_t)g_tick_count;
}
#endif    // System Timer available

/*********************** Thread Management *****************************/
/**
* @brief  Create a thread and add it to Active Threads and set it to state READY.
* @param  thread_def    thread definition referenced with \ref osThread.
* @param  argument      pointer that is passed to the thread function as start argument.
* @retval thread ID for reference by other functions or NULL in case of error.
* @note   MUST REMAIN UNCHANGED: \b osThreadCreate shall be consistent in every CMSIS-RTOS.
*/
osThreadId osThreadCreate (const osThreadDef_t *thread_def, void *argument)
{
    ktask_t *ptcb;

    if (RHINO_SUCCESS != krhino_task_create(thread_def->ptcb, thread_def->name, argument,
                                            makeRhinoPriority(thread_def->tpriority),
                                            thread_def->ticks, thread_def->pstackspace,
                                            thread_def->stacksize,
                                            (task_entry_t)thread_def->pthread, 1)) {
        return NULL;
    } else {
        ptcb = thread_def->ptcb;
    }

    return (osThreadId)ptcb;
}

/**
* @brief  Return the thread ID of the current running thread.
* @retval thread ID for reference by other functions or NULL in case of error.
* @note   MUST REMAIN UNCHANGED: \b osThreadGetId shall be consistent in every CMSIS-RTOS.
*/
osThreadId osThreadGetId (void)
{
    return (osThreadId)krhino_cur_task_get();
}

/**
* @brief  Terminate execution of a thread and remove it from Active Threads.
* @param   thread_id   thread ID obtained by \ref osThreadCreate or \ref osThreadGetId.
* @retval  status code that indicates the execution status of the function.
* @note   MUST REMAIN UNCHANGED: \b osThreadTerminate shall be consistent in every CMSIS-RTOS.
*/
osStatus osThreadTerminate (osThreadId thread_id)
{
#if (RHINO_CONFIG_TASK_DEL > 0)
    if (RHINO_SUCCESS == krhino_task_del((ktask_t *)thread_id)) {
        return osOK;
    } else {
        return osErrorOS;
    }
#else
    return osErrorOS;
#endif
}

/**
* @brief  Pass control to next thread that is in state \b READY.
* @retval status code that indicates the execution status of the function.
* @note   MUST REMAIN UNCHANGED: \b osThreadYield shall be consistent in every CMSIS-RTOS.
*/
osStatus osThreadYield (void)
{
    krhino_task_yield();

    return osOK;
}

/**
* @brief   Change priority of an active thread.
* @param   thread_id     thread ID obtained by \ref osThreadCreate or \ref osThreadGetId.
* @param   priority      new priority value for the thread function.
* @retval  status code that indicates the execution status of the function.
* @note   MUST REMAIN UNCHANGED: \b osThreadSetPriority shall be consistent in every CMSIS-RTOS.
*/
osStatus osThreadSetPriority (osThreadId thread_id, osPriority priority)
{
#if (RHINO_CONFIG_TASK_PRI_CHG > 0)
    if (RHINO_SUCCESS == task_pri_change((ktask_t *)thread_id, makeRhinoPriority(priority))) {
        return osOK;
    } else {
        return osErrorOS;
    }
#else
    return osErrorOS;
#endif
}

/**
* @brief   Get current priority of an active thread.
* @param   thread_id     thread ID obtained by \ref osThreadCreate or \ref osThreadGetId.
* @retval  current priority value of the thread function.
* @note   MUST REMAIN UNCHANGED: \b osThreadGetPriority shall be consistent in every CMSIS-RTOS.
*/
osPriority osThreadGetPriority (osThreadId thread_id)
{
    ktask_t *ptcb;

    if (thread_id == NULL) {
        return osPriorityError;
    }

    ptcb = (ktask_t *)thread_id;

    return makeCmsisPriority(ptcb->prio);
}

/*********************** Generic Wait Functions *******************************/
/**
* @brief   Wait for Timeout (Time Delay)
* @param   millisec      time delay value
* @retval  status code that indicates the execution status of the function.
*/
osStatus osDelay (uint32_t millisec)
{
    tick_t ticks = 0;

    if (millisec == 0) {
        ticks = RHINO_NO_WAIT;
    } else if (millisec == osWaitForever) {
        ticks = RHINO_WAIT_FOREVER;
    } else {
        ticks = (millisec * RHINO_CONFIG_TICKS_PER_SECOND) / 1000;
    }

    krhino_task_sleep(ticks ? ticks : 1); /* Minimum delay = 1 tick */
}

#if (defined (osFeature_Wait)  &&  (osFeature_Wait != 0)) /* Generic Wait available */
/**
* @brief  Wait for Signal, Message, Mail, or Timeout
* @param   millisec  timeout value or 0 in case of no time-out
* @retval  event that contains signal, message, or mail information or error code.
* @note   MUST REMAIN UNCHANGED: \b osWait shall be consistent in every CMSIS-RTOS.
*/
osEvent osWait (uint32_t millisec)
{
    osEvent ret;

    ret.status =  osErrorOS;  /* Task Notification not supported */

    return ret;
}

#endif  /* Generic Wait available */

/***********************  Timer Management Functions ***************************/
/**
* @brief  Create a timer.
* @param  timer_def     timer object referenced with \ref osTimer.
* @param  type          osTimerOnce for one-shot or osTimerPeriodic for periodic behavior.
* @param  argument      argument to the timer call back function.
* @retval  timer ID for reference by other functions or NULL in case of error.
* @note   MUST REMAIN UNCHANGED: \b osTimerCreate shall be consistent in every CMSIS-RTOS.
*/
osTimerId osTimerCreate (const osTimerDef_t *timer_def, os_timer_type type, void *argument)
{
    sys_time_t first = 1;
    sys_time_t round;

    /*
     * we use the round to save information for : one-shot or periodic,
     * when type == osTimerPeriodic, round is set to MAX_TIMER_TICKS - 1,
     * when type == osTimerOnce, round is set to 0.
     * osTimerStart() will set the right value for round again.
     *
     * also the parameter first will be set to 1 here for workround error
     * in krhino_timer_create(), osTimerStart() will set the right value.
     */

    if (type == osTimerPeriodic) {
        round = MAX_TIMER_TICKS - 1;
    } else {
        round = 0;
    }

    if (RHINO_SUCCESS == krhino_timer_create(timer_def->timer,
                                             timer_def->name, (timer_cb_t)timer_def->cb,
                                             first, round, argument, 0)) {
        return (osTimerId)timer_def->timer;
    } else {
        return NULL;
    }
}

#if (RHINO_CONFIG_TIMER > 0)

/**
* @brief  Start or restart a timer.
* @param  timer_id      timer ID obtained by \ref osTimerCreate.
* @param  millisec      time delay value of the timer.
* @retval  status code that indicates the execution status of the function
* @note   MUST REMAIN UNCHANGED: \b osTimerStart shall be consistent in every CMSIS-RTOS.
*/
osStatus osTimerStart (osTimerId timer_id, uint32_t millisec)
{
    osStatus   result = osOK;
    ktimer_t *ptimer = (ktimer_t *)timer_id;
    tick_t     ticks  = 0;

    if (millisec == 0) {
        ticks = RHINO_NO_WAIT;
    } else if (millisec == osWaitForever) {
        ticks = RHINO_WAIT_FOREVER;
    } else {
        ticks = (millisec * RHINO_CONFIG_TICKS_PER_SECOND) / 1000;
    }

    if (ticks == 0) {
        ticks = 1;
    }

    ptimer->init_count = ticks;

    /* check the type of timer, osTimerPeriodic or osTimerOnce*/

    if ((MAX_TIMER_TICKS - 1) == ptimer->round_ticks) {
        ptimer->round_ticks = ticks;
    }

    if (RHINO_SUCCESS != krhino_timer_start(ptimer)) {
        result = osErrorOS;
    }

    return result;
}

/**
* @brief  Stop a timer.
* @param  timer_id      timer ID obtained by \ref osTimerCreate
* @retval  status code that indicates the execution status of the function.
* @note   MUST REMAIN UNCHANGED: \b osTimerStop shall be consistent in every CMSIS-RTOS.
*/
osStatus osTimerStop (osTimerId timer_id)
{
    osStatus result = osOK;

    if (RHINO_SUCCESS != krhino_timer_stop((ktimer_t *)timer_id)) {
        result = osErrorOS;
    }

    return result;
}

/**
* @brief  Delete a timer.
* @param  timer_id      timer ID obtained by \ref osTimerCreate
* @retval  status code that indicates the execution status of the function.
* @note   MUST REMAIN UNCHANGED: \b osTimerDelete shall be consistent in every CMSIS-RTOS.
*/
osStatus osTimerDelete (osTimerId timer_id)
{
    osStatus result = osOK;

    if (RHINO_SUCCESS != krhino_timer_del((ktimer_t *)timer_id)) {
        result = osErrorOS;
    }

    return result;
}
#endif /* RHINO_CONFIG_TIMER */

#if (osFeature_Signals > 0)
/***************************  Signal Management ********************************/
/**
* @brief  Set the specified Signal Flags of an active thread.
* @param  thread_id     thread ID obtained by \ref osThreadCreate or \ref osThreadGetId.
* @param  signals       specifies the signal flags of the thread that should be set.
* @retval previous signal flags of the specified thread or 0x80000000 in case of incorrect parameters.
* @note   MUST REMAIN UNCHANGED: \b osSignalSet shall be consistent in every CMSIS-RTOS.
*/
int32_t osSignalSet (osThreadId thread_id, int32_t signal)
{
    return 0x80000000; /* Task Notification not supported */
}

/**
* @brief  Clear the specified Signal Flags of an active thread.
* @param  thread_id  thread ID obtained by \ref osThreadCreate or \ref osThreadGetId.
* @param  signals    specifies the signal flags of the thread that shall be cleared.
* @retval  previous signal flags of the specified thread or 0x80000000 in case of incorrect parameters.
* @note   MUST REMAIN UNCHANGED: \b osSignalClear shall be consistent in every CMSIS-RTOS.
*/
int32_t osSignalClear (osThreadId thread_id, int32_t signal)
{
    return 0x80000000; /* Task Notification not supported */
}

/**
* @brief  Wait for one or more Signal Flags to become signaled for the current \b RUNNING thread.
* @param  signals   wait until all specified signal flags set or 0 for any single signal flag.
* @param  millisec  timeout value or 0 in case of no time-out.
* @retval  event flag information or error code.
* @note   MUST REMAIN UNCHANGED: \b osSignalWait shall be consistent in every CMSIS-RTOS.
*/
osEvent osSignalWait (int32_t signals, uint32_t millisec)
{
    osEvent ret;

    ret.status =  osErrorOS;    /* Task Notification not supported */

    return ret;
}
#endif /* osFeature_Signals > 0 */

/****************************  Mutex Management ********************************/
/**
* @brief  Create and Initialize a Mutex object
* @param  mutex_def     mutex definition referenced with \ref osMutex.
* @retval  mutex ID for reference by other functions or NULL in case of error.
* @note   MUST REMAIN UNCHANGED: \b osMutexCreate shall be consistent in every CMSIS-RTOS.
*/
osMutexId osMutexCreate (const osMutexDef_t *mutex_def)
{
    if (RHINO_SUCCESS != krhino_mutex_create(mutex_def->mutex, mutex_def->name)) {
        return NULL;
    } else {
        return (osMutexId)mutex_def->mutex;
    }
}

/**
* @brief Wait until a Mutex becomes available
* @param mutex_id      mutex ID obtained by \ref osMutexCreate.
* @param millisec      timeout value or 0 in case of no time-out.
* @retval  status code that indicates the execution status of the function.
* @note   MUST REMAIN UNCHANGED: \b osMutexWait shall be consistent in every CMSIS-RTOS.
*/
osStatus osMutexWait (osMutexId mutex_id, uint32_t millisec)
{
    tick_t ticks = 0;
    kstat_t ret = 0;

    if (mutex_id == NULL) {
        return osErrorParameter;
    }

    if (millisec == 0) {
        ticks = RHINO_NO_WAIT;
    } else if (millisec == osWaitForever) {
        ticks = RHINO_WAIT_FOREVER;
    } else {
        ticks = (millisec * RHINO_CONFIG_TICKS_PER_SECOND) / 1000;
    }

    ret = krhino_mutex_lock((kmutex_t *)mutex_id, ticks);

    if ((ret == RHINO_SUCCESS) || (ret == RHINO_MUTEX_OWNER_NESTED)) {
        return osOK;
    } else {
        return osErrorOS;
    }
}

/**
* @brief Release a Mutex that was obtained by \ref osMutexWait
* @param mutex_id      mutex ID obtained by \ref osMutexCreate.
* @retval  status code that indicates the execution status of the function.
* @note   MUST REMAIN UNCHANGED: \b osMutexRelease shall be consistent in every CMSIS-RTOS.
*/
osStatus osMutexRelease (osMutexId mutex_id)
{
    kstat_t ret = 0;

    ret = krhino_mutex_unlock((kmutex_t *)mutex_id);

    if ((ret == RHINO_SUCCESS) || (ret == RHINO_MUTEX_OWNER_NESTED)) {
        return osOK;
    } else {
        return osErrorOS;
    }
}

/**
* @brief Delete a Mutex
* @param mutex_id  mutex ID obtained by \ref osMutexCreate.
* @retval  status code that indicates the execution status of the function.
* @note   MUST REMAIN UNCHANGED: \b osMutexDelete shall be consistent in every CMSIS-RTOS.
*/
osStatus osMutexDelete (osMutexId mutex_id)
{
    if (RHINO_SUCCESS == krhino_mutex_del((kmutex_t *)mutex_id)) {
        return osOK;
    } else {
        return osErrorOS;
    }
}

/********************  Semaphore Management Functions **************************/

#if (RHINO_CONFIG_SEM > 0)

/**
* @brief Create and Initialize a Semaphore object used for managing resources
* @param semaphore_def semaphore definition referenced with \ref osSemaphore.
* @param count         number of available resources.
* @retval  semaphore ID for reference by other functions or NULL in case of error.
* @note   MUST REMAIN UNCHANGED: \b osSemaphoreCreate shall be consistent in every CMSIS-RTOS.
*/
osSemaphoreId osSemaphoreCreate (const osSemaphoreDef_t *semaphore_def, int32_t count)
{
    if (RHINO_SUCCESS != krhino_sem_create(semaphore_def->sem,
                                           semaphore_def->name, (sem_count_t)count)) {
        return NULL;
    } else {
        return (osSemaphoreId)semaphore_def->sem;
    }
}

/**
* @brief Wait until a Semaphore token becomes available
* @param  semaphore_id  semaphore object referenced with \ref osSemaphore.
* @param  millisec      timeout value or 0 in case of no time-out.
* @retval  number of available tokens, or -1 in case of incorrect parameters.
* @note   MUST REMAIN UNCHANGED: \b osSemaphoreWait shall be consistent in every CMSIS-RTOS.
*/
int32_t osSemaphoreWait (osSemaphoreId semaphore_id, uint32_t millisec)
{
    tick_t ticks = 0;

    if (semaphore_id == NULL) {
        return osErrorParameter;
    }

    if (millisec == 0) {
        ticks = RHINO_NO_WAIT;
    } else if (millisec == osWaitForever) {
        ticks = RHINO_WAIT_FOREVER;
    } else {
        ticks = (millisec * RHINO_CONFIG_TICKS_PER_SECOND) / 1000;
    }

    if (RHINO_SUCCESS != krhino_sem_take((ksem_t *)semaphore_id, ticks)) {
        return osErrorOS;
    }

    return osOK;
}

/**
* @brief Release a Semaphore token
* @param  semaphore_id  semaphore object referenced with \ref osSemaphore.
* @retval  status code that indicates the execution status of the function.
* @note   MUST REMAIN UNCHANGED: \b osSemaphoreRelease shall be consistent in every CMSIS-RTOS.
*/
osStatus osSemaphoreRelease (osSemaphoreId semaphore_id)
{
    if (RHINO_SUCCESS == krhino_sem_give((ksem_t *)semaphore_id)) {
        return osOK;
    } else {
        return osErrorOS;
    }
}

/**
* @brief Delete a Semaphore
* @param  semaphore_id  semaphore object referenced with \ref osSemaphore.
* @retval  status code that indicates the execution status of the function.
* @note   MUST REMAIN UNCHANGED: \b osSemaphoreDelete shall be consistent in every CMSIS-RTOS.
*/
osStatus osSemaphoreDelete (osSemaphoreId semaphore_id)
{
    if (RHINO_SUCCESS == krhino_sem_del((ksem_t *)semaphore_id)) {
        return osOK;
    } else {
        return osErrorOS;
    }
}

#endif    /* Use Semaphores */

/*******************   Memory Pool Management Functions  ***********************/

#if (defined (osFeature_Pool)  &&  (osFeature_Pool != 0))

//TODO
//This is a primitive and inefficient wrapper around the existing AliOS memory management.
//A better implementation will have to modify heap_x.c!


typedef struct os_pool_cb {
    void *pool;
    uint8_t *markers;
    uint32_t pool_sz;
    uint32_t item_sz;
    uint32_t currentIndex;
} os_pool_cb_t;


/**
* @brief Create and Initialize a memory pool
* @param  pool_def      memory pool definition referenced with \ref osPool.
* @retval  memory pool ID for reference by other functions or NULL in case of error.
* @note   MUST REMAIN UNCHANGED: \b osPoolCreate shall be consistent in every CMSIS-RTOS.
*/
osPoolId osPoolCreate (const osPoolDef_t *pool_def)
{
    return NULL;
}

/**
* @brief Allocate a memory block from a memory pool
* @param pool_id       memory pool ID obtain referenced with \ref osPoolCreate.
* @retval  address of the allocated memory block or NULL in case of no memory available.
* @note   MUST REMAIN UNCHANGED: \b osPoolAlloc shall be consistent in every CMSIS-RTOS.
*/
void *osPoolAlloc (osPoolId pool_id)
{
    return NULL;
}

/**
* @brief Allocate a memory block from a memory pool and set memory block to zero
* @param  pool_id       memory pool ID obtain referenced with \ref osPoolCreate.
* @retval  address of the allocated memory block or NULL in case of no memory available.
* @note   MUST REMAIN UNCHANGED: \b osPoolCAlloc shall be consistent in every CMSIS-RTOS.
*/
void *osPoolCAlloc (osPoolId pool_id)
{
    return NULL;
}

/**
* @brief Return an allocated memory block back to a specific memory pool
* @param  pool_id       memory pool ID obtain referenced with \ref osPoolCreate.
* @param  block         address of the allocated memory block that is returned to the memory pool.
* @retval  status code that indicates the execution status of the function.
* @note   MUST REMAIN UNCHANGED: \b osPoolFree shall be consistent in every CMSIS-RTOS.
*/
osStatus osPoolFree (osPoolId pool_id, void *block)
{
    return osErrorOS;
}

#endif   /* Use Memory Pool Management */

/*******************   Message Queue Management Functions  *********************/

#if (defined (osFeature_MessageQ)  &&  (osFeature_MessageQ != 0)) /* Use Message Queues */

/**
* @brief Create and Initialize a Message Queue
* @param queue_def     queue definition referenced with \ref osMessageQ.
* @param  thread_id     thread ID (obtained by \ref osThreadCreate or \ref osThreadGetId) or NULL.
* @retval  message queue ID for reference by other functions or NULL in case of error.
* @note   MUST REMAIN UNCHANGED: \b osMessageCreate shall be consistent in every CMSIS-RTOS.
*/
osMessageQId osMessageCreate (const osMessageQDef_t *queue_def, osThreadId thread_id)
{
    (void) thread_id;

    if (RHINO_SUCCESS == krhino_fix_buf_queue_create(queue_def->queue,
                                                     queue_def->name, queue_def->pool,
                                                     queue_def->item_sz, queue_def->queue_sz)) {
        return (osMessageQId)queue_def->queue;
    } else {
        return NULL;
    }
}

/**
* @brief Put a Message to a Queue.
* @param  queue_id  message queue ID obtained with \ref osMessageCreate.
* @param  info      message information.
* @param  millisec  timeout value or 0 in case of no time-out.
* @retval status code that indicates the execution status of the function.
* @note   MUST REMAIN UNCHANGED: \b osMessagePut shall be consistent in every CMSIS-RTOS.
*/
osStatus osMessagePut (osMessageQId queue_id, uint32_t info, uint32_t millisec)
{
    (void)millisec;

    if (RHINO_SUCCESS == krhino_buf_queue_send((kbuf_queue_t *)queue_id, &info, 4)) {
        return osOK;
    } else {
        return osErrorOS;
    }
}

/**
* @brief Get a Message or Wait for a Message from a Queue.
* @param  queue_id  message queue ID obtained with \ref osMessageCreate.
* @param  millisec  timeout value or 0 in case of no time-out.
* @retval event information that includes status code.
* @note   MUST REMAIN UNCHANGED: \b osMessageGet shall be consistent in every CMSIS-RTOS.
*/
osEvent osMessageGet (osMessageQId queue_id, uint32_t millisec)
{
    osEvent event;
    tick_t ticks;
    size_t size;

    if (millisec == 0) {
        ticks = RHINO_NO_WAIT;
    } else if (millisec == osWaitForever) {
        ticks = RHINO_WAIT_FOREVER;
    } else {
        ticks = (millisec * RHINO_CONFIG_TICKS_PER_SECOND) / 1000;
    }

    event.def.message_id = queue_id;
    event.value.v = 0;

    if (queue_id == NULL) {
        event.status = osErrorParameter;
        return event;
    }

    if (RHINO_SUCCESS == krhino_buf_queue_recv((kbuf_queue_t *)queue_id,
                                               ticks, &event.value.v, &size)) {
        event.status = osEventMessage;
    } else {
        event.status = (ticks == 0) ? osOK : osEventTimeout;
    }

    return event;
}

#endif     /* Use Message Queues */

/********************   Mail Queue Management Functions  ***********************/
#if (defined (osFeature_MailQ)  &&  (osFeature_MailQ != 0))  /* Use Mail Queues */

typedef struct os_mailQ_cb {
    const osMailQDef_t *queue_def;
    QueueHandle_t handle;
    osPoolId pool;
} os_mailQ_cb_t;

/**
* @brief Create and Initialize mail queue
* @param  queue_def     reference to the mail queue definition obtain with \ref osMailQ
* @param   thread_id     thread ID (obtained by \ref osThreadCreate or \ref osThreadGetId) or NULL.
* @retval mail queue ID for reference by other functions or NULL in case of error.
* @note   MUST REMAIN UNCHANGED: \b osMailCreate shall be consistent in every CMSIS-RTOS.
*/
osMailQId osMailCreate (const osMailQDef_t *queue_def, osThreadId thread_id)
{
    return NULL;
}

/**
* @brief Allocate a memory block from a mail
* @param  queue_id      mail queue ID obtained with \ref osMailCreate.
* @param  millisec      timeout value or 0 in case of no time-out.
* @retval pointer to memory block that can be filled with mail or NULL in case error.
* @note   MUST REMAIN UNCHANGED: \b osMailAlloc shall be consistent in every CMSIS-RTOS.
*/
void *osMailAlloc (osMailQId queue_id, uint32_t millisec)
{
    return NULL;
}


/**
* @brief Allocate a memory block from a mail and set memory block to zero
* @param  queue_id      mail queue ID obtained with \ref osMailCreate.
* @param  millisec      timeout value or 0 in case of no time-out.
* @retval pointer to memory block that can be filled with mail or NULL in case error.
* @note   MUST REMAIN UNCHANGED: \b osMailCAlloc shall be consistent in every CMSIS-RTOS.
*/
void *osMailCAlloc (osMailQId queue_id, uint32_t millisec)
{
    return NULL;
}


/**
* @brief Put a mail to a queue
* @param  queue_id      mail queue ID obtained with \ref osMailCreate.
* @param  mail          memory block previously allocated with \ref osMailAlloc or \ref osMailCAlloc.
* @retval status code that indicates the execution status of the function.
* @note   MUST REMAIN UNCHANGED: \b osMailPut shall be consistent in every CMSIS-RTOS.
*/
osStatus osMailPut (osMailQId queue_id, void *mail)
{
    return osErrorOS;
}

/**
* @brief Get a mail from a queue
* @param  queue_id   mail queue ID obtained with \ref osMailCreate.
* @param millisec    timeout value or 0 in case of no time-out
* @retval event that contains mail information or error code.
* @note   MUST REMAIN UNCHANGED: \b osMailGet shall be consistent in every CMSIS-RTOS.
*/
osEvent osMailGet (osMailQId queue_id, uint32_t millisec)
{
    osEvent event;

    event.status = osErrorOS;

    return event;
}

/**
* @brief Free a memory block from a mail
* @param  queue_id mail queue ID obtained with \ref osMailCreate.
* @param  mail     pointer to the memory block that was obtained with \ref osMailGet.
* @retval status code that indicates the execution status of the function.
* @note   MUST REMAIN UNCHANGED: \b osMailFree shall be consistent in every CMSIS-RTOS.
*/
osStatus osMailFree (osMailQId queue_id, void *mail)
{
    return osErrorOS;
}
#endif  /* Use Mail Queues */

