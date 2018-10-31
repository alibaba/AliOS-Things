/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016 - 2017 NXP
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
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
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

#include "stdint.h"
#include "fsl_device_registers.h"
#include "usb.h"
#include "usb_osa.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define MSEC_TO_TICK(msec) ((1000L + ((uint32_t)configTICK_RATE_HZ * (uint32_t)(msec - 1U))) / 1000L)
#define TICKS_TO_MSEC(tick) ((tick)*1000uL / (uint32_t)configTICK_RATE_HZ)

#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))

#define USB_OSA_FREERTOS_EVENT_COUNT (2U)
#define USB_OSA_FREERTOS_SEM_COUNT (1U)
#define USB_OSA_FREERTOS_MUTEX_COUNT (3U)
#define USB_OSA_FREERTOS_MSGQ_COUNT (1U)
#define USB_OSA_FREERTOS_MSG_COUNT (8U)
#define USB_OSA_FREERTOS_MSG_SIZE (4U)

/* FreeRTOS Event status structure */
typedef struct _usb_osa_event_struct
{
    EventGroupHandle_t handle; /* The event handle */
    uint32_t flag;             /* Event flags, includes auto clear flag */
    StaticEventGroup_t event;  /* Event buffer */
    uint8_t isUsed;            /* Is used */
} usb_osa_event_struct_t;

/* FreeRTOS semaphore/mutex status structure */
typedef struct _usb_osa_sem_struct
{
    SemaphoreHandle_t handle; /* Semaphore handle */
    StaticSemaphore_t sem;    /* Semaphore buffer */
    uint8_t isUsed;           /* Is used */
} usb_osa_sem_struct_t;

/* BM msg status structure */
typedef struct _usb_osa_msg_struct
{
    uint32_t msg[USB_OSA_FREERTOS_MSG_SIZE]; /* Message entity pointer */
} usb_osa_msg_struct_t;

/* FreeRTOS msgq status structure */
typedef struct _usb_osa_msgq_struct
{
    QueueHandle_t handle;                                  /* MSGQ handle */
    usb_osa_msg_struct_t msgs[USB_OSA_FREERTOS_MSG_COUNT]; /* Message entity list */
    StaticQueue_t queue;                                   /* queue struct */
    uint8_t isUsed;                                        /* Is used */
} usb_osa_msgq_struct_t;

/* event struct */
static usb_osa_event_struct_t s_UsbFreertosEventStruct[USB_OSA_FREERTOS_EVENT_COUNT];

/* sem struct */
static usb_osa_sem_struct_t s_UsbFreertosSemStruct[USB_OSA_FREERTOS_SEM_COUNT];

/* mutex struct */
static usb_osa_sem_struct_t s_UsbFreertosMutexStruct[USB_OSA_FREERTOS_MUTEX_COUNT];

/* msgq struct */
static usb_osa_msgq_struct_t s_UsbFreertosMsgqStruct[USB_OSA_FREERTOS_MSGQ_COUNT];

#else
/* FreeRTOS Event status structure */
typedef struct _usb_osa_event_struct
{
    EventGroupHandle_t handle; /* The event handle */
    uint32_t flag;             /* Event flags, includes auto clear flag */
} usb_osa_event_struct_t;

#endif
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

void *USB_OsaMemoryAllocate(uint32_t length)
{
    void *p = (void *)pvPortMalloc(length);
    uint8_t *temp = (uint8_t *)p;
    if (p)
    {
        for (uint32_t count = 0U; count < length; count++)
        {
            temp[count] = 0U;
        }
    }
    return p;
}

void USB_OsaMemoryFree(void *p)
{
    vPortFree(p);
}
void USB_OsaEnterCritical(uint8_t *sr)
{
#if defined(__GIC_PRIO_BITS)
    if ((__get_CPSR() & CPSR_M_Msk) == 0x13)
#else
    if (__get_IPSR())
#endif
    {
        *sr = portSET_INTERRUPT_MASK_FROM_ISR();
    }
    else
    {
        portENTER_CRITICAL();
    }
}

