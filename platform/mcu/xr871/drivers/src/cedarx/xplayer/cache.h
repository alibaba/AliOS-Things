/*
* Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
* All rights reserved.
*
* File : cache.h
* Description : cache policy for net stream
* History :
*   Author  : AL3
*   Date    : 2015/05/05
*   Comment : first version
*
*/

#ifndef CACHE_H
#define CACHE_H

//#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "CdxParser.h"
#if VIDEO_SUPPORT
#include "vdecoder.h"
#endif
#include "player.h"

#define likely(p)     __builtin_expect(!!(p), 1)
#define unlikely(p)   __builtin_expect(!!(p), 0)

typedef struct CacheNode_t CacheNode;
struct CacheNode_t
{
    unsigned char* pData;
    int            nLength;
    int64_t        nPts;
    int64_t        nPcr;
    int            bIsFirstPart;
    int            bIsLastPart;
    int            eMediaType;
    int            nStreamIndex;
    int            nFlags;
    int64_t        nDuration;
    int            infoVersion;
    void*          info;
    CacheNode*     pNext;
};

typedef struct StreamCache_t
{
    pthread_mutex_t         mutex;
    int                     nMaxBufferSize;
    int                     nStartPlaySize;

    int                     nDataSize;
    int                     nFrameNum;
#if VIDEO_SUPPORT
    int                     nVideoFrameNum;
#endif
    CacheNode*              pHead;
    CacheNode*              pTail;

    int                     nPassedDataSize;
    int                     nPassedFrameNum;
#if VIDEO_SUPPORT
    int                     nPassedVideoFrameNum;
#endif
    CacheNode*              pPassedHead;

    int64_t                 nLastValidPts;
    CacheNode*              pNodeWithLastValidPts;
    int64_t                 nLastValidPcr;
    CacheNode*              pNodeWithLastValidPcr;
    int64_t                 nFirstPts;

    int                     nBitrate;           //* for ts/m2ts stream seek processing.
    Player*                 pPlayer;            //* for ts/m2ts stream seek processing.
    CdxParserTypeT          eContainerFormat;   //* for seek processing.
#if VIDEO_SUPPORT
    enum EVIDEOCODECFORMAT  eVideoCodecFormat;  //* for finding key frame in cache when seek.
#endif
}StreamCache;

StreamCache* StreamCacheCreate(void);

void StreamCacheDestroy(StreamCache* c);

void StreamCacheSetSize(StreamCache* c, int nStartPlaySize, int nMaxBufferSize);

int StreamCacheGetSize(StreamCache* c);

int StreamCacheUnderflow(StreamCache* c);

int StreamCacheOverflow(StreamCache* c);

int StreamCacheDataEnough(StreamCache* c);

CacheNode* StreamCacheNextFrame(StreamCache* c);

void StreamCacheFlushOneFrame(StreamCache* c);

int StreamCacheAddOneFrame(StreamCache* c, CacheNode* node);

void StreamCacheFlushAll(StreamCache* c);

int StreamCacheGetBufferFullness(StreamCache* c);

int StreamCacheGetLoadingProgress(StreamCache* c);

int StreamCacheSetMediaFormat(StreamCache*           c,
                              CdxParserTypeT         eContainerFormat,
#if VIDEO_SUPPORT
                              enum EVIDEOCODECFORMAT eVideoCodecFormat,
#else
                              int                    eVideoCodecFormat,
#endif
                              int                    nBitrate);

int StreamCacheSetPlayer(StreamCache* c, Player* pPlayer);

int64_t StreamCacheSeekTo(StreamCache* c, int64_t nSeekTimeUs);

#endif
