/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : streamManager.h
 * Description : stream manager
 * History :
 *
 */

#ifndef STREAM_MANAGER_H
#define STREAM_MANAGER_H

#include <pthread.h>

typedef struct StreamFrameS StreamFrame;
struct StreamFrameS {
    void*   pData;
    int     nLength;
    int64_t nPts;
    int64_t nPcr;
    int64_t nDuration;
};

typedef struct StreamManager StreamManager;

StreamManager* StreamManagerCreate(int nMaxBufferSize, int nMaxFrameNum, int nStreamID);

void StreamManagerDestroy(StreamManager* pSm);

void StreamManagerReset(StreamManager* pSm);

int StreamManagerBufferSize(StreamManager* pSm);

int StreamManagerStreamFrameNum(StreamManager* pSm);

int StreamManagerStreamDataSize(StreamManager* pSm);

int StreamManagerRequestBuffer(StreamManager* pSm, int nRequireSize, char** ppBuf, int* pBufSize);

int StreamManagerAddStream(StreamManager* pSm, StreamFrame* pStreamFrame);

StreamFrame* StreamManagerRequestStream(StreamManager* pSm);

StreamFrame* StreamManagerGetFrameInfo(StreamManager* pSm, int nFrameIndex);

int StreamManagerReturnStream(StreamManager* pSm, StreamFrame* pStreamFrame);

/* This function is deprecated. It does nothing. */
int StreamManagerFlushStream(StreamManager* pSm, StreamFrame* pStreamFrame);

/* Rewind the stream to a specific position/time. You should call this function
 * after switch track.
 */
int StreamManagerRewind(StreamManager *p, int64_t curTime);

#endif
