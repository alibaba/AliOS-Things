/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "../../SDL_internal.h"

#ifndef SDL_AOS_audio_h_
#define SDL_AOS_audio_h_

#include "sound_pcm.h"

#include "../SDL_sysaudio.h"

/* Hidden "this" pointer for the audio functions */
#define _THIS   SDL_AudioDevice *this

#define NUM_BUFFERS 2

struct SDL_PrivateAudioData
{
    /* The audio device handle */
    snd_pcm_t *pcm_handle;

    /* Raw mixing buffer */
    Uint8 *mixbuf;
    int mixlen;

    /* swizzle function */
    void (*swizzle_func)(_THIS, void *buffer, Uint32 bufferlen);
};

#endif /* SDL_AOS_audio_h_ */

/* vi: set ts=4 sw=4 expandtab: */
