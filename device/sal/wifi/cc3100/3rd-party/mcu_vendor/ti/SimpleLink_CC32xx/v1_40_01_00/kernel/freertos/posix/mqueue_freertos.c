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
 *  ======== mqueue_freertos.c ========
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* FreeRTOS header file */
#include <FreeRTOS.h>
#include <portmacro.h>
#include <queue.h>
#include <task.h>

#include <ti/drivers/dpl/HwiP.h>

#include "mqueue.h"
#include "time.h"

#define SCHED_SUSPEND(schedStarted) \
    if (schedStarted) {             \
        vTaskSuspendAll();          \
    }

#define SCHED_RESUME(schedStarted)  \
    if (schedStarted) {             \
        xTaskResumeAll();           \
    }

/*
 *  ======== MQueueObj ========
 *  The message queue object, created the first time the message queue is
 *  opened.
 */
typedef struct MQueueObj {
    struct MQueueObj  *next;
    struct MQueueObj  *prev;
    QueueHandle_t      queue;
    mq_attr            attrs;
    int                refCount;
    char              *name;
} MQueueObj;

/*
 *  ======== MQueueDesc ========
 *  Threads that call mq_open() get a descriptor handle.  This allows
 *  passing of different flags to mq_open().
 */
typedef struct MQueueDesc {
    MQueueObj *msgQueue;  /* The actual message queue object */
    long       flags;
} MQueueDesc;

/*
 *  Function for obtaining a timeout in Clock ticks from the difference of
 *  an absolute time and the current time.
 */
extern int _clock_abstime2ticks(clockid_t clockId,
        const struct timespec *abstime, uint32_t *ticks);

static MQueueObj *findInList(const char *name);

static MQueueObj *mqList = NULL;

/*
 *  ======== mq_close ========
 */
int mq_close(mqd_t mqdes)
{
    MQueueDesc *mqd = (MQueueDesc *)mqdes;
    MQueueObj  *msgQueue = mqd->msgQueue;
    bool        schedulerStarted;

    vPortFree(mqd);

    schedulerStarted = (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED);

    /* Disable the scheduler */
    SCHED_SUSPEND(schedulerStarted);

    msgQueue->refCount--;

    /* Re-enable the scheduler */
    SCHED_RESUME(schedulerStarted);

    return (0);
}

/*
 *  ======== mq_getattr ========
 */
int mq_getattr(mqd_t mqdes, struct mq_attr *mqstat)
{
    MQueueDesc *mqd = (MQueueDesc *)mqdes;
    MQueueObj  *msgQueue = mqd->msgQueue;

    /* Get the number of messages in the queue */
    if (HwiP_inISR()) {
        msgQueue->attrs.mq_curmsgs =
                uxQueueMessagesWaitingFromISR(msgQueue->queue);
    }
    else {
        msgQueue->attrs.mq_curmsgs = uxQueueMessagesWaiting(msgQueue->queue);
    }

    *mqstat = msgQueue->attrs;
    mqstat->mq_flags = mqd->flags;

    return (0);
}

/*
 *  ======== mq_open ========
 */
