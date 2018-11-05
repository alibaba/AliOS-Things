/*
 * Copyright (c) 2016, NXP Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
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
    vPortFree(addr);
}

/* FIXME !! */
extern uint32_t g_totAlloc;
void *a_malloc(int32_t size, uint8_t id)
{
    void *addr;

    addr = (void *)pvPortMalloc(size);
    assert(addr);
    if (addr != NULL)
    {
        /*FIXME: !!!*/
        g_totAlloc += size;
    }
    // UNUSED_ARGUMENT(id);

    return addr;
}

A_STATUS a_mutex_init(xSemaphoreHandle *pMutex)
{
    assert(pMutex);
    *pMutex = xSemaphoreCreateMutex();
    if (NULL == *pMutex)
    {
        return A_ERROR;
    }
    else
    {
        return A_OK;
    }
}

A_STATUS a_mutex_acquire(xSemaphoreHandle *pMutex)
{
    assert(pMutex);
    if (xSemaphoreGetMutexHolder(*pMutex) == xTaskGetCurrentTaskHandle())
    {
        return A_ERROR;
    }
    if (xSemaphoreTake(*pMutex, portMAX_DELAY) == pdFALSE)
    {
        return A_ERROR;
    }
    else
    {
        return A_OK;
    }
}

A_STATUS a_mutex_release(xSemaphoreHandle *pMutex)
{
    assert(pMutex);
    if (xSemaphoreGetMutexHolder(*pMutex) != xTaskGetCurrentTaskHandle())
    {
        assert(0); // TODO: use assert cause no-one check return value
        return A_ERROR;
    }
    if (xSemaphoreGive(*pMutex) == pdPASS)
    {
        return A_OK;
    }
    else
    {
        assert(0); // TODO: use assert cause no-one check return value
        return A_ERROR;
    }
}

boolean a_is_mutex_valid(xSemaphoreHandle *pMutex)
{
    // FIXME: check owner of mutex
    return true;
}

A_STATUS a_mutex_delete(xSemaphoreHandle *pMutex)
{
    assert(pMutex);
    assert(*pMutex);

    vSemaphoreDelete(*pMutex);
    return A_OK;
}

A_STATUS a_event_delete(event_t *pEvent)
{
    assert(pEvent);
    vEventGroupDelete(pEvent->eventHandler);
    return A_OK;
}

// zrusit autoclear flag !!!
A_STATUS a_event_init(event_t *pEvent, osa_event_clear_mode_t clearMode)
{
    assert(pEvent);

    pEvent->eventHandler = xEventGroupCreate();

    if (pEvent->eventHandler)
    {
        pEvent->clearMode = clearMode;
        return A_OK;
    }
    else
    {
        return A_ERROR;
    }
}

A_STATUS a_event_clear(event_t *pEvent, EventBits_t flagsToClear)
{
    assert(pEvent);

    if (__get_IPSR())
    {
        xEventGroupClearBitsFromISR(pEvent->eventHandler, flagsToClear);
    }
    else
    {
        xEventGroupClearBits(pEvent->eventHandler, flagsToClear);
    }

    return A_OK;
}

A_STATUS a_event_set(event_t *pEvent, EventBits_t flagsToSet)
{
    assert(pEvent);
    portBASE_TYPE xResult, taskToWake = pdFALSE;

    if (__get_IPSR())
    {
        xResult = xEventGroupSetBitsFromISR(pEvent->eventHandler, flagsToSet, &taskToWake);
        if (xResult != pdFAIL)
        {
            portYIELD_FROM_ISR(taskToWake);
        }
    }
    else
    {
        xEventGroupSetBits(pEvent->eventHandler, flagsToSet);
    }
    return A_OK;
}

A_STATUS a_event_wait(
    event_t *pEvent, EventBits_t flagsToWait, boolean waitAll, uint32_t timeout, EventBits_t *setFlags)
{
    assert(pEvent);
    BaseType_t clearMode;
    uint32_t timeoutTicks;
    EventBits_t flagsSave;

    /* Convert timeout from millisecond to tick. */
    if (timeout == 0xFFFFFFFFU) // OSA_WAIT_FOREVER)
    {
        timeoutTicks = portMAX_DELAY;
    }
    else
    {
        timeoutTicks = timeout / portTICK_PERIOD_MS;
    }

    clearMode = (kEventAutoClear == pEvent->clearMode) ? pdTRUE : pdFALSE;

    flagsSave = xEventGroupWaitBits(pEvent->eventHandler, flagsToWait, clearMode, (BaseType_t)waitAll, timeoutTicks);

    *setFlags = flagsSave & flagsToWait;
    // TODO DIFF: timeout ??
    if (*setFlags)
    {
        return A_OK;
    }
    else
    {
        return A_TIMEOUT; // TODO: unify with caller
    }
}

uint32_t a_time_get_msec(void)
{
    portTickType ticks;

    if (__get_IPSR())
    {
        ticks = xTaskGetTickCountFromISR();
    }
    else
    {
        ticks = xTaskGetTickCount();
    }

    return TICKS_TO_MSEC(ticks);
}

#define OSA_MAX_ISR_CRITICAL_SECTION_DEPTH 8

/* TODO: replace with semaphore */
static uint32_t g_base_priority_array[OSA_MAX_ISR_CRITICAL_SECTION_DEPTH];
static int32_t g_base_priority_top = 0;

void OSA_EnterCritical(osa_critical_section_mode_t mode)
{
    if (kCriticalDisableInt == mode)
    {
        if (__get_IPSR())
        {
            assert(g_base_priority_top < OSA_MAX_ISR_CRITICAL_SECTION_DEPTH);

            g_base_priority_array[g_base_priority_top] = portSET_INTERRUPT_MASK_FROM_ISR();
            g_base_priority_top++;
        }
        else
        {
            portENTER_CRITICAL();
        }
    }
    else
    {
        vTaskSuspendAll();
    }
}

void OSA_ExitCritical(osa_critical_section_mode_t mode)
{
    if (kCriticalDisableInt == mode)
    {
        if (__get_IPSR())
        {
            g_base_priority_top--;
            assert(g_base_priority_top >= 0);

            portCLEAR_INTERRUPT_MASK_FROM_ISR(g_base_priority_array[g_base_priority_top]);
        }
        else
        {
            portEXIT_CRITICAL();
        }
    }
    else
    {
        xTaskResumeAll();
    }
}
