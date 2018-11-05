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
 *  ======== semaphore_freertos.c ========
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* FreeRTOS header files */
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <portmacro.h>

#include <ti/drivers/dpl/HwiP.h>

#include "semaphore.h"
#include "time.h"
#include "errno.h"

/*
 *  Must have configUSE_COUNTING_SEMAPHORES == 1 in FreeRTOSConfig.h
 *
 *  Note:  Posix semaphore does not support binary mode.  If you want a
 *  binary semaphore, you can implement it with a condition variable and
 *  a mutex.
 */

/*
 *  Function for obtaining a timeout in Clock ticks from the difference of
 *  an absolute time and the current time.
 */
extern int _clock_abstime2ticks(clockid_t clockId,
        const struct timespec *abstime, uint32_t *ticks);

static int semaphorePend(SemaphoreHandle_t sem, TickType_t ticks);

/*
 *  ======== sem_destroy ========
 */
int sem_destroy(sem_t *semaphore)
{
    SemaphoreHandle_t sem = *((SemaphoreHandle_t *)semaphore);

    vSemaphoreDelete(sem);

    return (0);
}

/*
 *  ======== sem_getvalue ========
 */
int sem_getvalue(sem_t *semaphore, int *value)
{
    SemaphoreHandle_t sem = *((SemaphoreHandle_t *)semaphore);
    UBaseType_t count = 0;

    /* Not available until FreeRTOS version 9 */
    count = uxSemaphoreGetCount(sem);

    *value = (int)count;
    return (0);
}

/*
 *  ======== sem_init ========
 */
int sem_init(sem_t *semaphore, int pshared, unsigned value)
{
    SemaphoreHandle_t sem = NULL;

    if (value > SEM_VALUE_MAX) {
        /* Value too large */
        /* errno = EINVAL */
        return (-1);
    }

    /*
     *  FreeRTOS xSemaphoreCreateCounting() creates a queue of
     *  length maxCount, where maxCount is the maximum count that
     *  the semaphore should ever reach.  It looks like this should be
     *  ok since the item length for a semaphore queue is 0, so the
     *  memory allocated is not dependent on maxCount.
     */
    sem = xSemaphoreCreateCounting((UBaseType_t)SEM_VALUE_MAX,
            (UBaseType_t)value);

    *((SemaphoreHandle_t *)semaphore) = sem;

    if (semaphore == NULL) {
        /* errno = ENOMEM */
        return (-1);
    }

    return (0);
}

/*
 *  ======== sem_post ========
 */
int sem_post(sem_t *semaphore)
{
    SemaphoreHandle_t sem = *((SemaphoreHandle_t *)semaphore);
    BaseType_t        xHigherPriorityTaskWoken;
    BaseType_t        result;

    if (HwiP_inISR()) {
        result = xSemaphoreGiveFromISR(sem, &xHigherPriorityTaskWoken);

        if (result != pdTRUE) {
            /* The queue is full */
            return (-1);
        }
    }
    else {
        xSemaphoreGive(sem);
    }

    return (0);
}

/*
 *  ======== sem_timedwait ========
 */
int sem_timedwait(sem_t *semaphore, const struct timespec *abstime)
{
    SemaphoreHandle_t sem = *((SemaphoreHandle_t *)semaphore);
    int                retc = 0;
    uint32_t           timeout;

    /* Don't bother checking the time if the semaphore is available */
    if (semaphorePend(sem, 0) == 0) {
        return (0);
    }

    if (_clock_abstime2ticks(CLOCK_MONOTONIC, abstime, &timeout) != 0) {
        /* errno = EINVAL */
        return (-1);
    }

    retc = semaphorePend(sem, (TickType_t)timeout);

    return (retc);
}

/*
 *  ======== sem_trywait ========
 */
int sem_trywait(sem_t *semaphore)
{
    SemaphoreHandle_t sem = *((SemaphoreHandle_t *)semaphore);
    int        retc;

    retc = semaphorePend(sem, 0);
    if (retc != 0) {
        /* errno = EAGAIN */
    }

    return (retc);
}

/*
 *  ======== sem_wait ========
 */
int sem_wait(sem_t *semaphore)
{
    SemaphoreHandle_t sem = *((SemaphoreHandle_t *)semaphore);

    semaphorePend(sem, portMAX_DELAY);

    return (0);
}

/*
 *************************************************************************
 *                      Internal functions
 *************************************************************************
 */

/*
 *  ======== semaphorePend ========
 */
static int semaphorePend(SemaphoreHandle_t sem, TickType_t ticks)
{
    BaseType_t       status;
    int              retc = 0;

    status = xSemaphoreTake(sem, ticks);
    if (status != pdTRUE) {
        /* errno = ETIMEDOUT */
        retc = -1;
    }

    return (retc);
}