mqd_t mq_open(const char *name, int oflags, ...)
{
    va_list     va;
    mode_t      mode;
    mq_attr    *attrs = NULL;
    MQueueObj  *msgQueue;
    MQueueDesc *msgQueueDesc = NULL;
    MQueueDesc *mqd = (MQueueDesc *)(-1);
    bool        schedulerStarted;
    bool        failedStatus = false;

    va_start(va, oflags);

    if (oflags & O_CREAT) {
        mode = va_arg(va, mode_t);
        attrs = va_arg(va, mq_attr*);
    }

    va_end(va);

    if (name == NULL) {
        return ((mqd_t)(-1));
    }

    schedulerStarted = (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED);

    /* Disable the scheduler */
    SCHED_SUSPEND(schedulerStarted);

    msgQueue = findInList(name);

    /* Re-enable the scheduler */
    SCHED_RESUME(schedulerStarted);

    if ((msgQueue != NULL) && (oflags & O_CREAT) && (oflags & O_EXCL)) {
        /* Error: Message queue has alreadey been opened and O_EXCL is set */
        return ((mqd_t)(-1));
    }

    if ((msgQueue == NULL) && (!(oflags & O_CREAT) ||
                (attrs == NULL) || (attrs->mq_maxmsg <= 0))) {
        /*
         *  Error: Message has not been opened and O_CREAT is not set or
         *  attrs are bad.
         */
        return ((mqd_t)(-1));
    }

    /* Allocate the MQueueDesc */
    msgQueueDesc = pvPortMalloc(sizeof(MQueueDesc));
    if (msgQueueDesc == NULL) {
        return ((mqd_t)(-1));
    }

    if (msgQueue == NULL) {
        /* Allocate the MQueueObj */
        msgQueue = pvPortMalloc(sizeof(MQueueObj));

        if (msgQueue == NULL) {
            failedStatus = true;
            goto done;
        }

        msgQueue->refCount = 1;
        msgQueue->attrs = *attrs;

        msgQueue->name = pvPortMalloc(strlen(name) + 1);
        if (msgQueue->name == NULL) {
            failedStatus = true;
            goto done;
        }

        strcpy(msgQueue->name, name);

        msgQueue->queue = xQueueCreate((UBaseType_t)attrs->mq_maxmsg,
                (UBaseType_t)attrs->mq_msgsize);

        if (msgQueue->queue == NULL) {
            failedStatus = true;
            goto done;
        }

        /*
         *  Add the message queue to the list now
         */
        msgQueue->prev = NULL;

        /* Disable the scheduler */
        SCHED_SUSPEND(schedulerStarted);

        if (mqList != NULL) {
            mqList->prev = msgQueue;
        }

        msgQueue->next = mqList;
        mqList = msgQueue;
    }
    else {
        /* Disable the scheduler */
        SCHED_SUSPEND(schedulerStarted);

        msgQueue->refCount++;
    }

    SCHED_RESUME(schedulerStarted);

    msgQueueDesc->msgQueue = msgQueue;

    msgQueueDesc->flags = (oflags & O_NONBLOCK) ? O_NONBLOCK : 0;

done:
    if (failedStatus) {
        /*
         *  We only get here if we attempted to allocate msgQueue (i.e., it
         *  was not already in the list), so we're ok to free it.
         */
        if (msgQueue != NULL) {
            if (msgQueue->name != NULL) {
                vPortFree(msgQueue->name);
            }
            vPortFree(msgQueue);
        }

        if (msgQueueDesc != NULL) {
            vPortFree(msgQueueDesc);
        }
    }
    else {
        mqd = msgQueueDesc;
    }

    (void)mode;

    return ((mqd_t)mqd);
}

/*
 *  ======== mq_receive ========
 */
ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len,
        unsigned int *msg_prio)
{
    MQueueDesc *mqd = (MQueueDesc *)mqdes;
    MQueueObj  *msgQueue = mqd->msgQueue;
    BaseType_t  status;
    TickType_t  timeout;
    int         retVal;

    /*
     *  If msg_len is less than the message size attribute of the message
     *  queue, return an error.
     */
    if (msg_len < (msgQueue->attrs).mq_msgsize) {
        return (-1);
    }

    /*
     *  If the message queue is non-blocking, return an error (-1) if
     *  no message is available.
     *  Otherwise, wait forever to receive a message.
     */
    if (mqd->flags & O_NONBLOCK) {
        timeout = 0;
    }
    else {
        timeout = portMAX_DELAY;
    }

    if (HwiP_inISR()) {
        status = xQueueReceiveFromISR(msgQueue->queue, (void *)msg_ptr,
                NULL);
    }
    else {
        status = xQueueReceive(msgQueue->queue, (void *)msg_ptr, timeout);
    }

    if (status == pdTRUE) {
        retVal = (msgQueue->attrs).mq_msgsize;
    }
    else {
        retVal = -1;
    }

    return (retVal);
}

/*
 *  ======== mq_send ========
 */
