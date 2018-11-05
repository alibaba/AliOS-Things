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
 *  ======== pthread_rwlock_freertos.c ========
 */


#include <stdint.h>

#include <FreeRTOS.h>
#include <portmacro.h>
#include <semphr.h>
#include <task.h>

#include "pthread.h"
#include "time.h"
#include "errno.h"

#define MAXCOUNT 65535

/*
 *  ======== pthread_rwlock_Obj ========
 */
typedef struct pthread_rwlock_Obj {
    /*
     *  This semaphore must be acquired to obtain a write lock.
     *  A readlock can be obtained if there is already a read lock
     *  acquired, or by acquiring this semaphore.
     */
    SemaphoreHandle_t  sem;

    /*
     *  This semaphore is used to block readers when sem is in use
     *  by a write lock.
     */
    SemaphoreHandle_t  readSem;

    int       activeReaderCnt;   /* Number of read locks acquired */
    int       blockedReaderCnt;  /* Number of readers blocked on readSem */

    /*
     *  The 'owner' is the writer holding the lock, or the first reader
     *  that acquired the lock.
     */
    pthread_t owner;
} pthread_rwlock_Obj;

/*
 *  Function for obtaining a timeout in Clock ticks from the difference of
 *  an absolute time and the current time.
 */
extern int _clock_abstime2ticks(clockid_t clockId,
        const struct timespec *abstime, uint32_t *ticks);

static int rdlockAcquire(pthread_rwlock_Obj *lock, uint32_t timeout);

/*
 *************************************************************************
 *                      pthread_rwlockattr
 *************************************************************************
 */
/*
 *  ======== pthread_rwlockattr_destroy ========
 */
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr)
{
    return (0);
}

/*
 *  ======== pthread_rwlockattr_init ========
 */
int pthread_rwlockattr_init(pthread_rwlockattr_t * attr)
{
    return (0);
}


/*
 *************************************************************************
 *                      pthread_rwlock
 *************************************************************************
 */

/*
 *  ======== pthread_rwlock_destroy ========
 */
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock)
{
    pthread_rwlock_Obj  *rwlockObj = *((pthread_rwlock_Obj **)rwlock);

    /* Return EBUSY if the lock is in use. */
    if (rwlockObj->owner != NULL) {
        return (EBUSY);
    }

    vSemaphoreDelete(rwlockObj->sem);
    vSemaphoreDelete(rwlockObj->readSem);

    return (0);
}


/*
 *  ======== pthread_rwlock_init ========
 */
int pthread_rwlock_init(pthread_rwlock_t *rwlock,
        const pthread_rwlockattr_t *attr)
{
    pthread_rwlock_Obj *rwlockObj = NULL;

    *rwlock = NULL;

    rwlockObj = pvPortMalloc(sizeof(pthread_rwlock_Obj));
    if (rwlockObj == NULL) {
        return (ENOMEM);
    }

    /*
     *  FreeRTOS xSemaphoreCreateCounting() creates a queue of
     *  length maxCount, where maxCount is the maximum count that
     *  the semaphore should ever reach.  It looks like this should be
     *  ok since the item length for a semaphore queue is 0, so the
     *  memory allocated is not dependent on maxCount.
     */
    /* Create semaphore with count of 1 */
    rwlockObj->sem = xSemaphoreCreateCounting(MAXCOUNT, 1);
    if (rwlockObj->sem == NULL) {
        vPortFree(rwlockObj);
        return (ENOMEM);
    }

    /* Create semaphore with count of 0 */
    rwlockObj->readSem = xSemaphoreCreateCounting(MAXCOUNT, 0);
    if (rwlockObj->readSem == NULL) {
        vPortFree(rwlockObj);
        return (ENOMEM);
    }

    rwlockObj->activeReaderCnt = 0;
    rwlockObj->blockedReaderCnt = 0;

    rwlockObj->owner = NULL;

    *rwlock = (pthread_rwlock_t)rwlockObj;

    return (0);
}


/*
 *  ======== pthread_rwlock_rdlock ========
 */
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock)
{
    pthread_rwlock_Obj  *rwlockObj = *((pthread_rwlock_Obj **)rwlock);

    return (rdlockAcquire(rwlockObj, portMAX_DELAY));
}

/*
 *  ======== pthread_rwlock_timedrdlock ========
 */
int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock,
        const struct timespec *abstime)
{
    pthread_rwlock_Obj  *rwlockObj = *((pthread_rwlock_Obj **)rwlock);
    uint32_t           timeout;

    if (_clock_abstime2ticks(CLOCK_MONOTONIC, abstime, &timeout) != 0) {
        return (EINVAL);
    }

    return (rdlockAcquire(rwlockObj, timeout));
}

/*
 *  ======== pthread_rwlock_timedwrlock ========
 */
int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock,
        const struct timespec *abstime)
{
    pthread_rwlock_Obj  *rwlockObj = *((pthread_rwlock_Obj **)rwlock);
    uint32_t           timeout;

    if (_clock_abstime2ticks(CLOCK_MONOTONIC, abstime, &timeout) != 0) {
        return (EINVAL);
    }

    if (xSemaphoreTake(rwlockObj->sem, (TickType_t)timeout) == pdTRUE) {
        rwlockObj->owner = pthread_self();
    }

    return (ETIMEDOUT);
}

