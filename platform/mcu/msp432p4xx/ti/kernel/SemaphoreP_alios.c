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
 *  ======== SemaphoreP_alios.c ========
 */

#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/dpl/HwiP.h>

#include "aos/kernel.h"

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
    aos_sem_t sem;
    SemaphoreP_Params semParams;

    if (params == NULL) {
        params = &semParams;
        SemaphoreP_Params_init(params);
    }

    if (0 != aos_sem_new(&sem, count)) {
        return NULL;
    }

    return (SemaphoreP_Handle)sem.hdl;

}

/*
 *  ======== SemaphoreP_createBinary ========
 */
SemaphoreP_Handle SemaphoreP_createBinary(unsigned int count)
{
    aos_sem_t sem;

    if (0 != aos_sem_new(&sem, count)) {
        return NULL;
    }

    return ((SemaphoreP_Handle)sem.hdl);
}

/*
 *  ======== SemaphoreP_delete ========
 */
void SemaphoreP_delete(SemaphoreP_Handle handle)
{
    aos_sem_free((aos_sem_t *)&handle);
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
    int ret = 0;
    
    if (SemaphoreP_WAIT_FOREVER == timeout) {
        ret = aos_sem_wait((aos_sem_t *)&handle, AOS_WAIT_FOREVER);
    } else {
        ret = aos_sem_wait((aos_sem_t *)&handle, timeout);
    }

    if (ret) {
        return SemaphoreP_TIMEOUT;
    }

    return SemaphoreP_OK;
}

/*
 *  ======== SemaphoreP_post ========
 */
void SemaphoreP_post(SemaphoreP_Handle handle)
{
    aos_sem_signal((aos_sem_t *)&handle);
}

