/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxSeqBuffer.h
 * Description : SeqBuffer
 * History :
 *
 */

#ifndef CDX_SEQ_BUFFER_H
#define CDX_SEQ_BUFFER_H
#include <CdxList.h>
#include <CdxMemory.h>
#include <CdxBuffer.h>
#include <CdxMeta.h>

struct SeqBufferS
{
    CdxListNodeT node;
    CdxBufferT *val;
    cdx_uint32 seqNum;
};

#define SeqBufferDel(buf)\
    CdxListDel(&buf->node); \
    CdxBufferDestroy(buf->val); \
    CdxFree(buf)

static inline void SeqBufferClear(CdxListT *seqBufList)
{
    struct SeqBufferS *posSeqBuf, *tmpSeqBuf;

    CdxListForEachEntrySafe(posSeqBuf, tmpSeqBuf, seqBufList, node)
    {
        SeqBufferDel(posSeqBuf);
    }
    return ;
}

static inline cdx_err SeqBufferInsert(CdxListT *seqBufList, struct SeqBufferS *seqBuf)
{
    struct SeqBufferS *posSeqBuf;
    CdxListForEachEntryReverse(posSeqBuf, seqBufList, node)
    {
        if (posSeqBuf->seqNum <= seqBuf->seqNum)
        {
            break;
        }
    }

    if (posSeqBuf->seqNum == seqBuf->seqNum)
    {
        CDX_LOGW("Discarding duplicate buffer (%u, %u)",
                           posSeqBuf->seqNum, seqBuf->seqNum);
        return CDX_FAILURE;
    }

    CdxListAddAfter(&seqBuf->node, &posSeqBuf->node);
    return CDX_SUCCESS;
}

#endif
