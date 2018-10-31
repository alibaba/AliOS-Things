/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
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
#include "usb.h"
#include "usb_osa.h"
#include "stdlib.h"
#include "fsl_device_registers.h"
#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define USB_OSA_BM_EVENT_COUNT (2U)
#define USB_OSA_BM_SEM_COUNT (1U)
#define USB_OSA_BM_MSGQ_COUNT (1U)
#define USB_OSA_BM_MSG_COUNT (8U)
#define USB_OSA_BM_MSG_SIZE (4U)

/* BM Event status structure */
typedef struct _usb_osa_event_struct
{
    uint32_t value; /* Event mask */
    uint32_t flag;  /* Event flags, includes auto clear flag */
    uint8_t isUsed; /* Is used */
} usb_osa_event_struct_t;

/* BM semaphore status structure */
typedef struct _usb_osa_sem_struct
{
    uint32_t value; /* Semaphore count */
    uint8_t isUsed; /* Is used */
} usb_osa_sem_struct_t;

/* BM msg status structure */
typedef struct _usb_osa_msg_struct
{
    uint32_t msg[USB_OSA_BM_MSG_SIZE]; /* Message entity pointer */
} usb_osa_msg_struct_t;

/* BM msgq status structure */
typedef struct _usb_osa_msgq_struct
{
    usb_osa_msg_struct_t msgs[USB_OSA_BM_MSG_COUNT]; /* Message entity list */
    uint32_t count;                                  /* Max message entity count */
    uint32_t msgSize;                                /* Size of each message */
    uint32_t msgCount;                               /* Valid messages */
    uint32_t index;                                  /* The first empty message entity index */
    uint32_t current;                                /* The vaild message index */
    uint8_t isUsed;                                  /* Is used */
} usb_osa_msgq_struct_t;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

USB_GLOBAL USB_RAM_ADDRESS_ALIGNMENT(USB_DATA_ALIGN_SIZE) static usb_osa_sem_struct_t
    s_UsbBmSemStruct[USB_OSA_BM_SEM_COUNT];
USB_GLOBAL USB_RAM_ADDRESS_ALIGNMENT(USB_DATA_ALIGN_SIZE) static usb_osa_event_struct_t
    s_UsbBmEventStruct[USB_OSA_BM_EVENT_COUNT];
USB_GLOBAL USB_RAM_ADDRESS_ALIGNMENT(USB_DATA_ALIGN_SIZE) static usb_osa_msgq_struct_t
    s_UsbBmMsgqStruct[USB_OSA_BM_MSGQ_COUNT];

/*******************************************************************************
 * Code
 ******************************************************************************/

void *USB_OsaMemoryAllocate(uint32_t length)
{
    void *p = (void *)malloc(length);
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
    free(p);
}

void USB_OsaEnterCritical(uint32_t *sr)
{
    *sr = DisableGlobalIRQ();
    __ASM("CPSID I");
}

void USB_OsaExitCritical(uint32_t sr)
{
    EnableGlobalIRQ(sr);
}

