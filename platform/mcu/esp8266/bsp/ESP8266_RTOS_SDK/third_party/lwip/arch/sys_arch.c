/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/* lwIP includes. */

#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "arch/sys_arch.h"


/* Message queue constants. */
#define archMESG_QUEUE_LENGTH	(100)//( 6 )
#define archPOST_BLOCK_TIME_MS	( ( unsigned portLONG ) 10000 )


struct timeoutlist {
    //struct sys_timeouts timeouts;
    espos_task_t pid;
};

/* This is the number of threads that can be started with sys_thread_new() */
#define SYS_THREAD_MAX 4

//static struct timeoutlist timeoutlist[SYS_THREAD_MAX];
//static u16_t nextthread = 0;
int intlevel = 0;

/*-----------------------------------------------------------------------------------*/
// Initialize sys arch
void
sys_init(void)
{
}

/*-----------------------------------------------------------------------------------*/
//  Creates and returns a new semaphore. The "count" argument specifies
//  the initial state of the semaphore. TBD finish and test
err_t
sys_sem_new(sys_sem_t *sem, u8_t count)
{
    int ret = espos_sem_create(sem, 1, count);

    return ret == 0 ? ERR_OK : ERR_MEM;
}


/*-----------------------------------------------------------------------------------*/
// Deallocates a semaphore
void
sys_sem_free(sys_sem_t *sem)
{
    //vQueueDelete( sem );
    espos_sem_del(*sem);
}


/*-----------------------------------------------------------------------------------*/
// Signals a semaphore
void
sys_sem_signal(sys_sem_t *sem)
{
    //xSemaphoreGive(*sem);
    espos_sem_give(*sem);
}


/*-----------------------------------------------------------------------------------*/
/*
  Blocks the thread while waiting for the semaphore to be
  signaled. If the "timeout" argument is non-zero, the thread should
  only be blocked for the specified time (measured in
  milliseconds).

  If the timeout argument is non-zero, the return value is the number of
  milliseconds spent waiting for the semaphore to be signaled. If the
  semaphore wasn't signaled within the specified time, the return value is
  SYS_ARCH_TIMEOUT. If the thread didn't have to wait for the semaphore
  (i.e., it was already signaled), the function may return zero.

  Notice that lwIP implements a function with a similar name,
  sys_sem_wait(), that uses the sys_arch_sem_wait() function.
*/
u32_t
sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
    espos_tick_t StartTime, EndTime, Elapsed;
    unsigned long ulReturn;

    StartTime = espos_get_tick_count();

    if (timeout != 0) {
        if (espos_sem_take(*sem, espos_ms_to_ticks(timeout)) == 0) {
            EndTime = espos_get_tick_count();
            Elapsed = espos_ticks_to_ms(EndTime - StartTime);

            if (Elapsed == 0) {
                Elapsed = 1;
            }

            ulReturn = Elapsed;
        } else {
            ulReturn = SYS_ARCH_TIMEOUT;
        }
    } else { // must block without a timeout
        while (espos_sem_take(*sem, ESPOS_MAX_DELAY) != 0);

        EndTime = espos_get_tick_count();
        Elapsed = espos_ticks_to_ms(EndTime - StartTime);

        if (Elapsed == 0) {
            Elapsed = 1;
        }

        ulReturn = Elapsed;
    }

    return ulReturn ; // return time blocked
}

/*-----------------------------------------------------------------------------------*/
//  Creates an empty mailbox.
err_t
sys_mbox_new(sys_mbox_t *mbox, int size)
{
    err_t xReturn = ERR_MEM;

    // *mbox = xQueueCreate(size, sizeof(void *));
    int ret = espos_queue_create(mbox, sizeof(void *), size);

    if (!ret) {
        xReturn = ERR_OK;
    }

    return xReturn;
}

/*-----------------------------------------------------------------------------------*/
/*
  Deallocates a mailbox. If there are messages still present in the
  mailbox when the mailbox is deallocated, it is an indication of a
  programming error in lwIP and the developer should be notified.
*/
void
sys_mbox_free(sys_mbox_t *mbox)
{
//     if (uxQueueMessagesWaiting(*mbox)) {
//         /* Line for breakpoint.  Should never break here! */
// //		__asm volatile ( "NOP" );
//     }

    //vQueueDelete(*mbox);
    espos_queue_del(*mbox);
}

/*-----------------------------------------------------------------------------------*/
//   Posts the "msg" to the mailbox.
void
sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
    //while (xQueueSendToBack(*mbox, &msg, portMAX_DELAY) != pdTRUE);
    while (espos_queue_send(*mbox, &msg, ESPOS_MAX_DELAY) != 0);
}

err_t
sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
    err_t xReturn;

    if (espos_queue_send(*mbox, &msg, ESPOS_NO_DELAY) == 0) {
        xReturn = ERR_OK;
    } else {
        xReturn = ERR_MEM;
    }

    return xReturn;
}

