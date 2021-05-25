/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>

#include "uvoice_types.h"

#include "uvoice_os.h"
#include "uvoice_common.h"
#include "uvoice_audio.h"
#include "audio_common.h"
#include "audio_stream.h"
#include "audio_process.h"
#include "audio_aec.h"


#define AEC_REFER_DELAY_MS        10


int audio_out_process(struct out_stream *out, uint8_t *buffer, int nbytes)
{
    if (!out) {
        snd_err("out null !\n");
        return -1;
    }

#ifdef UVOICE_EQ_ENABLE
    equalizer_process(&out->pcm.config, buffer, nbytes);
#endif

    return 0;
}

int audio_out_process_init(struct audio_device *adev, struct out_stream *out)
{
#ifdef UVOICE_EQ_ENABLE
    struct pcm_config *config;
    int period_size;
    int bits;

    if (!adev || !out) {
        snd_err("args null !\n");
        return -1;
    }

    config = &out->pcm.config;
    bits = pcm_format_to_bits(config->format);
    period_size = period_samples_to_bytes(config);

    if (adev->equalizer_enable && !out->eq) {
        out->eq = equalizer_create(period_size,
            config->rate, bits, config->channels);
        if (!out->eq) {
            snd_err("create equalizer failed !\n");
            return -1;
        }
    }
#endif

    return 0;
}

int audio_out_process_deinit(struct out_stream *out)
{
#ifdef UVOICE_EQ_ENABLE
    if (!out) {
        snd_err("out null !\n");
        return -1;
    }

    if (out->eq) {
        equalizer_release(out->eq);
        out->eq = NULL;
    }
#endif
    return 0;
}

int audio_out_conserve(struct audio_device *adev, uint8_t *buffer,
    int nbytes)
{
    struct in_stream *in;

    in = adev->in;
    if (!in)
        goto __exit;

#ifdef UVOICE_AEC_ENABLE
    if (in->state == STREAM_RUNNING) {
        if (in->aec)
            return audio_aec_refer_conserve(in->aec, buffer, nbytes);
    }
#endif

__exit:
    return 0;
}

int audio_in_process(struct in_stream *in, uint8_t *buffer, int nbytes)
{
    if (!in) {
        snd_err("stream null !\n");
        return -1;
    }

    if (!buffer) {
        snd_err("buffer null !\n");
        return -1;
    }

#ifdef UVOICE_AEC_ENABLE
    if (in->aec) {
        if (audio_aec_process(in->aec, buffer, nbytes)) {
            snd_err("aec process failed !\n");
            return -1;
        }
    }
#endif

#ifdef UVOICE_ANC_ENABLE
    if (in->ns) {
        if (noise_suppression_process(in->ns, buffer, nbytes)) {
            snd_err("ns process failed !\n");
            return -1;
        }
    }
#endif

#ifdef UVOICE_AGC_ENABLE
    if (in->agc) {
        if (auto_gain_control_process(in->agc, buffer, nbytes)) {
            snd_err("agc process failed !\n");
            return -1;
        }
    }
#endif

    return 0;
}

int audio_in_process_init(struct audio_device *adev, struct in_stream *in)
{
    struct pcm_config *config;
    int bits;

    if (!adev || !in) {
        snd_err("args null !\n");
        return -1;
    }

#ifdef UVOICE_ANC_ENABLE
    if (adev->noise_suppression) {
        in->ns = snd_zalloc(sizeof(struct noise_suppression), AFM_EXTN);
        if (!in->ns) {
            snd_err("alloc noise supression struct failed !\n");
            return -1;
        }
    }
#endif

#ifdef UVOICE_AGC_ENABLE
    if (adev->auto_gain_control) {
        in->agc = snd_zalloc(sizeof(struct auto_gain_control), AFM_EXTN);
        if (!in->agc) {
            snd_err("alloc auto gain control struct failed !\n");
            if (in->ns) {
                snd_free(in->ns);
                in->ns = NULL;
            }
            return -1;
        }
    }
#endif

    config = &in->pcm.config;
    bits = pcm_format_to_bits(config->format);

#ifdef UVOICE_ANC_ENABLE
    if (in->ns) {
        if (noise_suppression_create(in->ns,
                config->rate, bits, config->period_size, 3)) {
            snd_err("create ns failed !\n");
            audio_in_process_deinit(in);
            return -1;
        }
    }
#endif

#ifdef UVOICE_AGC_ENABLE
    if (in->agc) {
        if (auto_gain_control_create(in->agc,
                config->rate, bits, config->period_size)) {
            snd_err("create agc failed !\n");
            audio_in_process_deinit(in);
            return -1;
        }
    }
#endif

#ifdef UVOICE_AEC_ENABLE
    if (adev->echo_cancellation) {
        if (audio_aec_init(in)) {
            snd_err("init audio aec failed !\n");
            audio_in_process_deinit(in);
            return -1;
        }
    }
#endif

    return 0;
}

int audio_in_process_deinit(struct in_stream *in)
{
    if (!in) {
        snd_err("stream null !\n");
        return -1;
    }

#ifdef UVOICE_AEC_ENABLE
    if (in->aec)
        audio_aec_deinit(in);
#endif

#ifdef UVOICE_ANC_ENABLE
    if (in->ns) {
        noise_suppression_release(in->ns);
        snd_free(in->ns);
        in->ns = NULL;
    }
#endif

#ifdef UVOICE_AGC_ENABLE
    if (in->agc) {
        auto_gain_control_release(in->agc);
        snd_free(in->agc);
        in->agc = NULL;
    }
#endif

    return 0;
}