void USB_OsaExitCritical(uint8_t sr)
{
#if defined(__GIC_PRIO_BITS)
    if ((__get_CPSR() & CPSR_M_Msk) == 0x13)
#else
    if (__get_IPSR())
#endif
    {
        portCLEAR_INTERRUPT_MASK_FROM_ISR(sr);
    }
    else
    {
        portEXIT_CRITICAL();
    }
}

usb_osa_status_t USB_OsaEventCreate(usb_osa_event_handle *handle, uint32_t flag)
{
    usb_osa_event_struct_t *event = NULL;
#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    USB_OSA_SR_ALLOC();
#endif

    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }

#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    USB_OSA_ENTER_CRITICAL();
    for (uint32_t index = 0; index < USB_OSA_FREERTOS_EVENT_COUNT; index++)
    {
        if (0 == s_UsbFreertosEventStruct[index].isUsed)
        {
            event = &s_UsbFreertosEventStruct[index];
            event->isUsed = 1U;
            break;
        }
    }
    USB_OSA_EXIT_CRITICAL();
#else
    event = (usb_osa_event_struct_t *)USB_OsaMemoryAllocate(sizeof(usb_osa_event_struct_t));
#endif
    if (NULL == event)
    {
        return kStatus_USB_OSA_Error;
    }

#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    event->handle = xEventGroupCreateStatic(&event->event);
    if (NULL == event->handle)
    {
        USB_OSA_ENTER_CRITICAL();
        event->isUsed = 0U;
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_OSA_Error;
    }
#else
    event->handle = xEventGroupCreate();
    if (NULL == event->handle)
    {
        USB_OsaMemoryFree(event);
        return kStatus_USB_OSA_Error;
    }