/*-----------------------------------------------------------------------------------*/
/*
  Blocks the thread until a message arrives in the mailbox, but does
  not block the thread longer than "timeout" milliseconds (similar to
  the sys_arch_sem_wait() function). The "msg" argument is a result
  parameter that is set by the function (i.e., by doing "*msg =
  ptr"). The "msg" parameter maybe NULL to indicate that the message
  should be dropped.

  The return values are the same as for the sys_arch_sem_wait() function:
  Number of milliseconds spent waiting or SYS_ARCH_TIMEOUT if there was a
  timeout.

  Note that a function with a similar name, sys_mbox_fetch(), is
  implemented by lwIP.
*/
u32_t
sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
    void *dummyptr;
    espos_tick_t StartTime, EndTime, Elapsed;
    unsigned long ulReturn;

    StartTime = espos_get_tick_count();

    if (msg == NULL) {
        msg = &dummyptr;
    }

    if (timeout != 0) {
        if (0 == espos_queue_recv(*mbox, &(*msg), espos_ms_to_ticks(timeout))) {
            EndTime = espos_get_tick_count();
            Elapsed = espos_ticks_to_ms(EndTime - StartTime);

            if (Elapsed == 0) {
                Elapsed = 1;
            }

            ulReturn = Elapsed;
        } else { // timed out blocking for message
            *msg = NULL;
            ulReturn = SYS_ARCH_TIMEOUT;
        }
    } else { // block forever for a message.
        while (0 != espos_queue_recv(*mbox, &(*msg), ESPOS_MAX_DELAY));

        EndTime = espos_get_tick_count();
        Elapsed = espos_ticks_to_ms(EndTime - StartTime);

        if (Elapsed == 0) {
            Elapsed = 1;
        }

        ulReturn = Elapsed;
    }

    return ulReturn ; // return time blocked TBD test
}

u32_t
sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
    void *pvDummy;
    unsigned long ulReturn;

    if (msg == NULL) {
        msg = &pvDummy;
    }

    if (0 == espos_queue_recv(*mbox, &(*msg), 0)) {
        ulReturn = ERR_OK;
    } else {
        ulReturn = SYS_MBOX_EMPTY;
    }

    return ulReturn;
}

/** Create a new mutex
 * @param mutex pointer to the mutex to create
 * @return a new mutex */
err_t
sys_mutex_new(sys_mutex_t *pxMutex)
{
    err_t xReturn = ERR_MEM;

    int ret = espos_mutex_create(pxMutex, ESPOS_MUTEX_NORMAL);

    if (!ret) {
        xReturn = ERR_OK;

    } else {
        ;
    }

    return xReturn;
}

/** Lock a mutex
 * @param mutex the mutex to lock */
void
sys_mutex_lock(sys_mutex_t *pxMutex)
{
    while (espos_mutex_lock(*pxMutex, ESPOS_MAX_DELAY) != 0);
}

err_t
sys_mutex_trylock(sys_mutex_t *pxMutex)
{
	if (espos_mutex_lock(*pxMutex, 0) == 0) return 0;
	else return -1;
}

/** Unlock a mutex
 * @param mutex the mutex to unlock */
void
sys_mutex_unlock(sys_mutex_t *pxMutex)
{
    espos_mutex_unlock(*pxMutex);
}


/** Delete a semaphore
 * @param mutex the mutex to delete */
void
sys_mutex_free(sys_mutex_t *pxMutex)
{
    espos_mutex_del(*pxMutex);
}

u32_t
sys_now(void)
{
    return espos_get_tick_count();
}

/*-----------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
// TBD
/*-----------------------------------------------------------------------------------*/
/*
  Starts a new thread with priority "prio" that will begin its execution in the
  function "thread()". The "arg" argument will be passed as an argument to the
  thread() function. The id of the new thread is returned. Both the id and
  the priority are system dependent.
*/
sys_thread_t
sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
    espos_task_t CreatedTask;
    int result;

    //result = xTaskCreate(thread, (signed char *)name, stacksize, arg, prio, &CreatedTask);
    result = espos_task_create(&CreatedTask, name, arg, prio, 1, stacksize, thread, ESPOS_TASK_CREATE_NORMAL);

    if (result == 0) {
        return CreatedTask;
    } else {
        return ESPOS_OBJ_NONE;
    }
}

/*
  This optional function does a "fast" critical region protection and returns
  the previous protection level. This function is only called during very short
  critical regions. An embedded system which supports ISR-based drivers might
  want to implement this function by disabling interrupts. Task-based systems
  might want to implement this by using a mutex or disabling tasking. This
  function should support recursive calls from the same task or interrupt. In
  other words, sys_arch_protect() could be called while already protected. In
  that case the return value indicates that it is already protected.

  sys_arch_protect() is only required if your port is supporting an operating
  system.
*/
sys_prot_t
sys_arch_protect(void)
{
    _espos_enter_critical(NULL);
    return (sys_prot_t) 1;
}

/*
  This optional function does a "fast" set of critical region protection to the
  value specified by pval. See the documentation for sys_arch_protect() for
  more information. This function is only required if your port is supporting
  an operating system.
*/
void
sys_arch_unprotect(sys_prot_t pval)
{
    (void) pval;
    _espos_exit_critical(NULL, 0);
}

/*
 * Prints an assertion messages and aborts execution.
 */
void
sys_arch_assert(const char *file, int line)
{
    os_printf("\nAssertion: %d in %s\n", line, file);

    //while(1);
}

void
sys_arch_msleep(int ms)
{
	espos_task_delay(espos_ms_to_ticks(ms));
}

