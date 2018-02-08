/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : bitrateEstimater.h
 * Description : bitrate estimater
 * History :
 *
 */

#ifndef BITRATE_ESTIMATER_H
#define BITRATE_ESTIMATER_H

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define BITRATE_ARRAY_SIZE          50
#define BITRATE_ESTIMATE_INTERVAL   5
#define PTS_DISCONTINUE_INTERVAL    1000000

typedef struct BR_ESTIMATER_NODE
{
    int64_t nFramePts;
    int     nFrameLen;
}BitrateEstimateNode;

typedef struct BitrateEstimater
{
    pthread_mutex_t     mutex;
    int                 nBitrate;
    int                 nWritePos;
    int                 nValidNodeCnt;
    int                 nWritePosLastEstimate;
    BitrateEstimateNode nodes[BITRATE_ARRAY_SIZE];
}BitrateEstimater;

BitrateEstimater* BitrateEstimaterCreate(void);

void BitrateEstimaterDestroy(BitrateEstimater* be);

void BitrateEstimaterUpdate(BitrateEstimater* be, int64_t nPts, int nFrameLen);

int BitrateEstimaterGetBitrate(BitrateEstimater* be);

void BitrateEstimaterReset(BitrateEstimater* be);

#endif