/*
 *  ======== pthread_rwlock_tryrdlock ========
 */
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock)
{
    pthread_rwlock_Obj  *rwlockObj = *((pthread_rwlock_Obj **)rwlock);

    return (rdlockAcquire(rwlockObj, 0));
}

/*
 *  ======== pthread_rwlock_trywrlock ========
 */
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock)
{
    pthread_rwlock_Obj  *rwlockObj = *((pthread_rwlock_Obj **)rwlock);

    if (xSemaphoreTake(rwlockObj->sem, 0) == pdTRUE) {
        rwlockObj->owner = pthread_self();

        return (0);
    }

    return (EBUSY);
}

/*
 *  ======== pthread_rwlock_unlock ========
 */
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock)
{
    pthread_rwlock_Obj  *rwlockObj = *((pthread_rwlock_Obj **)rwlock);
    int                  i;

    /* Disable the scheduler */
    vTaskSuspendAll();

    if (rwlockObj->activeReaderCnt) {
        /*
         *  Lock is held by a reader.  The last active reader
         *  releases the semaphore.
         */
        if (--rwlockObj->activeReaderCnt == 0) {
            rwlockObj->owner = NULL;
            xSemaphoreGive(rwlockObj->sem);
        }
    }
    else {
        /*
         *  Lock is held by a writer.  Release the semaphore and
         *  if there are any blocked readers, unblock all of them.
         *  By unblocking all readers, we ensure that the highest
         *  priority reader is unblocked.
         */
        xSemaphoreGive(rwlockObj->sem);

        /* Unblock all readers */
        for (i = 0; i < rwlockObj->blockedReaderCnt; i++) {
            xSemaphoreGive(rwlockObj->readSem);
        }

        rwlockObj->owner = NULL;
    }

    /* Re-enable the scheduler */
    xTaskResumeAll();

    return (0);
}

/*
 *  ======== pthread_rwlock_wrlock ========
 */
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock)
{
    pthread_rwlock_Obj  *rwlockObj = *((pthread_rwlock_Obj **)rwlock);

    xSemaphoreTake(rwlockObj->sem, portMAX_DELAY);

    rwlockObj->owner = pthread_self();

    return (0);
}


/*
 *************************************************************************
 *                      internal functions
 *************************************************************************
 */

/*
 *  ======== rdlockAcquire ========
 */
static int rdlockAcquire(pthread_rwlock_Obj *rwlock, uint32_t timeout)
{
    TickType_t  curTicks;
    TickType_t  prevTicks;
    TickType_t  deltaTicks;

    /* Disable the scheduler */
    vTaskSuspendAll();

    if (rwlock->activeReaderCnt > 0) {
        /* The semaphore is owned by a reader, no need to pend. */
        rwlock->activeReaderCnt++;
        xTaskResumeAll();

        return (0);
    }

    if (xSemaphoreTake(rwlock->sem, 0) == pdTRUE) {
        /* Got the semaphore */
        rwlock->activeReaderCnt++;
        rwlock->owner = pthread_self();

        /*
         *  Either there are no blocked readers, or a writer has just
         *  unlocked rwlock->sem and posted all the blocked readers.
         */
        xTaskResumeAll();

        return (0);
    }

    if (timeout == 0) {
        xTaskResumeAll();
        return (EBUSY);
    }

    rwlock->blockedReaderCnt++;
    curTicks = xTaskGetTickCount();

    xTaskResumeAll();

    for (;;) {
        if (xSemaphoreTake(rwlock->readSem, (TickType_t)timeout) != pdTRUE) {
            vTaskSuspendAll();
            rwlock->blockedReaderCnt--;
            xTaskResumeAll();

            return (ETIMEDOUT);
        }

        vTaskSuspendAll();

        /*
         *  If another reader is active, the rwlock->sem is owned
         *  by a reader, so just increment the active reader count.
         */
        if (rwlock->activeReaderCnt > 0) {
            rwlock->blockedReaderCnt--;
            rwlock->activeReaderCnt++;

            xTaskResumeAll();

            return (0);
        }

        /*
         *  We have been unblocked by a writer.  Try again to take the
         *  rwlock->sem, since another writer or reader may have taken
         *  it in the meantime.
         */
        if (xSemaphoreTake(rwlock->sem, 0) == pdTRUE) {
            /* Got it */
            rwlock->blockedReaderCnt--;
            rwlock->activeReaderCnt++;

            rwlock->owner = pthread_self();

            xTaskResumeAll();

            return (0);
        }

        if (timeout != portMAX_DELAY) {
            prevTicks = curTicks;
            curTicks = xTaskGetTickCount();
            deltaTicks = curTicks - prevTicks;

            if (deltaTicks >= timeout) {
                /* Timed out without acquiring the lock */
                rwlock->blockedReaderCnt--;

                xTaskResumeAll();

                break;
            }
            timeout -= deltaTicks;
        }

        xTaskResumeAll();
    }

    return (ETIMEDOUT);
}
