/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : baseComponent.h
 * Description : base component
 * History :
 *     Date     : 2016/08/03
 *     Comment  : first version
 */

#ifndef BASE_COMPONENT_H
#define BASE_COMPONENT_H

#include <AwMessageQueue.h>
//#include <semaphore.h>
#include "pthread.h"
#include <stdint.h>

struct AwMessage {
    AWMESSAGE_COMMON_MEMBERS

    sem_t *replySem;
    int *result;
    union {
        int64_t seekTime;
        void *opaque;
        int64_t int64Value;
    };
};

typedef struct BaseMsgHandler {
    msgHandlerT start;
    msgHandlerT stop;
    msgHandlerT pause;
    msgHandlerT reset;
    msgHandlerT setEos;
    msgHandlerT quit;
    union {
        msgHandlerT continueWork;
        msgHandlerT decode;
        msgHandlerT encode;
        msgHandlerT mux;
        msgHandlerT render;
    };
} BaseMsgHandler;

enum MESSAGE_ID {
    MESSAGE_ID_START = 0,
    MESSAGE_ID_STOP = 1,
    MESSAGE_ID_PAUSE = 2,
    MESSAGE_ID_QUIT = 3,
    MESSAGE_ID_RESET = 4,
    MESSAGE_ID_EOS = 5,

    /* continue decoder/encoder/mux/render */
    MESSAGE_ID_CONTINUE = 6,

    MESSAGE_ID_SET_WINDOW,
    MESSAGE_ID_SET_3D_MODE,
    MESSAGE_ID_SET_AUDIO_SINK,
    MESSAGE_ID_SET_VIDEO_HIDE,
    MESSAGE_ID_SET_HOLD_LAST_PICTURE,
    MESSAGE_ID_SET_TIMESHIFT,
    MESSAGE_ID_SET_LAYER_INFO,
    MESSAGE_ID_SET_SUB_RENDER,
    MESSAGE_ID_SET_DI,
};

typedef struct BaseCompCtx {
    /**
     * It should point to a string literal.
     */
    const char *name;

    /**
     * This mq should be initialized before call BaseCompInit()
     */
    AwMessageQueue *mq;

    /* continueWork doesn't need a reply */
    sem_t replySem[MESSAGE_ID_EOS + 1];
    BaseMsgHandler handler;
} BaseCompCtx;

int BaseCompInit(BaseCompCtx *p, const char *name,
                AwMessageQueue *mq, BaseMsgHandler *handler);

void BaseCompDestroy(BaseCompCtx *p);

typedef void (*task_t)(void *);

int BaseCompStart(BaseCompCtx *p, task_t afterPostBeforeWait, void *arg);

int BaseCompStop(BaseCompCtx *p, task_t afterPostBeforeWait, void *arg);

int BaseCompPause(BaseCompCtx *p, task_t afterPostBeforeWait, void *arg);

int BaseCompReset(BaseCompCtx *p, int64_t nSeekTime,
        task_t afterPostBeforeWait, void *arg);

int BaseCompSetEos(BaseCompCtx *p, task_t afterPostBeforeWait, void *arg);

int BaseCompQuit(BaseCompCtx *p, task_t afterPostBeforeWait, void *arg);

int BaseCompContinue(BaseCompCtx *p);

#endif