int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len,
        unsigned int msg_prio)
{
    MQueueDesc *mqd = (MQueueDesc *)mqdes;
    MQueueObj  *msgQueue = mqd->msgQueue;
    BaseType_t  status;
    TickType_t  timeout;
    int         retVal = 0;

    /*
     *  If O_NONBLOCK is not set, block until space is available in the
     *  queue.  Otherwise, return -1 if no space is available.
     */
    if (mqd->flags & O_NONBLOCK) {
        timeout = 0;
    }
    else {
        timeout = portMAX_DELAY;
    }

    if (HwiP_inISR()) {
        status = xQueueSendFromISR(msgQueue->queue, (void *)msg_ptr,
                NULL);
    }
    else {
        status = xQueueSend(msgQueue->queue, (void *)msg_ptr, timeout);
    }

    if (status != pdTRUE) {
        retVal = -1;
    }

    return (retVal);
}

/*
 *  ======== mq_setattr ========
 */
int mq_setattr(mqd_t mqdes, const struct mq_attr *mqstat,
        struct mq_attr *omqstat)
{
    MQueueDesc *mqd = (MQueueDesc *)mqdes;
    MQueueObj  *msgQueue = mqd->msgQueue;
    bool        schedulerStarted;

    if ((mqstat->mq_flags != 0) && (mqstat->mq_flags != O_NONBLOCK)) {
        /* mq_flags should be 0 or O_NONBLOCK */
        return (-1);
    }

    /*
     *  The message queue attributes corresponding to the following
     *  members defined in the mq_attr structure shall be set to the
     *  specified values upon successful completion of mq_setattr():
     *  mq_flags
     *        The value of this member is the bitwise-logical OR of
     *        zero or more of O_NONBLOCK and any implementation-defined flags.
     *
     *  The values of the mq_maxmsg, mq_msgsize, and mq_curmsgs members of
     *  the mq_attr structure shall be ignored by mq_setattr().
    */
    schedulerStarted = (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED);

    /* Disable the scheduler */
    SCHED_SUSPEND(schedulerStarted);

    if (omqstat != NULL) {
        *omqstat = msgQueue->attrs;
        omqstat->mq_flags = mqd->flags;
    }

    mqd->flags = mqstat->mq_flags;

    /* Re-enable the scheduler */
    SCHED_RESUME(schedulerStarted);

    return (0);
}

/*
 *  ======== mq_timedreceive ========
 *  Receive the oldest of the highest priority messages from the message
 *  queue.
 *  If O_NONBLOCK was not specified when the message queue was opened,
 *  and no message exists on the queue, the wait will end when the
 *  timeout expires. If O_NONBLOCK is set, this function returns immediately
 *  (returning -1 if no message exists).
 *
 *  The timeout expires when the absolute time specified by abs_timeout
 *  passes, as measured by the clock on which timeouts are based (that is,
 *  when the value of that clock equals or exceeds abs_timeout), or if the
 *  absolute time specified by abs_timeout has already been passed at the
 *  time of the call.
 *
 *  If the Timers option is supported, the timeout shall be based on the
 *  CLOCK_REALTIME clock; if the Timers option is not supported, the timeout
 *  shall be based on the system clock as returned by the time() function.
 *
 *  The validity of the abs_timeout parameter need not be checked if a
 *  message can be removed from the message queue immediately.
 */
ssize_t mq_timedreceive(mqd_t mqdes, char *msg_ptr, size_t msg_len,
        unsigned int *msg_prio, const struct timespec *abstime)
{
    MQueueDesc *mqd = (MQueueDesc *)mqdes;
    MQueueObj  *msgQueue = mqd->msgQueue;
    uint32_t    timeout;
    BaseType_t  status;
    int         retVal;

    /*
     *  If msg_len is less than the message size attribute of the message
     *  queue, return an error.
     */
    if (msg_len < (msgQueue->attrs).mq_msgsize) {
        return (-1);
    }

    if (mqd->flags & O_NONBLOCK) {
        timeout = 0;
    }
    else {
        if (_clock_abstime2ticks(CLOCK_MONOTONIC, abstime, &timeout) != 0) {
            return (-1);
        }
    }

    if (HwiP_inISR()) {
        status = xQueueReceiveFromISR(msgQueue->queue, (void *)msg_ptr,
                NULL);
    }
    else {
        status = xQueueReceive(msgQueue->queue, (void *)msg_ptr,
                (TickType_t)timeout);
    }

    retVal = (status == pdTRUE) ? (msgQueue->attrs).mq_msgsize : -1;

    return (retVal);
}

