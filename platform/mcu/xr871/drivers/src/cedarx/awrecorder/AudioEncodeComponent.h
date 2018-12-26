/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : AudioEncodeComponent.h
 * Description : AudioEncodeComponent
 * History :
 *
 */

#ifndef AUDIO_ENCODE_COMPONENT_H
#define AUDIO_ENCODE_COMPONENT_H

#include "aencoder.h"
#include "awencoder.h"

typedef void* AudioEncodeComp;


enum AUDIOENCODERNOTIFY
{
    AUDIO_ENCODE_NOTIFY_ERROR,
    AUDIO_ENCODE_NOTIFY_CRASH,
    AUDIO_ENCODE_NOTIFY_EOS,
};

typedef int (*AudioEncodeCallback)(void* pUserData, int eMessageId, void* param);
AudioEncodeComp* AudioEncodeCompCreate();
int AudioEncodeCompInit(AudioEncodeComp* p, AudioEncodeConfig* config);
void AudioEncodeCompDestory(AudioEncodeComp* p);
int AudioEncodeCompStart(AudioEncodeComp *p);
int AudioEncodeCompStop(AudioEncodeComp *p);
int AudioEncodeCompReset(AudioEncodeComp* p);
int AudioEncodeCompInputBuffer(AudioEncodeComp *p, AudioInputBuffer *buf);
int AudioEncodeCompRequestBuffer(AudioEncodeComp *p, AudioEncOutBuffer *bufInfo);
int AudioEncodeCompReturnBuffer(AudioEncodeComp *p, AudioEncOutBuffer *bufInfo);
int AudioEncodeCompSetCallback(AudioEncodeComp *p, AudioEncodeCallback notifier, void* pUserData);
#endif
