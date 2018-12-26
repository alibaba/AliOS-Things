/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxQueue.h
 * Description : Queue
 * History :
 *
 */

#ifndef CDX_QUEUE_H
#define CDX_QUEUE_H
#include <CdxTypes.h>
#include <cdx_log.h>
#include <AwPool.h>

typedef struct CdxQueueS CdxQueueT;
typedef void * CdxQueueDataT;

struct CdxQueueOpsS
{
    CdxQueueDataT (*pop)(CdxQueueT *);
    cdx_err (*push)(CdxQueueT *, CdxQueueDataT);
    cdx_bool (*empty)(CdxQueueT *);

};

struct CdxQueueS
{
    struct CdxQueueOpsS *ops;
};

static inline CdxQueueDataT CdxQueuePop(CdxQueueT *queue)
{
    CDX_CHECK(queue);
    CDX_CHECK(queue->ops);
    CDX_CHECK(queue->ops->pop);
    return queue->ops->pop(queue);
}

static inline cdx_err CdxQueuePush(CdxQueueT *queue, CdxQueueDataT data)
{
    CDX_CHECK(queue);
    CDX_CHECK(queue->ops);
    CDX_CHECK(queue->ops->push);
    return queue->ops->push(queue, data);
}

static inline cdx_bool CdxQueueEmpty(CdxQueueT *queue)
{
    CDX_CHECK(queue);
    CDX_CHECK(queue->ops);
    CDX_CHECK(queue->ops->empty);
    return queue->ops->empty(queue);
}

#ifdef __cplusplus
extern "C"
{
#endif
/*this is a look free queue*/
CdxQueueT *CdxQueueCreate(AwPoolT *pool);

void CdxQueueDestroy(CdxQueueT *queue);

#ifdef __cplusplus
}
#endif

#endif
