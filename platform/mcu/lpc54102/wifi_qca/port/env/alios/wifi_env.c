/*
 * The Clear BSD License
 * Copyright (c) 2016, NXP Semiconductor, Inc.
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used tom  endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "wifi_common.h"

/*TODO: check return value */
void a_free(void *addr, uint8_t id)
{
    // UNUSED_ARGUMENT(id);
    krhino_mm_free(addr);
}

/* FIXME !! */
extern uint32_t g_totAlloc;
void *a_malloc(int32_t size, uint8_t id)
{
    void *addr;

    addr = (void *)krhino_mm_alloc(size);
    assert(addr);
    if (addr != NULL)
    {
        /*FIXME: !!!*/
//        PRINTF("SIZE = %d\r\n", size);
        g_totAlloc += size;
    }
    // UNUSED_ARGUMENT(id);

    return addr;
}

A_STATUS a_mutex_init(A_MUTEX_T *pMutex)
{
    assert(pMutex);

    if (krhino_mutex_create(pMutex, "mutex") == RHINO_SUCCESS)
    {
        return A_OK;
    }
    else
    {
        return A_ERROR;
    }
}

A_STATUS a_mutex_acquire(A_MUTEX_T *pMutex)
{
    assert(pMutex);

    if (krhino_mutex_lock(pMutex, RHINO_WAIT_FOREVER) == RHINO_SUCCESS)
    {
        return A_OK;
    }
    else
    {
        return A_ERROR;
    }
}

A_STATUS a_mutex_release(A_MUTEX_T *pMutex)
{
    assert(pMutex);

    if (krhino_mutex_unlock(pMutex) == RHINO_SUCCESS)
    {
        return A_OK;
    }
    else
    {
        assert(0); // TODO: use assert cause no-one check return value
        return A_ERROR;
    }
}

boolean a_is_mutex_valid(A_MUTEX_T *pMutex)
{
    // FIXME: check owner of mutex
    return aos_mutex_is_valid(pMutex);
}

A_STATUS a_mutex_delete(A_MUTEX_T *pMutex)
{
    assert(pMutex);

    krhino_mutex_del(pMutex);
    return A_OK;
}

A_STATUS a_event_delete(A_EVENT *pEvent)
{
    assert(pEvent);
    krhino_event_del(&pEvent->event);
    return A_OK;
}

// zrusit autoclear flag !!!
A_STATUS a_event_init(A_EVENT *pEvent, osa_event_clear_mode_t clearMode)
{
    assert(pEvent);

    krhino_event_create(&pEvent->event, "event", 0);

    pEvent->clearMode = clearMode;
    return A_OK;
}

A_STATUS a_event_clear(A_EVENT *pEvent, A_EVENT_FLAGS flagsToClear)
{
	A_STATUS ret = A_OK;
    assert(pEvent);
	(void)ret;
#if 0
    A_EVENT_FLAGS actl_flags = flagsToClear;
    if (pEvent->clearMode == kEventAutoClear) {
        krhino_event_get(&pEvent->event, flagsToClear, RHINO_OR_CLEAR, &actl_flags, RHINO_NO_WAIT);
    } else {
        krhino_event_get(&pEvent->event, flagsToClear, RHINO_OR, &actl_flags, RHINO_NO_WAIT);
    }
#else

    A_EVENT_FLAGS flag = ~flagsToClear;
    ret = (A_STATUS)krhino_event_set(&pEvent->event, flag, RHINO_AND);
#endif
	if(ret == A_OK){
		return A_OK;
	}

    return A_ERROR;
}

A_STATUS a_event_set(A_EVENT *pEvent, A_EVENT_FLAGS flagsToSet)
{
	A_STATUS ret = A_OK;
    assert(pEvent);

    ret = (A_STATUS)krhino_event_set(&pEvent->event, flagsToSet, RHINO_OR);
	if(ret == A_OK){
		return A_OK;
	}

    return A_ERROR;
}

A_STATUS a_event_wait(
    A_EVENT *pEvent, A_EVENT_FLAGS flagsToWait, boolean waitAll, uint32_t timeout, A_EVENT_FLAGS *setFlags)
{
    assert(pEvent);

    if (pEvent->clearMode == kEventAutoClear) {
        if (waitAll) {
            krhino_event_get(&pEvent->event, flagsToWait, RHINO_AND_CLEAR, setFlags, timeout);
        } else {
            krhino_event_get(&pEvent->event, flagsToWait, RHINO_OR_CLEAR, setFlags, timeout);
        }
    } else {
        if (waitAll) {
            krhino_event_get(&pEvent->event, flagsToWait, RHINO_AND, setFlags, timeout);
        } else {
            krhino_event_get(&pEvent->event, flagsToWait, RHINO_OR, setFlags, timeout);
        }
    }

    return A_OK;
}

uint32_t a_time_get_msec(void)
{
    tick_t ticks;

    if (__get_IPSR())
    {
        ticks = krhino_sys_tick_get();
    }
    else
    {
        ticks = krhino_sys_tick_get();
    }

    return TICKS_TO_MSEC(ticks);
}



void a_task_delay(uint32_t ticks)
{
    krhino_task_sleep(ticks);
}
