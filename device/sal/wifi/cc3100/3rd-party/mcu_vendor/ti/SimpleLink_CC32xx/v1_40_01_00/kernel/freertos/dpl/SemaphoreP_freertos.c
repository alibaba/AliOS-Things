/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
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
 *  ======== SemaphoreP_freertos.c ========
 */

#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/dpl/HwiP.h>

#include <FreeRTOS.h>
#include <semphr.h>
#include <queue.h>

/*
 *  Maximum count for a semaphore.
 */
#define MAXCOUNT 0xffff


/*
 *  ======== SemaphoreP_create ========
 */
SemaphoreP_Handle SemaphoreP_create(unsigned int count,
                                    SemaphoreP_Params *params)
{
    SemaphoreHandle_t sem = NULL;
    SemaphoreP_Params semParams;

    if (params == NULL) {
        params = &semParams;
        SemaphoreP_Params_init(params);
    }

    if (params->mode == SemaphoreP_Mode_COUNTING) {
#if (configUSE_COUNTING_SEMAPHORES == 1)
        /*
         *  The size of the semaphore queue is not dependent on MAXCOUNT.
         *
         *  FreeRTOS xSemaphoreCreateCounting() appears from the
         *  code in xQueueCreateCountingSemaphore() to create
         *  a queue of length maxCount, where maxCount is the
         *  maximum count that the semaphore should ever reach.
         *  However, the queue item size (queueSEMAPHORE_QUEUE_ITEM_LENGTH),
         *  is 0, so no actual memoory is allocated for the queue items.
         *  Therefore we can pass any non-zero number as the maximum
         *  semaphore count.
         */
        sem = xSemaphoreCreateCounting((UBaseType_t)MAXCOUNT,
                (UBaseType_t)count);
#endif
    }
    else {
        sem = xSemaphoreCreateBinary();
        if ((sem != NULL) && (count != 0)) {
            xSemaphoreGive(sem);
        }
    }

    return ((SemaphoreP_Handle)sem);
}

/*
 *  ======== SemaphoreP_createBinary ========
 */
SemaphoreP_Handle SemaphoreP_createBinary(unsigned int count)
{
    SemaphoreHandle_t sem = NULL;

    sem = xSemaphoreCreateBinary();
    if ((sem != NULL) && (count != 0)) {
        xSemaphoreGive(sem);
    }

    return ((SemaphoreP_Handle)sem);
}

/*
 *  ======== SemaphoreP_delete ========
 */
void SemaphoreP_delete(SemaphoreP_Handle handle)
{
    vSemaphoreDelete((SemaphoreHandle_t)handle);
}

/*
 *  ======== SemaphoreP_Params_init ========
 */
void SemaphoreP_Params_init(SemaphoreP_Params *params)
{
    params->mode = SemaphoreP_Mode_COUNTING;
    params->callback = NULL;
}

/*
 *  ======== SemaphoreP_pend ========
 */
SemaphoreP_Status SemaphoreP_pend(SemaphoreP_Handle handle, uint32_t timeout)
{
    BaseType_t status;

    status = xSemaphoreTake((SemaphoreHandle_t)handle, timeout);

    if (status == pdTRUE) {
        return (SemaphoreP_OK);
    }

    return (SemaphoreP_TIMEOUT);
}

/*
 *  ======== SemaphoreP_post ========
 */
void SemaphoreP_post(SemaphoreP_Handle handle)
{
    BaseType_t xHigherPriorityTaskWoken;

    if (!HwiP_inISR()) {
        /* Not in ISR */
        xSemaphoreGive((SemaphoreHandle_t)handle);
    }
    else {
        xSemaphoreGiveFromISR((SemaphoreHandle_t)handle,
                &xHigherPriorityTaskWoken);
    }
}

#if (configSUPPORT_STATIC_ALLOCATION == 1)
/*
 *  ======== SemaphoreP_staticObjectSize ========
 */
size_t SemaphoreP_staticObjectSize(void)
{
    return (sizeof(StaticSemaphore_t));
}
#endif
