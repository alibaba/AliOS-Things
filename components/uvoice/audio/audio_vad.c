/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#include "uvoice_types.h"

#include "uvoice_os.h"
#include "uvoice_types.h"
#include "uvoice_event.h"
#include "uvoice_common.h"
#include "uvoice_audio.h"

#include "audio_common.h"
#include "audio_stream.h"
#include "audio_vad.h"


#define VAD_END_DEBOUNCE_TIME_MS        990 /* muliti of 30 */


int audio_vad_process(struct voice_active_detect *vad,
    const uint8_t *buffer, int nbytes)
{
    int ret;

    if (!vad) {
        snd_err("vad null !\n");
        return -1;
    }

    ret = vad_process(vad, buffer, nbytes);
    if (ret < 0) {
        snd_err("vad process error %d !\n", ret);
        return -1;
    }

    if (ret == VAD_STAT_SPEECH) {
        if (vad->state == VAD_STAT_NOISE) {
            vad->state = VAD_STAT_SPEECH;
            uvoice_event_post(UVOICE_EV_ST,
                UVOICE_CODE_VAD_START, 0);
        }
        if (vad->end_count > 0)
            vad->end_count = 0;
    } else if (ret == VAD_STAT_NOISE) {
        if (vad->state == VAD_STAT_SPEECH) {
            if (vad->end_count++ >= vad->end_target &&
                os_current_time() - vad->start_time >= 2000) {
                vad->state = VAD_STAT_NOISE;
                vad->end_count = 0;
                uvoice_event_post(UVOICE_EV_ST,
                    UVOICE_CODE_VAD_END, 0);
            }
        }
    }

    return 0;
}

struct voice_active_detect *audio_vad_create(int rate, int period_size)
{
    struct voice_active_detect *vad;
    int proc_samples;

    vad = snd_zalloc(sizeof(struct voice_active_detect), AFM_EXTN);
    if (!vad) {
        snd_err("alloc vad struct failed !\n");
        return NULL;
    }

    proc_samples = MIN((rate / 100) * 3, period_size);

    vad->state = VAD_STAT_NOISE;
    vad->end_target = VAD_END_DEBOUNCE_TIME_MS /
        ((proc_samples * 1000) / rate);

    if (vad_create(vad, rate, proc_samples, 0)) {
        snd_err("create vad failed !\n");
        snd_free(vad);
        return NULL;
    }

    vad->start_time = os_current_time();

    snd_debug("vad create, end target %d\n",
        vad->end_target);
    return vad;
}

int audio_vad_release(struct voice_active_detect *vad)
{
    if (vad) {
        vad_release(vad);
        snd_free(vad);
        snd_debug("vad release\n");
    }

    return 0;
}

