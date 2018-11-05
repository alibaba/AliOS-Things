/*
 * Copyright (c) 2016-2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  ======== pthread_mutex_freertos.c ========
 *
 *  The following APIs are not supported:
 *
 *      pthread_mutexattr_getprioceiling()
 *      pthread_mutexattr_setprioceiling()
 *      pthread_mutex_getprioceiling()
 *      pthread_mutex_setprioceiling()
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* FreeRTOS header files */
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <portmacro.h>

#include "pthread.h"
#include "sys/types.h"
#include "time.h"
#include "errno.h"

/*
 *  ======== pthread_mutex_Obj ========
 */
typedef struct pthread_mutex_Obj {
    int                protocol;
    pthread_t          owner;
    int                type;
    SemaphoreHandle_t  sem;
} pthread_mutex_Obj;

/*
 *  Function for obtaining a timeout in Clock ticks from the difference of
 *  an absolute time and the current time.
 */
extern int _clock_abstime2ticks(clockid_t clockId,
        const struct timespec *abstime, uint32_t *ticks);

static int acquireMutex(pthread_mutex_Obj *mutex, uint32_t timeout);

/*
 *  Default mutex attrs.
 */
static pthread_mutexattr_t defAttrs = {
    PTHREAD_MUTEX_DEFAULT, /* type */
    PTHREAD_PRIO_NONE,     /* protocol */
};

/*
 *************************************************************************
 *                      pthread_mutexattr
 *************************************************************************
 */
/*
 *  ======== pthread_mutexattr_destroy ========
 */
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr)
{
    return (0);
}

/*
 *  ======== pthread_mutexattr_gettype ========
 */
int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr,
        int *type)
{
    *type = attr->type;
    return (0);
}

/*
 *  ======== pthread_mutexattr_getprotocol ========
 */
int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *attr,
        int *protocol)
{
    *protocol = attr->protocol;
    return (0);
}

/*
 *  ======== pthread_mutexattr_init ========
 */
int pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
    *attr = defAttrs;
    return (0);
}

/*
 *  ======== pthread_mutexattr_setprotocol ========
 */
int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol)
{
    if ((protocol != PTHREAD_PRIO_NONE) &&
            (protocol != PTHREAD_PRIO_INHERIT) &&
            (protocol != PTHREAD_PRIO_PROTECT)) {
        return (EINVAL);
    }

    attr->protocol = protocol;
    return (0);
}

/*
 *  ======== pthread_mutexattr_settype ========
 */
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type)
{
    int     retc = EINVAL;

    if ((type == PTHREAD_MUTEX_NORMAL) ||
#if configUSE_RECURSIVE_MUTEXES == 1
            (type == PTHREAD_MUTEX_RECURSIVE) ||
#endif
            (type == PTHREAD_MUTEX_ERRORCHECK)) {
        attr->type = type;
        retc = 0;
    }

    return (retc);
}

/*
 *************************************************************************
 *                      pthread_mutex
 *************************************************************************
 */


/*
 *  ======== pthread_mutex_destroy ========
 */
int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    pthread_mutex_Obj *mutexObj = (pthread_mutex_Obj *)*mutex;

    if (mutexObj->sem != NULL) {
        vSemaphoreDelete(mutexObj->sem);
    }

    vPortFree((void *)(*mutex));

    *mutex = NULL;

    return (0);
}

/*
 *  ======== pthread_mutex_init ========
 */
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
    pthread_mutex_Obj   *mutexObj;
    pthread_mutexattr_t *mAttrs;
    SemaphoreHandle_t    sem;
    *mutex = NULL;

    mutexObj = pvPortMalloc(sizeof(pthread_mutex_Obj));

    if (mutexObj == NULL) {
        return (ENOMEM);
    }

    mAttrs = (attr == NULL) ? &defAttrs : (pthread_mutexattr_t *)attr;

    mutexObj->type = mAttrs->type;
    mutexObj->sem = NULL;
    mutexObj->owner = NULL;

    mutexObj->protocol = mAttrs->protocol;

    if ((mutexObj->type == PTHREAD_MUTEX_NORMAL) ||
            (mutexObj->type == PTHREAD_MUTEX_ERRORCHECK)) {
        sem = xSemaphoreCreateMutex();
    }
#if configUSE_RECURSIVE_MUTEXES == 1
    else {
        /* PTHREAD_MUTEX_RECURSIVE */
        sem = xSemaphoreCreateRecursiveMutex();
    }
