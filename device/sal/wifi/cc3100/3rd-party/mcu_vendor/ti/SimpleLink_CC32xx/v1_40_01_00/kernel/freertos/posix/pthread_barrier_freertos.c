/*
 * Copyright (c) 2016, Texas Instruments Incorporated
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
 *  ======== pthread_barrier_freertos.c ========
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <FreeRTOS.h>
#include <portmacro.h>
#include <task.h>
#include <semphr.h>

#include "pthread.h"
#include "errno.h"


/*
 *  ======== pthread_barrier_t ========
 */
typedef struct pthread_barrier_Obj {
    SemaphoreHandle_t sem;
    /*
     *  Would like to use a static semaphore, but this requires we requires
     *  the application implement these hook functions:
     *
     *      vApplicationGetIdleTaskMemory()
     *      vApplicationGetTimerTaskMemory()
     */
    //StaticSemaphore_t semBuffer;
    int               count;
    int               pendCount;
} pthread_barrier_Obj;


/*
 *************************************************************************
 *                      pthread_barrierattr
 *************************************************************************
 */
/*
 *  ======== pthread_barrierattr_destroy ========
 */
int pthread_barrierattr_destroy(pthread_barrierattr_t *attr)
{
    return (0);
}

/*
 *  ======== pthread_barrierattr_init ========
 */
int pthread_barrierattr_init(pthread_barrierattr_t *attr)
{
    return (0);
}

/*
 *************************************************************************
 *                      pthread_barrier
 *************************************************************************
 */
int pthread_barrier_destroy(pthread_barrier_t *barrier)
{
    pthread_barrier_Obj *barrierObj = *((pthread_barrier_Obj **)barrier);

    if (barrierObj->sem) {
        vSemaphoreDelete(barrierObj->sem);
    }

    vPortFree(barrierObj);

    return (0);
}

/*
 *  ======== pthread_barrier_init ========
 */
int pthread_barrier_init(pthread_barrier_t *barrier,
        const pthread_barrierattr_t *attr, unsigned count)
{
    pthread_barrier_Obj *barrierObj;

    *barrier = NULL;

    barrierObj = pvPortMalloc(sizeof(pthread_barrier_Obj));
    if (barrierObj == NULL) {
        return (ENOMEM);
    }

    /*
     *  Create semaphore with a maximum count of 'count' and an initial
     *  count of 0.
     */
//    barrierObj->sem = xSemaphoreCreateCountingStatic(count, 0,
//            barrierObj->semBuffer);
    barrierObj->sem = xSemaphoreCreateCounting(count, 0);
    if (barrierObj->sem == NULL) {
        vPortFree(barrierObj);
        return (ENOMEM);
    }

    barrierObj->count = count;
    barrierObj->pendCount = 0;

    *barrier = (pthread_barrier_t)barrierObj;

    return (0);
}

/*
 *  ======== pthread_barrier_wait ========
 */
int pthread_barrier_wait(pthread_barrier_t *barrier)
{
    pthread_barrier_Obj *barrierObj = *((pthread_barrier_Obj **)barrier);
    int                  i;

    /* Disable the scheduler */
    vTaskSuspendAll();

    if (++barrierObj->pendCount < barrierObj->count) {
        /* Re-enable the scheduler */
        xTaskResumeAll();

        xSemaphoreTake(barrierObj->sem, portMAX_DELAY);
        return (0);
    }
    else {
        for (i = 0; i < barrierObj->count - 1; i++) {
            xSemaphoreGive(barrierObj->sem);
        }

        /* Re-initialize the barrier */
        barrierObj->pendCount = 0;

        /* Re-enable the scheduler */
        xTaskResumeAll();

        /*
         *  pthread_barrier_wait() returns PTHREAD_BARRIER_SERIAL_THREAD
         *  for one arbitrarily chosen thread, so we'll choose the
         *  last one to wait.  The return value for all other threads
         *  is 0.
         */
        return (PTHREAD_BARRIER_SERIAL_THREAD);
    }
}
