/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : framerateEstimater.h
 * Description : framerate estimater
 * History :
 *
 */

#ifndef FRAME_ESTIMATER_H
#define FRAME_ESTIMATER_H

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define FRAMERATE_ARRAY_SIZE    4
#define FRAMERATE_START_ESTIMATE_SIZE 2

typedef struct FramerateEstimater
{
    int64_t         nPts[FRAMERATE_ARRAY_SIZE];
    int             nWritePos;
    int             nValidPtsCnt;
    int             nFrameDuration;
    float           fPlayRate;
    pthread_mutex_t mutex;
}FramerateEstimater;

FramerateEstimater* FramerateEstimaterCreate(void);

void FramerateEstimaterDestroy(FramerateEstimater* fe);

void FramerateEstimaterUpdate(FramerateEstimater* fe, int64_t nPts);

int FramerateEstimaterGetFramerate(FramerateEstimater* fe);

int FramerateEstimaterGetFrameDuration(FramerateEstimater* fe); //* in unit of us.

int FramerateEstimaterSetPlayrate(FramerateEstimater* fe,float rate);

void FramerateEstimaterReset(FramerateEstimater* fe);

#endif
