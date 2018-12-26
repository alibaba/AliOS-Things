/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : CdxTsemaphore.h
 * Description : Allwinner Muxer Writer with Thread Operation
 * History :
 *   Author  : Q Wang <eric_wang@allwinnertech.com>
 *   Date    : 2014/12/17
 *   Comment : 创建初始版本，用于V3 CedarX_1.0
 *
 *   Author  : GJ Wu <wuguanjian@allwinnertech.com>
 *   Date    : 2016/04/18
 *   Comment_: 修改移植用于CedarX_2.0
 */

#ifndef __CDX_TSEMAPHORE_H__
#define __CDX_TSEMAPHORE_H__

#include <pthread.h>

typedef struct CdxSem
{
    pthread_cond_t     condition;
    pthread_mutex_t mutex;
    unsigned int     semval;
}CdxSemT;

int CdxSemInit(CdxSemT *tsem, unsigned int val);
void CdxSemDeinit(CdxSemT *tsem);
void CdxSemDown(CdxSemT *tsem);
void CdxSemUp(CdxSemT *tsem);
void CdxSemUpUnique(CdxSemT *tsem);
void CdxSemReset(CdxSemT *tsem);
void CdxSemWait(CdxSemT *tsem);
void CdxSemSignal(CdxSemT *tsem);
unsigned int CdxSemGetVal(CdxSemT *tsem);


#define cdx_mutex_lock(x)    pthread_mutex_lock(x)
#define cdx_mutex_unlock(x)  pthread_mutex_unlock(x)

#define cdx_cond_wait_while_exp(tsem, expression) \
    pthread_mutex_lock(&tsem.mutex); \
    while (expression) { \
        pthread_cond_wait(&tsem.condition, &tsem.mutex); \
    } \
    pthread_mutex_unlock(&tsem.mutex);

#define cdx_cond_wait_if_exp(tsem, expression) \
    pthread_mutex_lock(&tsem.mutex); \
    if (expression) { \
        pthread_cond_wait(&tsem.condition, &tsem.mutex); \
    } \
    pthread_mutex_unlock(&tsem.mutex);

#endif

