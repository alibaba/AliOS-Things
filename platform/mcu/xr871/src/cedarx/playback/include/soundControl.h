/*
 * Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * File : soundControl.h
 * Description : soundControl
 * History :
 *
 */

#ifndef SOUND_CONTROL_H
#define SOUND_CONTROL_H
#include "cdx_log.h"
#include "adecoder.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum XAudioTimestretchStretchMode  {
    AUDIO_TIMESTRETCH_STRETCH_DEFAULT            = 0,
    AUDIO_TIMESTRETCH_STRETCH_SPEECH             = 1,
} XAudioTimestretchStretchMode;

typedef enum XAudioTimestretchFallbackMode  {
    XAUDIO_TIMESTRETCH_FALLBACK_CUT_REPEAT     = -1,
    XAUDIO_TIMESTRETCH_FALLBACK_DEFAULT        = 0,
    XAUDIO_TIMESTRETCH_FALLBACK_MUTE           = 1,
    XAUDIO_TIMESTRETCH_FALLBACK_FAIL           = 2,
} XAudioTimestretchFallbackMode;

typedef struct XAudioPlaybackRate {
    float mSpeed;
    float mPitch;
    enum XAudioTimestretchStretchMode  mStretchMode;
    enum XAudioTimestretchFallbackMode mFallbackMode;
}XAudioPlaybackRate;

typedef struct SoundCtrl SoundCtrl;

typedef struct SoundControlOpsS SoundControlOpsT;

struct SoundControlOpsS
{
    void (*destroy)(SoundCtrl* s);

    void (*setFormat)(SoundCtrl* s, CdxPlaybkCfg* cfg);

    int (*start)(SoundCtrl* s);

    int (*stop)(SoundCtrl* s);

    int (*pause)(SoundCtrl* s);

    int (*flush)(SoundCtrl* s, void *block);

    int (*write)(SoundCtrl* s, void* pData, int nDataSize);

    int (*reset)(SoundCtrl* s);

    int (*getCachedTime)(SoundCtrl* s);

    int (*getFrameCount)(SoundCtrl* s);

    int (*setPlaybackRate)(SoundCtrl* s,const XAudioPlaybackRate *rate);

};

struct SoundCtrl
{
    struct SoundControlOpsS* ops;
};

static inline void SoundDeviceDestroy(SoundCtrl* s)
{
    CDX_CHECK(s);
    CDX_CHECK(s->ops);
    CDX_CHECK(s->ops->destroy);
    return s->ops->destroy(s);
}

static inline void SoundDeviceSetFormat(SoundCtrl* s, CdxPlaybkCfg* cfg)
{
    CDX_CHECK(s);
    CDX_CHECK(s->ops);
    CDX_CHECK(s->ops->setFormat);
    return s->ops->setFormat(s, cfg);
}

static inline int SoundDeviceStart(SoundCtrl* s)
{
    CDX_CHECK(s);
    CDX_CHECK(s->ops);
    CDX_CHECK(s->ops->start);
    return s->ops->start(s);
}

static inline int SoundDeviceStop(SoundCtrl* s)
{
    CDX_CHECK(s);
    CDX_CHECK(s->ops);
    CDX_CHECK(s->ops->stop);
    return s->ops->stop(s);
}

static inline int SoundDevicePause(SoundCtrl* s)
{
    CDX_CHECK(s);
    CDX_CHECK(s->ops);
    CDX_CHECK(s->ops->pause);
    return s->ops->pause(s);
}

static inline int SoundDeviceFlush(SoundCtrl* s, void* block)
{
    CDX_CHECK(s);
    CDX_CHECK(s->ops);
    CDX_CHECK(s->ops->flush);
    return s->ops->flush(s, block);
}

static inline int SoundDeviceWrite(SoundCtrl* s, void* pData, int nDataSize)
{
    CDX_CHECK(s);
    CDX_CHECK(s->ops);
    CDX_CHECK(s->ops->write);
    return s->ops->write(s, pData, nDataSize);
}

static inline int SoundDeviceReset(SoundCtrl* s)
{
    CDX_CHECK(s);
    CDX_CHECK(s->ops);
    CDX_CHECK(s->ops->reset);
    return s->ops->reset(s);
}

static inline int SoundDeviceGetCachedTime(SoundCtrl* s)
{
    CDX_CHECK(s);
    CDX_CHECK(s->ops);
    CDX_CHECK(s->ops->getCachedTime);
    return s->ops->getCachedTime(s);
}

static inline int SoundDeviceGetFrameCount(SoundCtrl* s)
{
    CDX_CHECK(s);
    CDX_CHECK(s->ops);
    CDX_CHECK(s->ops->getFrameCount);
    return s->ops->getFrameCount(s);
}

static inline int SoundDeviceSetPlaybackRate(SoundCtrl* s,const XAudioPlaybackRate *rate)
{
    CDX_CHECK(s);
    CDX_CHECK(s->ops);
    return s->ops->setPlaybackRate(s,rate);
}

#ifdef __cplusplus
}
#endif

#endif