#endif
    event->flag = flag;
    *handle = event;
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaEventDestroy(usb_osa_event_handle handle)
{
    usb_osa_event_struct_t *event = (usb_osa_event_struct_t *)handle;
#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    USB_OSA_SR_ALLOC();
#endif

    if (handle)
    {
        if (event->handle)
        {
            vEventGroupDelete(event->handle);
        }
#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
        USB_OSA_ENTER_CRITICAL();
        event->isUsed = 0U;
        USB_OSA_EXIT_CRITICAL();
#else
        USB_OsaMemoryFree(handle);
#endif
        return kStatus_USB_OSA_Success;
    }
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaEventSet(usb_osa_event_handle handle, uint32_t bitMask)
{
    usb_osa_event_struct_t *event = (usb_osa_event_struct_t *)handle;
    portBASE_TYPE taskToWake = pdFALSE;
    if (handle)
    {
#if defined(__GIC_PRIO_BITS)
        if ((__get_CPSR() & CPSR_M_Msk) == 0x13)
#else
        if (__get_IPSR())
#endif
        {
            if (pdPASS == xEventGroupSetBitsFromISR(event->handle, (EventBits_t)bitMask, &taskToWake))
            {
                MISRAC_DISABLE
                portYIELD_FROM_ISR(taskToWake);
                MISRAC_ENABLE
            }
        }
        else
        {
            xEventGroupSetBits(event->handle, (EventBits_t)bitMask);
        }
        return kStatus_USB_OSA_Success;
    }
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaEventWait(
    usb_osa_event_handle handle, uint32_t bitMask, uint32_t flag, uint32_t timeout, uint32_t *bitSet)
{
    usb_osa_event_struct_t *event = (usb_osa_event_struct_t *)handle;
    BaseType_t autoClear;
    EventBits_t bits;

    if (handle)
    {
        if (!timeout)
        {
            timeout = portMAX_DELAY;
        }
        else
        {
            timeout = MSEC_TO_TICK(timeout);
        }

        if (event->flag)
        {
            autoClear = pdTRUE;
        }
        else
        {
            autoClear = pdFALSE;
        }

        bits = xEventGroupWaitBits(event->handle, (EventBits_t)bitMask, autoClear, (BaseType_t)flag, timeout);

        if (bitSet)
        {
            *bitSet = bits & ((EventBits_t)bitMask);
            if (*bitSet)
            {
                return kStatus_USB_OSA_Success;
            }
            return kStatus_USB_OSA_TimeOut;
        }
    }
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaEventCheck(usb_osa_event_handle handle, uint32_t bitMask, uint32_t *bitSet)
{
    usb_osa_event_struct_t *event = (usb_osa_event_struct_t *)handle;
    EventBits_t bits;

    if (handle)
    {
#if defined(__GIC_PRIO_BITS)
        if ((__get_CPSR() & CPSR_M_Msk) == 0x13)
#else
        if (__get_IPSR())
#endif
        {
            bits = xEventGroupGetBitsFromISR(event->handle);
        }
        else
        {
            bits = xEventGroupGetBits(event->handle);
        }
        bits = (bits & bitMask);
        if (bits)
        {
            if (bitSet)
            {
                *bitSet = bits & ((EventBits_t)bitMask);
            }
            return kStatus_USB_OSA_Success;
        }
    }
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaEventClear(usb_osa_event_handle handle, uint32_t bitMask)
{
    EventBits_t ev;
    usb_osa_event_struct_t *event = (usb_osa_event_struct_t *)handle;

    if (handle)
    {
#if defined(__GIC_PRIO_BITS)
        if ((__get_CPSR() & CPSR_M_Msk) == 0x13)
#else
        if (__get_IPSR())
#endif
        {
            xEventGroupClearBitsFromISR(event->handle, (EventBits_t)bitMask);
        }
        else
        {
            ev = xEventGroupClearBits(event->handle, (EventBits_t)bitMask);
            if (ev == 0)
            {
                return kStatus_USB_OSA_Error;
            }
        }
        return kStatus_USB_OSA_Success;
    }
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaSemCreate(usb_osa_sem_handle *handle, uint32_t count)
{
#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    usb_osa_sem_struct_t *sem = NULL;
    USB_OSA_SR_ALLOC();
#endif
    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }

#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    USB_OSA_ENTER_CRITICAL();
    for (uint32_t index = 0; index < USB_OSA_FREERTOS_SEM_COUNT; index++)
    {
        if (0 == s_UsbFreertosSemStruct[index].isUsed)
        {
            sem = &s_UsbFreertosSemStruct[index];
            sem->isUsed = 1U;
            break;
        }
    }
    USB_OSA_EXIT_CRITICAL();
    if (NULL == sem)
    {
        return kStatus_USB_OSA_Error;
    }
    sem->handle = xSemaphoreCreateCountingStatic(0xFFU, count, &sem->sem);
    if (NULL == (sem->handle))
    {
        USB_OSA_ENTER_CRITICAL();
        sem->isUsed = 0U;
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_OSA_Error;
    }
    *handle = (usb_osa_sem_handle)sem;
#else
    *handle = (usb_osa_sem_handle)xSemaphoreCreateCounting(0xFFU, count);
    if (NULL == (*handle))
    {
        return kStatus_USB_OSA_Error;
    }
#endif

    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaSemDestroy(usb_osa_sem_handle handle)
{
#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    usb_osa_sem_struct_t *sem = (usb_osa_sem_struct_t *)handle;
    USB_OSA_SR_ALLOC();
#endif
    if (handle)
    {
#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
        vSemaphoreDelete(sem->handle);
        USB_OSA_ENTER_CRITICAL();
        sem->isUsed = 0U;
        USB_OSA_EXIT_CRITICAL();
#else
        vSemaphoreDelete(handle);
#endif
        return kStatus_USB_OSA_Success;
    }
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaSemPost(usb_osa_sem_handle handle)
{
#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    usb_osa_sem_struct_t *osaSem = (usb_osa_sem_struct_t *)handle;
    SemaphoreHandle_t sem;
#else
    SemaphoreHandle_t sem = (SemaphoreHandle_t)handle;
#endif
    portBASE_TYPE taskToWake = pdFALSE;

    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }

#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    sem = (SemaphoreHandle_t)osaSem->handle;
#endif
#if defined(__GIC_PRIO_BITS)
    if ((__get_CPSR() & CPSR_M_Msk) == 0x13)
#else
    if (__get_IPSR())
#endif
    {
        if (pdPASS == xSemaphoreGiveFromISR(sem, &taskToWake))
        {
            MISRAC_DISABLE
            portYIELD_FROM_ISR(taskToWake);
            MISRAC_ENABLE
            return kStatus_USB_OSA_Success;
        }
    }
    else
    {
        if (pdTRUE == xSemaphoreGive(sem))
        {
            return kStatus_USB_OSA_Success;
        }
    }
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaSemWait(usb_osa_sem_handle handle, uint32_t timeout)
{
#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    usb_osa_sem_struct_t *osaSem = (usb_osa_sem_struct_t *)handle;
    SemaphoreHandle_t sem;
#else
    SemaphoreHandle_t sem = (SemaphoreHandle_t)handle;
#endif

    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }
#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    sem = (SemaphoreHandle_t)osaSem->handle;
#endif
    if (!timeout)
    {
        timeout = portMAX_DELAY;
    }
    else
    {
        timeout = MSEC_TO_TICK(timeout);
    }

    if (pdFALSE == xSemaphoreTake(sem, timeout))
    {
        return kStatus_USB_OSA_TimeOut;
    }
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMutexCreate(usb_osa_mutex_handle *handle)
{
#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    usb_osa_sem_struct_t *mutex = NULL;
    USB_OSA_SR_ALLOC();
#endif
    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }
#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    USB_OSA_ENTER_CRITICAL();
    for (uint32_t index = 0; index < USB_OSA_FREERTOS_MUTEX_COUNT; index++)
    {
        if (0 == s_UsbFreertosMutexStruct[index].isUsed)
        {
            mutex = &s_UsbFreertosMutexStruct[index];
            mutex->isUsed = 1U;
            break;
        }
    }
    USB_OSA_EXIT_CRITICAL();
    if (NULL == mutex)
    {
        return kStatus_USB_OSA_Error;
    }
    mutex->handle = xSemaphoreCreateRecursiveMutexStatic(&mutex->sem);
    if (NULL == (mutex->handle))
    {
        USB_OSA_ENTER_CRITICAL();
        mutex->isUsed = 0U;
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_OSA_Error;
    }
    *handle = (usb_osa_sem_handle)mutex;
#else
    *handle = (usb_osa_sem_handle)xSemaphoreCreateRecursiveMutex();
    if (NULL == (*handle))
    {
        return kStatus_USB_OSA_Error;
    }
#endif
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMutexDestroy(usb_osa_mutex_handle handle)
{
#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    usb_osa_sem_struct_t *mutex = (usb_osa_sem_struct_t *)handle;
    USB_OSA_SR_ALLOC();
#endif
    if (handle)
    {
#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
        vSemaphoreDelete(mutex->handle);
        USB_OSA_ENTER_CRITICAL();
        mutex->isUsed = 0U;
        USB_OSA_EXIT_CRITICAL();
#else
        vSemaphoreDelete(handle);
#endif
        return kStatus_USB_OSA_Success;
    }
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaMutexLock(usb_osa_mutex_handle handle)
{
#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    usb_osa_sem_struct_t *osaMutex = (usb_osa_sem_struct_t *)handle;
    SemaphoreHandle_t mutex;
#else
    SemaphoreHandle_t mutex = (SemaphoreHandle_t)handle;
#endif

    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }
#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    mutex = (SemaphoreHandle_t)osaMutex->handle;
#endif
    if (xSemaphoreTakeRecursive(mutex, portMAX_DELAY) == pdFALSE)
    {
        return kStatus_USB_OSA_TimeOut;
    }

    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMutexUnlock(usb_osa_mutex_handle handle)
{
#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    usb_osa_sem_struct_t *osaMutex = (usb_osa_sem_struct_t *)handle;
    SemaphoreHandle_t mutex;
#else
    SemaphoreHandle_t mutex = (SemaphoreHandle_t)handle;
#endif

    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }
#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    mutex = (SemaphoreHandle_t)osaMutex->handle;
#endif
    if (xSemaphoreGiveRecursive(mutex) == pdFALSE)
    {
        return kStatus_USB_OSA_Error;
    }
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMsgqCreate(usb_osa_msgq_handle *handle, uint32_t count, uint32_t size)
{
#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    usb_osa_msgq_struct_t *msgq = NULL;
    USB_OSA_SR_ALLOC();
#endif
    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }

#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    USB_OSA_ENTER_CRITICAL();
    for (uint32_t index = 0; index < USB_OSA_FREERTOS_MSGQ_COUNT; index++)
    {
        if (0 == s_UsbFreertosMsgqStruct[index].isUsed)
        {
            msgq = &s_UsbFreertosMsgqStruct[index];
            msgq->isUsed = 1U;
            break;
        }
    }
    USB_OSA_EXIT_CRITICAL();
    if (NULL == msgq)
    {
        return kStatus_USB_OSA_Error;
    }
    msgq->handle = xQueueCreateStatic(count, size * sizeof(uint32_t), (uint8_t *)&msgq->msgs[0], &msgq->queue);
    if (NULL == (msgq->handle))
    {
        USB_OSA_ENTER_CRITICAL();
        msgq->isUsed = 0U;
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_OSA_Error;
    }
    *handle = (usb_osa_msgq_handle)msgq;
#else
    *handle = (usb_osa_msgq_handle)xQueueCreate(count, size * sizeof(uint32_t));
    if (NULL == (*handle))
    {
        return kStatus_USB_OSA_Error;
    }
#endif

    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMsgqDestroy(usb_osa_msgq_handle handle)
{
#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    usb_osa_msgq_struct_t *msgq = (usb_osa_msgq_struct_t *)handle;
    USB_OSA_SR_ALLOC();
#endif
    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }

#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    vQueueDelete((QueueHandle_t)(msgq->handle));
    USB_OSA_ENTER_CRITICAL();
    msgq->isUsed = 0U;
    USB_OSA_EXIT_CRITICAL();
#else
    vQueueDelete((QueueHandle_t)handle);
#endif
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMsgqSend(usb_osa_msgq_handle handle, void *msg)
{
#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    usb_osa_msgq_struct_t *osaMsgq = (usb_osa_msgq_struct_t *)handle;
    QueueHandle_t msgq;
#else
    QueueHandle_t msgq = (QueueHandle_t)handle;
#endif
    portBASE_TYPE taskToWake = pdFALSE;

    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }

#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    msgq = osaMsgq->handle;
#endif

#if defined(__GIC_PRIO_BITS)
    if ((__get_CPSR() & CPSR_M_Msk) == 0x13)
#else
    if (__get_IPSR())
#endif
    {
        if (pdPASS == xQueueSendToBackFromISR(msgq, msg, &taskToWake))
        {
            MISRAC_DISABLE
            portYIELD_FROM_ISR(taskToWake);
            MISRAC_ENABLE
            return kStatus_USB_OSA_Success;
        }
    }
    else
    {
        if (pdPASS == xQueueSendToBack(msgq, msg, 0U))
        {
            return kStatus_USB_OSA_Success;
        }
    }
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaMsgqRecv(usb_osa_msgq_handle handle, void *msg, uint32_t timeout)
{
#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    usb_osa_msgq_struct_t *osaMsgq = (usb_osa_msgq_struct_t *)handle;
    QueueHandle_t msgq;
#else
    QueueHandle_t msgq = (QueueHandle_t)handle;
#endif

    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }

#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    msgq = osaMsgq->handle;
#endif

    if (!timeout)
    {
        timeout = portMAX_DELAY;
    }
    else
    {
        timeout = MSEC_TO_TICK(timeout);
    }
    if (pdPASS != xQueueReceive(msgq, msg, timeout))
    {
        return kStatus_USB_OSA_TimeOut;
    }
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMsgqCheck(usb_osa_msgq_handle handle, void *msg)
{
#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    usb_osa_msgq_struct_t *osaMsgq = (usb_osa_msgq_struct_t *)handle;
    QueueHandle_t msgq;
#else
    QueueHandle_t msgq = (QueueHandle_t)handle;
#endif

    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }

#if (defined(configSUPPORT_STATIC_ALLOCATION) && (configSUPPORT_STATIC_ALLOCATION > 0U))
    msgq = osaMsgq->handle;
#endif

    if (uxQueueMessagesWaiting(msgq))
    {
        if (pdPASS == xQueueReceive(msgq, msg, 1U))
        {
            return kStatus_USB_OSA_Success;
        }
    }

    return kStatus_USB_OSA_Error;
}