usb_osa_status_t USB_OsaEventCreate(usb_osa_event_handle *handle, uint32_t flag)
{
    usb_osa_event_struct_t *event = NULL;
    USB_OSA_SR_ALLOC();

    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }

    USB_OSA_ENTER_CRITICAL();
    for (uint32_t i = 0; i < USB_OSA_BM_EVENT_COUNT; i++)
    {
        if (0 == s_UsbBmEventStruct[i].isUsed)
        {
            event = &s_UsbBmEventStruct[i];
            break;
        }
    }

    if (NULL == event)
    {
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_OSA_Error;
    }

    event->value = 0U;
    event->flag = flag;
    event->isUsed = 1;
    *handle = event;
    USB_OSA_EXIT_CRITICAL();
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaEventDestroy(usb_osa_event_handle handle)
{
    usb_osa_event_struct_t *event = (usb_osa_event_struct_t *)handle;
    USB_OSA_SR_ALLOC();

    if (handle)
    {
        USB_OSA_ENTER_CRITICAL();
        event->isUsed = 0;
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_OSA_Success;
    }
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaEventSet(usb_osa_event_handle handle, uint32_t bitMask)
{
    usb_osa_event_struct_t *event = (usb_osa_event_struct_t *)handle;
    USB_OSA_SR_ALLOC();

    if (handle)
    {
        USB_OSA_ENTER_CRITICAL();
        event->value |= bitMask;
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_OSA_Success;
    }
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaEventWait(
    usb_osa_event_handle handle, uint32_t bitMask, uint32_t flag, uint32_t timeout, uint32_t *bitSet)
{
    usb_osa_event_struct_t *event = (usb_osa_event_struct_t *)handle;
    uint32_t bits;
    USB_OSA_SR_ALLOC();

    if (handle)
    {
        USB_OSA_ENTER_CRITICAL();
        bits = event->value & bitMask;
        if (flag)
        {
            if (bits != bitMask)
            {
                USB_OSA_EXIT_CRITICAL();
                return kStatus_USB_OSA_TimeOut;
            }
        }
        else
        {
            if (!bits)
            {
                USB_OSA_EXIT_CRITICAL();
                return kStatus_USB_OSA_TimeOut;
            }
        }
        if (bitSet)
        {
            *bitSet = bits;
        }
        if (event->flag)
        {
            event->value &= ~bits;
        }
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_OSA_Success;
    }
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaEventCheck(usb_osa_event_handle handle, uint32_t bitMask, uint32_t *bitSet)
{
    usb_osa_event_struct_t *event = (usb_osa_event_struct_t *)handle;
    uint32_t bits;
    USB_OSA_SR_ALLOC();

    if (handle)
    {
        USB_OSA_ENTER_CRITICAL();
        bits = event->value & bitMask;

        if (!bits)
        {
            USB_OSA_EXIT_CRITICAL();
            return kStatus_USB_OSA_Error;
        }

        if (bitSet)
        {
            *bitSet = bits;
        }
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_OSA_Success;
    }
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaEventClear(usb_osa_event_handle handle, uint32_t bitMask)
{
    usb_osa_event_struct_t *event = (usb_osa_event_struct_t *)handle;
    uint32_t bits;
    USB_OSA_SR_ALLOC();

    if (handle)
    {
        USB_OSA_ENTER_CRITICAL();
        bits = event->value & bitMask;
        event->value &= ~bits;
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_OSA_Success;
    }
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaSemCreate(usb_osa_sem_handle *handle, uint32_t count)
{
    usb_osa_sem_struct_t *sem = NULL;
    USB_OSA_SR_ALLOC();

    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }

    USB_OSA_ENTER_CRITICAL();
    for (uint32_t i = 0; i < USB_OSA_BM_SEM_COUNT; i++)
    {
        if (0 == s_UsbBmSemStruct[i].isUsed)
        {
            sem = &s_UsbBmSemStruct[i];
            break;
        }
    }
    if (NULL == sem)
    {
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_OSA_Error;
    }

    sem->value = count;
    sem->isUsed = 1;
    *handle = sem;
    USB_OSA_EXIT_CRITICAL();
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaSemDestroy(usb_osa_sem_handle handle)
{
    usb_osa_sem_struct_t *sem = (usb_osa_sem_struct_t *)handle;
    USB_OSA_SR_ALLOC();

    if (handle)
    {
        USB_OSA_ENTER_CRITICAL();
        sem->isUsed = 0;
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_OSA_Success;
    }
    return kStatus_USB_OSA_Error;
}

usb_osa_status_t USB_OsaSemPost(usb_osa_sem_handle handle)
{
    usb_osa_sem_struct_t *sem = (usb_osa_sem_struct_t *)handle;
    USB_OSA_SR_ALLOC();

    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }

    USB_OSA_ENTER_CRITICAL();
    sem->value++;
    USB_OSA_EXIT_CRITICAL();
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaSemWait(usb_osa_sem_handle handle, uint32_t timeout)
{
    usb_osa_sem_struct_t *sem = (usb_osa_sem_struct_t *)handle;
    USB_OSA_SR_ALLOC();

    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }

    USB_OSA_ENTER_CRITICAL();
    if (sem->value)
    {
        sem->value--;
    }
    else
    {
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_OSA_TimeOut;
    }
    USB_OSA_EXIT_CRITICAL();
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMutexCreate(usb_osa_mutex_handle *handle)
{
    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }
    *handle = (usb_osa_mutex_handle)0xFFFF0000U;
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMutexDestroy(usb_osa_mutex_handle handle)
{
    return kStatus_USB_OSA_Success;
}
usb_osa_status_t USB_OsaMutexLock(usb_osa_mutex_handle handle)
{
    return kStatus_USB_OSA_Success;
}
usb_osa_status_t USB_OsaMutexUnlock(usb_osa_mutex_handle handle)
{
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMsgqCreate(usb_osa_msgq_handle *handle, uint32_t count, uint32_t size)
{
    usb_osa_msgq_struct_t *msgq = NULL;
    USB_OSA_SR_ALLOC();

    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }
    USB_OSA_ENTER_CRITICAL();

    for (uint32_t i = 0; i < USB_OSA_BM_MSGQ_COUNT; i++)
    {
        if (0 == s_UsbBmMsgqStruct[i].isUsed)
        {
            msgq = &s_UsbBmMsgqStruct[i];
            break;
        }
    }
    if ((NULL == msgq) || (count > USB_OSA_BM_MSG_COUNT) || (size > USB_OSA_BM_MSG_SIZE))
    {
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_OSA_Error;
    }
    msgq->count = count;
    msgq->msgSize = size;
    msgq->msgCount = 0U;
    msgq->index = 0U;
    msgq->current = 0U;
    msgq->isUsed = 1;
    *handle = msgq;
    USB_OSA_EXIT_CRITICAL();
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMsgqDestroy(usb_osa_msgq_handle handle)
{
    usb_osa_msgq_struct_t *msgq = (usb_osa_msgq_struct_t *)handle;
    USB_OSA_SR_ALLOC();

    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }
    USB_OSA_ENTER_CRITICAL();
    msgq->isUsed = 0;
    USB_OSA_EXIT_CRITICAL();
    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMsgqSend(usb_osa_msgq_handle handle, void *msg)
{
    usb_osa_msgq_struct_t *msgq = (usb_osa_msgq_struct_t *)handle;
    usb_osa_msg_struct_t *msgEntity;
    uint32_t *p;
    uint32_t *q;
    uint32_t count;
    USB_OSA_SR_ALLOC();

    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }
    USB_OSA_ENTER_CRITICAL();
    if (msgq->msgCount >= msgq->count)
    {
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_OSA_Error;
    }

    msgEntity = &msgq->msgs[msgq->index];
    p = (uint32_t *)&msgEntity->msg[0];
    q = (uint32_t *)msg;

    for (count = 0U; count < msgq->msgSize; count++)
    {
        p[count] = q[count];
    }

    if (0U == msgq->msgCount)
    {
        msgq->current = msgq->index;
    }

    msgq->msgCount++;
    msgq->index++;
    msgq->index = msgq->index % msgq->count;

    USB_OSA_EXIT_CRITICAL();

    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMsgqRecv(usb_osa_msgq_handle handle, void *msg, uint32_t timeout)
{
    usb_osa_msgq_struct_t *msgq = (usb_osa_msgq_struct_t *)handle;
    usb_osa_msg_struct_t *msgEntity;
    uint32_t *p;
    uint32_t *q;
    uint32_t count;
    USB_OSA_SR_ALLOC();

    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }
    USB_OSA_ENTER_CRITICAL();
    if (msgq->msgCount < 1U)
    {
        USB_OSA_EXIT_CRITICAL();
        return kStatus_USB_OSA_TimeOut;
    }

    msgEntity = &msgq->msgs[msgq->current];
    q = (uint32_t *)&msgEntity->msg[0];
    p = (uint32_t *)msg;

    for (count = 0U; count < msgq->msgSize; count++)
    {
        p[count] = q[count];
    }

    msgq->msgCount--;
    msgq->current++;
    msgq->current = msgq->current % msgq->count;

    USB_OSA_EXIT_CRITICAL();

    return kStatus_USB_OSA_Success;
}

usb_osa_status_t USB_OsaMsgqCheck(usb_osa_msgq_handle handle, void *msg)
{
    usb_osa_msgq_struct_t *msgq = (usb_osa_msgq_struct_t *)handle;
    uint32_t msgCount;
    USB_OSA_SR_ALLOC();

    if (!handle)
    {
        return kStatus_USB_OSA_Error;
    }

    USB_OSA_ENTER_CRITICAL();
    msgCount = msgq->msgCount;
    USB_OSA_EXIT_CRITICAL();

    if (msgCount)
    {
        if (kStatus_USB_OSA_Success == USB_OsaMsgqRecv(msgq, msg, 0U))
        {
            return kStatus_USB_OSA_Success;
        }
    }

    return kStatus_USB_OSA_Error;
}
