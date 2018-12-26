/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxMessageQueue.h
 * Description : message queue
 * History :
 *
 */

#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <stdint.h>
//#include <semaphore.h>
#include "pthread.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MessageQueueContext AwMessageQueue;

#ifndef uintptr_t
typedef size_t uintptr_t;
#endif

typedef struct AwMessage AwMessage;
typedef void (*msgHandlerT)(AwMessage *msg, void *arg);

/* Define your own struct AwMessage and put AWMESSAGE_COMMON_MEMBERS at the
 * beginning
 */
#define AWMESSAGE_COMMON_MEMBERS \
    int          messageId; \
    msgHandlerT  execute; \

/**
 * @param nMaxMessageNum How many messages the message queue can hold
 * @param pName The name of the message queue which is used in log output
 * @param nMessageSize sizeof(struct AwMessage)
 */
AwMessageQueue* AwMessageQueueCreate__(int nMaxMessageNum, const char* pName,
                                    size_t nMessageSize);
#define AwMessageQueueCreate(nMaxMessageNum, pName) \
        AwMessageQueueCreate__(nMaxMessageNum, pName, sizeof(AwMessage))

void AwMessageQueueDestroy(AwMessageQueue* mq);

int AwMessageQueuePostMessage(AwMessageQueue* mq, AwMessage* m);

int AwMessageQueueWaitMessage(AwMessageQueue* mq, int64_t timeout);

int AwMessageQueueGetMessage(AwMessageQueue* mq, AwMessage* m);

int AwMessageQueueTryGetMessage(AwMessageQueue* mq, AwMessage* m, int64_t timeout);

int AwMessageQueueFlush(AwMessageQueue* mq);

int AwMessageQueueGetCount(AwMessageQueue* mq);

//* define a semaphore timedwait method for common use.
int SemTimedWait(sem_t* sem, int64_t time_ms);

#ifdef __cplusplus
}
#endif

#endif
