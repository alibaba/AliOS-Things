/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2020 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "../../SDL_internal.h"

#if SDL_AUDIO_DRIVER_ALIOS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#include "SDL_audio.h"
#include "SDL_error.h"
#include "SDL_timer.h"
#include "../SDL_audio_c.h"
#include "../SDL_audiodev_c.h"
#include "../SDL_sysaudio.h"
#include "SDL_alios_audio.h"
#include "ulog/ulog.h"

#define LOG_TAG  "[SDL_alios]"
#define ALIOS_AUDIO_DRIVER_NAME         "alios_sound"

static SDL_atomic_t ALSA_hotplug_shutdown;
static SDL_Thread *ALSA_hotplug_thread;
static int fBusyFlag = 0;
hint_list_t node;

static int SDLCALL
ALSA_HotplugThread(void *arg)
{
    int ret = -1;
    SDL_sem *first_run_semaphore = (SDL_sem *) arg;
    hint_list_t *hints = NULL;
    Uint32 ticks;

    SDL_SetThreadPriority(SDL_THREAD_PRIORITY_LOW);
    while (!SDL_AtomicGet(&ALSA_hotplug_shutdown)) {
        if(!hints) {
            LOGD(LOG_TAG, "%s:%d, search sound card ...", __func__, __LINE__);
            //ret = snd_device_name_hint(-1, "pcm", node);
            if(1) {
                hints = (hint_list_t *)&node;
                strncpy(hints->name, "/dev/pcmC0D0p", sizeof(hints->name));
                if(hints) {
                    LOGD(LOG_TAG, "%s:%d, SDL_AddAudioDevice %s", __func__, __LINE__, hints->name);
                    SDL_AddAudioDevice(0, hints->name, (void *)hints);
                    SDL_SemPost(first_run_semaphore);
                }
            }
        }

        /* Block awhile before checking again, unless we're told to stop. */
        ticks = SDL_GetTicks() + 5000;
        while (!SDL_AtomicGet(&ALSA_hotplug_shutdown) && !SDL_TICKS_PASSED(SDL_GetTicks(), ticks)) {
            SDL_Delay(1000);
        }
    }
    return 0;
}

static void
AOS_DetectDevices(void)
{
    /* Start the device detection thread here, wait for an initial iteration to complete. */
    SDL_sem *semaphore = SDL_CreateSemaphore(0);
    if (!semaphore) {
        return;  /* oh well. */
    }

    SDL_AtomicSet(&ALSA_hotplug_shutdown, 0);

    ALSA_hotplug_thread = SDL_CreateThread(ALSA_HotplugThread, "SDLHotplugALSA", semaphore);
    if (ALSA_hotplug_thread) {
        SDL_SemWait(semaphore);  /* wait for the first iteration to finish. */
    }

    SDL_DestroySemaphore(semaphore);
}


static int
AOS_OpenDevice(_THIS, void *handle, const char *devname, int iscapture)
{
    int status = 0;
    snd_pcm_t *pcm_handle = NULL;
    snd_pcm_hw_params_t *hwparams = NULL;
    snd_pcm_sw_params_t *swparams = NULL;
    snd_pcm_format_t format = SNDRV_PCM_FORMAT_ALL;
    SDL_AudioFormat test_format = 0;
    unsigned int rate = 0;
    unsigned int channels = 0;

    this->hidden = (struct SDL_PrivateAudioData *) SDL_malloc(sizeof(*this->hidden));
    if (this->hidden == NULL) {
        return SDL_OutOfMemory();
    }
    SDL_zerop(this->hidden);

    status = snd_pcm_open(&pcm_handle, "hw", iscapture ? SND_PCM_STREAM_CAPTURE : SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);
    if (status < 0) {
        LOGE(LOG_TAG, "%s:%d, ALSA: Couldn't open audio device", __func__, __LINE__);
        return -1;
    }
    this->hidden->pcm_handle = pcm_handle;

    /* Try for a closest match on audio format */
    status = -1;
    for (test_format = SDL_FirstAudioFormat(this->spec.format); test_format && (status < 0);) {
        status = 0;             /* if we can't support a format, it'll become -1. */
        switch (test_format) {
        case AUDIO_S8:
            format = SNDRV_PCM_FORMAT_S8;
            break;
        case AUDIO_S16LSB:
            format = SNDRV_PCM_FORMAT_S16_LE;
            break;
        case AUDIO_S32LSB:
            format = SNDRV_PCM_FORMAT_S32_LE;
            break;
        default:
            status = -1;
            break;
        }
        if (status < 0) {
            test_format = SDL_NextAudioFormat();
        }
    }
    if (status < 0) {
        LOGE(LOG_TAG, "%s:%d, status %d", __func__, __LINE__, status);
        return -1;
    }
    this->spec.format = test_format;


    /* Set the number of channels */
    channels = this->spec.channels;

    /* Set the audio rate */
    rate = this->spec.freq;

    /* Calculate the final parameters for this audio specification */
    SDL_CalculateAudioSpec(&this->spec);

    /* Allocate mixing buffer */
    if (!iscapture) {
        this->hidden->mixlen = this->spec.size;
        this->hidden->mixbuf = (Uint8 *) SDL_malloc(this->hidden->mixlen);
        if (this->hidden->mixbuf == NULL) {
            return SDL_OutOfMemory();
        }
        SDL_memset(this->hidden->mixbuf, this->spec.silence, this->hidden->mixlen);
    }

    snd_pcm_set_params(this->hidden->pcm_handle,
                       format,
                       SNDRV_PCM_ACCESS_RW_NONINTERLEAVED,
                       channels,
                       rate,
                       0,
                       0);
    snd_pcm_prepare(this->hidden->pcm_handle);
    return 0;
}