/*
 *  ======== mq_timedsend ========
 *  Add a message to the message queue.
 *  If the message queue is full and O_NONBLOCK is not set, the wait for
 *  sufficient room in the queue is terminated when the specified
 *  timeout expires. If the message queue has O_NONBLOCK set,
 *  this function is equivalent to mq_send() (returns error if no space
 *  in queue).
 *
 *  The timeout shall expire when the absolute time specified by abstime
 *  passes, as measured by the clock on which timeouts are based (that is,
 *  when the value of that clock equals or exceeds abstime), or if the
 *  absolute time specified by abstime has already been passed at the time
 *  of the call.
 *
 *  The timeout shall be based on the CLOCK_REALTIME clock. The resolution
 *  of the timeout shall be the resolution of the clock on which it is based.
 *  The timespec argument is defined in the <time.h> header.
 *
 *  The validity of the abstime parameter need not be checked when there
 *  is sufficient room in the queue.
 */
int mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len,
        unsigned int msg_prio, const struct timespec *abstime)
{
    MQueueDesc *mqd = (MQueueDesc *)mqdes;
    MQueueObj  *msgQueue = mqd->msgQueue;
    uint32_t    timeout;
    BaseType_t  status;
    int         retVal = 0;

    if (mqd->flags & O_NONBLOCK) {
        timeout = 0;
    }
    else {
        if (_clock_abstime2ticks(CLOCK_MONOTONIC, abstime, &timeout) != 0) {
            return (-1);
        }
    }

    if (HwiP_inISR()) {
        status = xQueueSendFromISR(msgQueue->queue, (void *)msg_ptr,
                NULL);
    }
    else {
        status = xQueueSend(msgQueue->queue, (void *)msg_ptr,
                (TickType_t)timeout);
    }

    if (status != pdTRUE) {
        retVal = -1;
    }
    return (retVal);
}

/*
 *  ======== mq_unlink ========
 */
int mq_unlink(const char *name)
{
    MQueueObj  *msgQueue;
    MQueueObj  *nextMQ, *prevMQ;
    bool        schedulerStarted;

    schedulerStarted = (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED);

    /* Disable the scheduler */
    SCHED_SUSPEND(schedulerStarted);

    msgQueue = findInList(name);

    if ((msgQueue != NULL) && (msgQueue->refCount == 0)) {
        /* If the message queue is in the list, remove it. */
        if (mqList == msgQueue) {
            mqList = msgQueue->next;
        }
        else {
            prevMQ = msgQueue->prev;
            nextMQ = msgQueue->next;

            if (prevMQ) {
                prevMQ->next = nextMQ;
            }
            if (nextMQ) {
                nextMQ->prev = prevMQ;
            }
        }

        msgQueue->next = msgQueue->prev = NULL;

        /* Re-enable the scheduler */
        SCHED_RESUME(schedulerStarted);

        if (msgQueue->queue != NULL) {
            vQueueDelete(msgQueue->queue);
        }

        if (msgQueue->name != NULL) {
            vPortFree(msgQueue->name);
        }

        vPortFree(msgQueue);

        return (0);
    }

    /* Re-enable the scheduler */
    SCHED_RESUME(schedulerStarted);

    return (-1);
}


/*
 *************************************************************************
 *                      Internal functions
 *************************************************************************
 */

/*
 *  ======== findInList ========
 */
static MQueueObj *findInList(const char *name)
{
    MQueueObj *mq;

    mq = mqList;

    while (mq != NULL) {
        if (strcmp(mq->name, name) == 0) {
            return (mq);
        }
        mq = mq->next;
    }

    return (NULL);
}