#endif

    if (sem == NULL) {
        pthread_mutex_destroy((pthread_mutex_t *)&mutexObj);
        return (ENOMEM);
    }

    mutexObj->sem = sem;

    *mutex = (pthread_mutex_t)mutexObj;

    return (0);
}

/*
 *  ======== pthread_mutex_lock ========
 *
 *  Retuns:
 *    0      - Lock was successfully acquired.
 *    EDEADLK - Mutex type is PTHREAD_MUTEX_ERRORCHECK and calling thread
 *              already owns the mutex.
 */
int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    pthread_mutex_Obj *mutexObj = (pthread_mutex_Obj *)*mutex;
    uint32_t           timeout = portMAX_DELAY;
    int                retc;

    retc = acquireMutex(mutexObj, timeout);

    return (retc);
}

/*
 *  ======== pthread_mutex_timedlock ========
 *
 *  Retuns:
 *    0      - Lock was successfully acquired.
 *    ETIMEDOUT - timeout expired before mutex could be acquired
 *    EDEADLK - Mutex type is PTHREAD_MUTEX_ERRORCHECK and calling thread
 *              already owns the mutex.
 */
int pthread_mutex_timedlock(pthread_mutex_t *mutex,
        const struct timespec *abstime)
{
    uint32_t           timeout;
    int                retc;


    /* Don't bother checking the time if the mutex is available */
    if (acquireMutex((pthread_mutex_Obj *)*mutex, 0) == 0) {
        return (0);
    }

    if (_clock_abstime2ticks(CLOCK_MONOTONIC, abstime, &timeout) != 0) {
        return (EINVAL);
    }

    retc = acquireMutex((pthread_mutex_Obj *)*mutex, timeout);
    retc = (retc == EBUSY) ? ETIMEDOUT : retc;

    return (retc);
}

/*
 *  ======== pthread_mutex_trylock ========
 *  Retuns:
 *    0      - Lock was successfully acquired.
 *    EBUSY  - The mutex is already locked and mutex is owned by another
 *             thread or not recursive.
 */
int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
    pthread_mutex_Obj *mutexObj = (pthread_mutex_Obj *)*mutex;
    int        retc = 0;

    /* Attempt to acquire the mutex with timeout of 0 */
    retc = acquireMutex(mutexObj, 0);

    retc = (retc == ETIMEDOUT) ? EBUSY : retc;

    return (retc);
}

/*
 *  ======== pthread_mutex_unlock ========
 *  Retuns:
 *    0      - The mutex was successfully released.
 *    EPERM  - Calling thread does not own the mutex.
 */
int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    pthread_mutex_Obj *mutexObj = (pthread_mutex_Obj *)*mutex;
    BaseType_t         status;
    int                retc = 0;

    /*
     *  Don't bother checking whether or not this thread owns the
     *  mutex:
     *
     *      if (mutexObj->owner != pthread_self()) {
     *          return (EPERM);
     *      }
     *
     *  Instead, we'll rely on FreeRTOS returning an error.
     */

    if (mutexObj->type == PTHREAD_MUTEX_RECURSIVE) {
        status = xSemaphoreGiveRecursive(mutexObj->sem);
    }
    else {
        /* Disable the scheduler */
        vTaskSuspendAll();

        status = xSemaphoreGive(mutexObj->sem);
        mutexObj->owner = NULL;

        /* Re-enable the scheduler */
        xTaskResumeAll();
    }

    if (status != pdTRUE) {
        /*
         *  This is an internal error.  Semaphores are implemented using
         *  queues, and an error occurs if there is no space on the queue
         *  to post a message, indicating that the semaphore was not
         *  obtained correctly.
         */
        retc = EFREERTOS;
    }
    return (retc);
}


/*
 *************************************************************************
 *                      Internal functions
 *************************************************************************
 */

/*
 *  ======== acquireMutex ========
 */
static int acquireMutex(pthread_mutex_Obj *mutex, uint32_t timeout)
{
    pthread_t       *thisThread;
    BaseType_t       status;
    int              retc = 0;

    thisThread = pthread_self();

    /* Return if the thread already owns the mutex */
    if (mutex->owner == thisThread) {
        if (mutex->type == PTHREAD_MUTEX_ERRORCHECK) {
            return (EDEADLK);
        }
    }

    if (mutex->type == PTHREAD_MUTEX_RECURSIVE) {
        status = xSemaphoreTakeRecursive(mutex->sem, (TickType_t)timeout);
    }
    else {
        status = xSemaphoreTake(mutex->sem, (TickType_t)timeout);
    }

    if (status != pdTRUE) {
        retc = ETIMEDOUT;
    }
    else {
        mutex->owner = thisThread;
    }

    return (retc);
}