/* This function waits until it is possible to write a full sound buffer */
static void
AOS_WaitDevice(_THIS)
{
    //LOGD(LOG_TAG, "%s:%d", __func__, __LINE__);
}

static Uint8 *
AOS_GetDeviceBuf(_THIS)
{
    //LOGD(LOG_TAG, "%s:%d", __func__, __LINE__);
    return (this->hidden->mixbuf);
}

static void
ALSA_PlayDevice(_THIS)
{
    const Uint8 *sample_buf = (const Uint8 *) this->hidden->mixbuf;
    const int frame_size = ((SDL_AUDIO_BITSIZE(this->spec.format)) / 8) * this->spec.channels;
    snd_pcm_uframes_t frames_left = ((snd_pcm_uframes_t) this->spec.samples);

    if(this->hidden->swizzle_func)
        this->hidden->swizzle_func(this, this->hidden->mixbuf, frames_left);

    if(0 == fBusyFlag) {
        fBusyFlag = 1;
        snd_pcm_start(this->hidden->pcm_handle);
    }
    while ( frames_left > 0 && SDL_AtomicGet(&this->enabled) ) {
        int status = snd_pcm_writei(this->hidden->pcm_handle, sample_buf, frames_left);
        if (status > 0) {
            sample_buf += status * frame_size;
            frames_left -= status;
            //Uint32 delay = (frames_left / 2 * 1000) / this->spec.freq;
            //SDL_Delay(delay);
        }
    }
}

static void
AOS_Deinitialize(void)
{
    if (ALSA_hotplug_thread != NULL) {
        SDL_AtomicSet(&ALSA_hotplug_shutdown, 1);
        SDL_WaitThread(ALSA_hotplug_thread, NULL);
        ALSA_hotplug_thread = NULL;
    }
}

static void
AOS_CloseDevice(_THIS)
{
    if (this->hidden->pcm_handle) {
        /* Wait for the submitted audio to drain
           ALSA_snd_pcm_drop() can hang, so don't use that.
         */
        Uint32 delay = ((this->spec.samples * 1000) / this->spec.freq) * 2;
        SDL_Delay(delay);

        snd_pcm_drop(this->hidden->pcm_handle);
        snd_pcm_close(this->hidden->pcm_handle);
    }
    SDL_free(this->hidden->mixbuf);
    SDL_free(this->hidden);
    fBusyFlag = 0;
}

static int
AOSAUDIO_Init(SDL_AudioDriverImpl * impl)
{
    /* Set the function pointers */
    impl->DetectDevices = AOS_DetectDevices;
    impl->OpenDevice = AOS_OpenDevice;
    impl->WaitDevice = AOS_WaitDevice;
    impl->GetDeviceBuf = AOS_GetDeviceBuf;
    impl->PlayDevice = ALSA_PlayDevice;
    impl->CloseDevice = AOS_CloseDevice;
    impl->Deinitialize = AOS_Deinitialize;
    //impl->CaptureFromDevice = ALSA_CaptureFromDevice;
    //impl->FlushCapture = ALSA_FlushCapture;

    impl->HasCaptureSupport = SDL_FALSE;

    return 1;   /* this audio target is available. */
}

AudioBootStrap AOSAUDIO_bootstrap = {
    "aos", "AOS audio driver", AOSAUDIO_Init, 0
};

 /* SDL_AUDI */

#endif /* SDL_AUDIO_DRIVER_PSP */

/* vi: set ts=4 sw=4 expandtab: */
