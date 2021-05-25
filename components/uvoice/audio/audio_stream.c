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
#include "uvoice_event.h"
#include "uvoice_audio.h"
#include "uvoice_common.h"

#include "uvoice_os.h"
#include "uvoice_pcm.h"
#include "audio_common.h"
#include "audio_mixer.h"
#include "audio_vad.h"
#include "audio_stream.h"
#include "audio_process.h"


#ifdef XR871_BOARD_V02
#define OUT_DEVICE_DEFAULT        SND_DEVICE_OUT_SPEAKER_AND_HEADPHONE
#else
#define OUT_DEVICE_DEFAULT        SND_DEVICE_OUT_SPEAKER
#endif
#define IN_DEVICE_DEFAULT        SND_DEVICE_IN_PRIMARY_MIC

#define OUT_VOLUME_DEFAULT        4

#ifdef __os_linux__
#define OUT_PCM_DUMP_PATH        "hal_pcm_out.dump"
#else
#define OUT_PCM_DUMP_PATH        "/sdcard/hal_pcm_out.dump"
#endif


static struct audio_device *g_adev;

int out_volume_set(int volume)
{
    struct audio_device *adev = g_adev;
    static bool first_set = true;

    if (!adev) {
        snd_err("adev null !\n");
        return -1;
    }

    if (volume < VOLUME_LEVEL_MIN || volume > VOLUME_LEVEL_MAX) {
        snd_err("volume level %d invalid !\n", volume);
        return -1;
    }

    if (adev->out_volume != volume || first_set) {
        adev->out_volume = volume;
        first_set = false;
        snd_info("set volume %d\n", adev->out_volume);
        volume_set(adev->out_device, adev->out_volume);
    }
    return 0;
}

int out_volume_get(void)
{
    struct audio_device *adev = g_adev;
    if (!adev) {
        snd_err("adev null !\n");
        return -1;
    }
    return adev->out_volume;
}

int out_volume_range(int *max, int *min)
{
    if (min)
        *min = VOLUME_LEVEL_MIN;
    if (max)
        *max = VOLUME_LEVEL_MAX;
    return 0;
}

int out_device_set(snd_device_t device)
{
    struct audio_device *adev = g_adev;

    if (!adev) {
        snd_err("adev null !\n");
        return -1;
    }

    if (device < SND_DEVICE_OUT_BEGIN || device > SND_DEVICE_IN_END) {
        snd_err("invalid device %d !\n", device);
        return -1;
    }

    adev->fixed_out_device = device;
    return 0;
}

int out_extpa_set(audio_extpa_info_t *info)
{
    struct external_pa_info pa_info;
    pa_info.used = info->used;
    pa_info.active_high = info->active_high;
    pa_info.delay_ms = info->delay_ms;
    pa_info.pin = info->pin;
    return uvoice_extpa_config(&pa_info);
}

int out_stream_reset(struct out_stream *out)
{
    if (!out) {
        snd_err("stream null !\n");
        return -1;
    }

    snd_info("stream reset\n");
    out_stream_stop(out);
    return 0;
}

int out_stream_mute(struct out_stream *out, int mute)
{
    struct audio_device *adev = g_adev;

    if (!adev) {
        snd_err("adev null !\n");
        return -1;
    }

    if (!out) {
        snd_err("stream null !\n");
        return -1;
    }

    os_mutex_lock(out->lock, OS_WAIT_FOREVER);
    out->silent = mute;
    uvoice_dev_mute(&out->pcm, adev->out_device, 1);
    os_mutex_unlock(out->lock);
    snd_debug("mute %d\n", out->silent);
    return 0;
}

int out_stream_period_duration(struct out_stream *out)
{
    struct pcm_config *config;
    int delay;

    if (!out) {
        snd_err("stream null !\n");
        return -1;
    }

    config = &out->pcm.config;

    if (!pcm_rate_valid(config->rate)) {
        snd_err("sample rate invalid !\n");
        return -1;
    }

    delay = config->period_size / (config->rate / 1000);

    delay -= 5;

    return delay > 0 ? delay : 1;
}

bool out_stream_configured(struct out_stream *out)
{
    if (out && out->state != STREAM_STOP)
        return true;
    return false;
}

int out_stream_configure(struct out_stream *out, media_pcminfo_t *pcminfo)
{
    struct audio_device *adev = g_adev;
    struct pcm_device *pcm;
    struct pcm_config *config;

    if (!adev) {
        snd_err("adev null !\n");
        return -1;
    }

    if (!out) {
        snd_err("stream null !\n");
        return -1;
    }

    if (!pcminfo) {
        snd_err("pcminfo null !\n");
        return -1;
    }

    if (!pcm_rate_valid(pcminfo->rate)) {
        snd_err("rate %d invalid !\n", pcminfo->rate);
        return -1;
    }

    if (!pcm_channel_valid(pcminfo->channels) || pcminfo->channels > 2) {
        snd_err("channels %d invalid !\n", pcminfo->channels);
        return -1;
    }

    if (!pcm_bits_valid(pcminfo->bits)) {
        snd_err("bits %d invalid !\n", pcminfo->bits);
        return -1;
    }

    if (pcminfo->frames <= 0) {
        snd_err("period_size %d invalid !\n", pcminfo->frames);
        return -1;
    }

    os_mutex_lock(out->lock, OS_WAIT_FOREVER);
    if (out->state != STREAM_STOP) {
        snd_err("configure failed ! state %d\n",
            out->state);
        os_mutex_unlock(out->lock);
        return -1;
    }

    pcm = &out->pcm;
    config = &pcm->config;

    pcm->dir = PCM_OUT;

    config->rate = pcminfo->rate;
    config->channels = pcminfo->channels;
    config->period_size = pcminfo->frames >= 1024 ? 1024 : 640;
    config->period_count = 4;
    config->format = bits_to_pcm_format(pcminfo->bits);

    if (uvoice_pcm_setup(pcm)) {
        snd_err("pcm setup failed !\n");
        os_mutex_unlock(out->lock);
        return -1;
    }

    if (pcminfo->rate != config->rate)
        pcminfo->rate = config->rate;
    if (pcminfo->channels != config->channels)
        pcminfo->channels = config->channels;
    if (pcminfo->bits != pcm_format_to_bits(config->format))
        pcminfo->bits = pcm_format_to_bits(config->format);

    audio_out_process_init(adev, out);

    out->state = STREAM_SETUP;
    os_mutex_unlock(out->lock);
    snd_debug("stream setup\n");
    return 0;
}

static int out_stream_start(struct out_stream *out)
{
    struct audio_device *adev = g_adev;

    if (!adev) {
        snd_err("adev null !\n");
        return -1;
    }

    if (!out) {
        snd_err("stream null !\n");
        return -1;
    }

    uvoice_dev_mute(&out->pcm, adev->out_device, 0);

    if (adev->fixed_out_device >= SND_DEVICE_OUT_BEGIN &&
        adev->fixed_out_device <= SND_DEVICE_OUT_END)
        device_select(adev, adev->fixed_out_device, true);
    else
        device_select(adev, adev->out_device, true);

    if (uvoice_pcm_open(&out->pcm)) {
        snd_err("pcm open failed !\n");
        return -1;
    }

#ifdef AUDIO_CACHE_ENABLE
    out->buffer_size = period_samples_to_bytes(&out->pcm.config);
    out->buffer = snd_zalloc(out->buffer_size, AFM_EXTN);
    if (!out->buffer) {
        snd_err("alloc buffer failed !\n");
        uvoice_pcm_close(&out->pcm);
        return -1;
    }
    snd_debug("alloc out buffer %d\n", out->buffer_size);
#endif

    out->state = STREAM_START;
    snd_info("stream start\n");
    return 0;
}

int out_stream_write(struct out_stream *out, uint8_t *buffer,
    int nbytes)
{
#ifdef AUDIO_CACHE_ENABLE
    struct audio_device *adev = g_adev;
    int remaining_size = nbytes;
    int avail_size;
    int copy_size;
    int ret;

    if (!adev) {
        snd_err("adev null !\n");
        return -1;
    }

    if (!out) {
        snd_err("stream null !\n");
        return -1;
    }

    os_mutex_lock(out->lock, OS_WAIT_FOREVER);

    if (out->state == STREAM_STOP) {
        snd_info("stream stop\n");
        goto __exit;
    }

    avail_size = remaining_size + out->buffer_dirty_size;

    if (avail_size < out->buffer_size) {
        snd_memcpy(out->buffer + out->buffer_dirty_size,
            buffer, remaining_size);
        out->buffer_dirty_size += remaining_size;
        goto __exit;
    }

    if (out->state == STREAM_SETUP) {
        if (out_stream_start(out)) {
            snd_err("start failed !\n");
            os_mutex_unlock(out->lock);
            return -1;
        }
    }

    while (avail_size >= out->buffer_size) {
        copy_size = MIN(remaining_size,
            out->buffer_size - out->buffer_dirty_size);
        snd_memcpy(out->buffer + out->buffer_dirty_size,
            buffer + nbytes - remaining_size, copy_size);
        remaining_size -= copy_size;

        audio_out_process(out, out->buffer, out->buffer_size);

        if (out->dump) {
            fwrite(out->buffer, out->buffer_size, 1, out->dump);
            if (ferror(out->dump)) {
                snd_err("write dump failed %d!\n", ferror(out->dump));
                os_mutex_unlock(out->lock);
                return -1;
            }
        }

        ret = uvoice_pcm_write(&out->pcm, out->buffer, out->buffer_size);
        if (ret < 0) {
            snd_err("write %d failed %d!\n", out->buffer_size, ret);
            os_mutex_unlock(out->lock);
            return -1;
        }

#ifdef UVOICE_AEC_ENABLE
        audio_out_conserve(adev, out->buffer, out->buffer_size);
#endif

        out->buffer_dirty_size = 0;
        avail_size -= out->buffer_size;
        if (out->state == STREAM_START) {
            out->state = STREAM_RUNNING;
            snd_debug("stream running\n");
        }
    }

    if (remaining_size > 0) {
        snd_memcpy(out->buffer + out->buffer_dirty_size,
            buffer + nbytes - remaining_size, remaining_size);
        out->buffer_dirty_size += remaining_size;
    }

__exit:
    os_mutex_unlock(out->lock);
#else

    struct audio_device *adev = g_adev;
    int ret;

    if (out->state == STREAM_STOP) {
        snd_info("stream stop\n");
        return 0;
    }

    if (out->state == STREAM_SETUP) {
        if (out_stream_start(out)) {
            snd_err("start failed !\n");
            return -1;
        }
    }

    ret = uvoice_pcm_write(&out->pcm, buffer, nbytes);
    if (ret < 0) {
        snd_err("write %d failed %d!\n", nbytes, ret);
        return -1;
    }
#endif
    return 0;
}

int out_stream_silent(struct out_stream *out)
{
#ifdef AUDIO_CACHE_ENABLE
    if (!out) {
        snd_err("stream null !\n");
        return -1;
    }

    os_mutex_lock(out->lock, OS_WAIT_FOREVER);

    if (out->state != STREAM_RUNNING &&
            out->state != STREAM_START)
        goto __exit;

    if (out->buffer_dirty_size > 0) {
        snd_debug("drain %d\n", out->buffer_dirty_size);
        memset(out->buffer + out->buffer_dirty_size, 0,
            out->buffer_size - out->buffer_dirty_size);
        if (out->dump) {
            fwrite(out->buffer, out->buffer_size, 1, out->dump);
            if (ferror(out->dump)) {
                snd_err("write dump failed %d!\n", ferror(out->dump));
                os_mutex_unlock(out->lock);
                return -1;
            }
        }
        uvoice_pcm_write(&out->pcm, out->buffer, out->buffer_size);
        out->buffer_dirty_size = 0;
    } else {
        memset(out->buffer, 0, out->buffer_size);
        uvoice_pcm_write(&out->pcm, out->buffer, out->buffer_size);
    }

__exit:
    os_mutex_unlock(out->lock);
    return 0;
#else
    return uvoice_pcm_silence(&out->pcm);
#endif
}

int out_stream_stop(struct out_stream *out)
{
    struct audio_device *adev = g_adev;

    if (!adev) {
        snd_err("adev null !\n");
        return -1;
    }

    if (!out) {
        snd_err("stream null !\n");
        return -1;
    }

    os_mutex_lock(out->lock, OS_WAIT_FOREVER);
    if (out->state != STREAM_RUNNING &&
            out->state != STREAM_START) {
        snd_debug("stream not start\n");
        goto __exit;
    }

#ifdef AUDIO_CACHE_ENABLE
    if (out->buffer_dirty_size > 0) {
        uvoice_pcm_write(&out->pcm,
            out->buffer, out->buffer_dirty_size);
        out->buffer_dirty_size = 0;
    }
#endif

    if (!out->silent)
        uvoice_pcm_flush(&out->pcm);

    uvoice_dev_mute(&out->pcm, adev->out_device, 1);

    uvoice_pcm_close(&out->pcm);

#ifdef AUDIO_CACHE_ENABLE
    snd_free(out->buffer);
    out->buffer = NULL;
    out->buffer_size = 0;
#endif

__exit:
    out->silent = 0;
    out->state = STREAM_STOP;
    os_mutex_unlock(out->lock);

    snd_info("stream stop\n");
    return 0;
}

struct out_stream *out_stream_create(void)
{
    struct audio_device *adev = g_adev;
    struct out_stream *out;

    if (!adev) {
        snd_err("adev null !\n");
        return NULL;
    }

    out = snd_zalloc(sizeof(struct out_stream), AFM_EXTN);
    if (!out) {
        snd_err("alloc out stream failed !\n");
        return NULL;
    }

    if (adev->out_pcm_dump) {
        snd_debug("open out dump\n");
        out->dump = fopen(OUT_PCM_DUMP_PATH, "wb+");
        if (!out->dump) {
            snd_err("open out dump failed !\n");
            snd_free(out);
            return NULL;
        }
    }

    out->lock = os_mutex_new();
    out->state = STREAM_STOP;

    adev->out = out;

    snd_info("stream create\n");
    return out;
}

int out_stream_release(struct out_stream *out)
{
    struct audio_device *adev = g_adev;

    if (!adev) {
        snd_err("adev null !\n");
        return -1;
    }

    if (!out) {
        snd_err("stream null !\n");
        return -1;
    }

    if (out->state != STREAM_STOP) {
        snd_err("stream not stop !\n");
        return -1;
    }

    if (out->dump) {
        snd_debug("close out dump\n");
        fclose(out->dump);
    }

    audio_out_process_deinit(out);

    os_mutex_free(out->lock);
    snd_free(out);
    adev->out = NULL;

    snd_info("stream release\n");
    return 0;
}

bool in_stream_configured(struct in_stream *in)
{
    if (in && in->state != STREAM_STOP)
        return true;
    return false;
}

int in_stream_configure(struct in_stream *in, media_pcminfo_t *pcminfo)
{
    struct audio_device *adev = g_adev;
    struct pcm_device *pcm;

    if (!adev) {
        snd_err("adev null !\n");
        return -1;
    }

    if (!in) {
        snd_err("stream null !\n");
        return -1;
    }

    if (!pcminfo) {
        snd_err("pcminfo null !\n");
        return -1;
    }

    if (in->state != STREAM_STOP) {
        snd_err("state %d, error !\n", in->state);
        return -1;
    }

    if (!pcm_rate_valid(pcminfo->rate)) {
        snd_err("rate %d invalid !\n", pcminfo->rate);
        return -1;
    }

    if (!pcm_channel_valid(pcminfo->channels)) {
        snd_err("channels %d invalid !\n", pcminfo->channels);
        return -1;
    }

    if (!pcm_bits_valid(pcminfo->bits)) {
        snd_err("bits %d invalid !\n", pcminfo->bits);
        return -1;
    }

    if (pcminfo->frames <= 0) {
        snd_err("frames %d invalid !\n", pcminfo->frames);
        return -1;
    }

    os_mutex_lock(in->lock, OS_WAIT_FOREVER);

    pcm = &in->pcm;
    pcm->dir = PCM_IN;

    pcm->config.rate = pcminfo->rate;
    pcm->config.channels = pcminfo->channels;
    pcm->config.period_size = pcminfo->frames;
    pcm->config.format = bits_to_pcm_format(pcminfo->bits);

    if (uvoice_pcm_setup(pcm)) {
        snd_err("pcm setup failed !\n");
        os_mutex_unlock(in->lock);
        return -1;
    }

    if (pcminfo->rate != pcm->config.rate)
        pcminfo->rate = pcm->config.rate;
    if (pcminfo->channels != pcm->config.channels)
        pcminfo->channels = pcm->config.channels;
    if (pcminfo->bits != pcm_format_to_bits(pcm->config.format))
        pcminfo->bits = pcm_format_to_bits(pcm->config.format);

    if (audio_in_process_init(adev, in)) {
        snd_err("audio process init failed !\n");
        os_mutex_unlock(in->lock);
        return -1;
    }

#ifdef UVOICE_VAD_ENABLE
    if (adev->vad_enable) {
        in->vad = audio_vad_create(pcm->config.rate,
            pcm->config.period_size);
        if (!in->vad) {
            snd_err("create vad failed !\n");
            audio_in_process_deinit(in);
            os_mutex_unlock(in->lock);
            return -1;
        }
    }
#endif

    in->state = STREAM_SETUP;
    os_mutex_unlock(in->lock);
    snd_debug("stream setup\n");
    return 0;
}

static FILE *in_stream_dump;

static int in_stream_start(struct in_stream *in)
{
    struct audio_device *adev = g_adev;
    struct pcm_device *pcm;

    if (!adev) {
        snd_err("adev null !\n");
        return -1;
    }

    if (!in) {
        snd_err("stream null !\n");
        return -1;
    }

    pcm = &in->pcm;
    pcm->dir = PCM_IN;

    if (uvoice_pcm_open(pcm)) {
        snd_err("pcm open failed !\n");
        return -1;
    }

#if 0
    if (in_stream_dump)
        fclose(in_stream_dump);
    in_stream_dump = fopen("/sdcard/in_dump_before_anc.pcm", "wb+");
    if (!in_stream_dump) {
        snd_err("open in stream dump fail !\n");
        uvoice_pcm_close(pcm);
        return -1;
    }
    snd_info("in stream dump open\n");
#endif

    device_select(adev, adev->in_device, true);

    in->state = STREAM_START;

    adev->in = in;

    snd_info("stream start\n");
    return 0;
}

int in_stream_read(struct in_stream *in, uint8_t *buffer,
    int nbytes)
{
    int ret;

    if (!in) {
        snd_err("stream null !\n");
        return -1;
    }

    os_mutex_lock(in->lock, OS_WAIT_FOREVER);
    if (in->state == STREAM_STOP) {
        snd_err("stream stop !\n");
        os_mutex_unlock(in->lock);
        return -1;
    } else if (in->state == STREAM_SETUP) {
        if (in_stream_start(in)) {
            snd_err("start failed !\n");
            os_mutex_unlock(in->lock);
            return -1;
        }
    }

    ret = uvoice_pcm_read(&in->pcm, buffer, nbytes);
    if (ret < 0) {
        snd_err("read %d failed %d!\n", nbytes, ret);
        os_mutex_unlock(in->lock);
        return -1;
    }

    if (in_stream_dump) {
        fwrite(buffer, ret, 1, in_stream_dump);
        if (ferror(in_stream_dump)) {
            snd_err("write in stream dump fail %d!\n",
                ferror(in_stream_dump));
        }
    }

    if (audio_in_process(in, buffer, ret)) {
        snd_err("audio process failed !\n");
        os_mutex_unlock(in->lock);
        return -1;
    }

#ifdef UVOICE_VAD_ENABLE
    if (in->vad) {
        if (audio_vad_process(in->vad, buffer, ret)) {
            snd_err("audio process failed !\n");
            os_mutex_unlock(in->lock);
            return -1;
        }
    }
#endif

    if (in->state == STREAM_START)
        in->state = STREAM_RUNNING;

    os_mutex_unlock(in->lock);
    return ret;
}

int in_stream_stop(struct in_stream *in)
{
    struct audio_device *adev = g_adev;

    if (!adev) {
        snd_err("adev null !\n");
        return -1;
    }

    if (!in) {
        snd_err("stream null !\n");
        return -1;
    }

    os_mutex_lock(in->lock, OS_WAIT_FOREVER);

    if (in->state != STREAM_RUNNING &&
        in->state != STREAM_START) {
        snd_err("stop failed ! state %d\n", in->state);
        os_mutex_unlock(in->lock);
        return -1;
    }

    if (in_stream_dump) {
        fclose(in_stream_dump);
        in_stream_dump = NULL;
        snd_info("in stream dump close\n");
    }

    uvoice_pcm_close(&in->pcm);
    in->state = STREAM_STOP;
    snd_info("stream stop\n");
    os_mutex_unlock(in->lock);
    return 0;
}

struct in_stream *in_stream_create(void)
{
    struct audio_device *adev = g_adev;
    struct in_stream *in;

    if (!adev) {
        snd_err("adev null !\n");
        return NULL;
    }

    in = snd_zalloc(sizeof(struct in_stream), AFM_EXTN);
    if (!in) {
        snd_err("alloc in stream failed !\n");
        return NULL;
    }

    in->lock = os_mutex_new();
    in->state = STREAM_STOP;
    adev->in = in;

    snd_info("stream create\n");
    return in;
}

int in_stream_release(struct in_stream *in)
{
    struct audio_device *adev = g_adev;

    if (!adev) {
        snd_err("adev null !\n");
        return -1;
    }

    if (!in) {
        snd_err("stream null !\n");
        return -1;
    }

#ifdef UVOICE_VAD_ENABLE
    audio_vad_release(in->vad);
#endif

    audio_in_process_deinit(in);

    os_mutex_free(in->lock);
    snd_free(in);
    adev->in = NULL;
    snd_info("stream release\n");
    return 0;
}

int audio_param_set(int key, int value)
{
    struct audio_device *adev = g_adev;

    if (!adev) {
        snd_err("adev null !\n");
        return -1;
    }

    switch (key) {
        case PARAM_KEY_NOISE_SUPPRESSION:
        if (value == PARAM_VAL_ENABLE)
            adev->noise_suppression = 1;
        else if (value == PARAM_VAL_DISABLE)
            adev->noise_suppression = 0;
        break;
        case PARAM_KEY_ECHO_CANCELLATION:
        if (value == PARAM_VAL_ENABLE)
            adev->echo_cancellation = 1;
        else if (value == PARAM_VAL_DISABLE)
            adev->echo_cancellation = 0;
        break;
        case PARAM_KEY_AUTO_GAIN_CONTROL:
        if (value == PARAM_VAL_ENABLE)
            adev->auto_gain_control = 1;
        else if (value == PARAM_VAL_DISABLE)
            adev->auto_gain_control = 0;
        break;
        case PARAM_KEY_VOICE_ACTIVE_DETECT:
        if (value == PARAM_VAL_ENABLE)
            adev->vad_enable = 1;
        else if (value == PARAM_VAL_DISABLE)
            adev->vad_enable = 0;
        break;
        case PARAM_KEY_EFFECT_EQUALIZER:
        if (value == PARAM_VAL_ENABLE)
            adev->equalizer_enable = 1;
        else if (value == PARAM_VAL_DISABLE)
            adev->equalizer_enable = 0;
        break;
        case PARAM_KEY_OUT_PCM_DUMP:
        if (value == PARAM_VAL_ENABLE)
            adev->out_pcm_dump = 1;
        else if (value == PARAM_VAL_DISABLE)
            adev->out_pcm_dump = 0;
        break;
        default:
        break;
    }

    return 0;
}

int audio_pcm_notify(pcm_message_t msg)
{
    switch (msg) {
        case PCM_MSG_TX_WAITING:
        break;
        case PCM_MSG_TX_UNDERRUN:
        snd_warn("tx underrun\n");
        break;
        case PCM_MSG_TX_OVERRUN:
        snd_warn("tx overrun\n");
        break;
        case PCM_MSG_RX_UNDERRUN:
        snd_warn("rx underrun\n");
        break;
        case PCM_MSG_RX_OVERRUN:
        snd_warn("rx overrun\n");
        break;
        default:
        break;
    }
    return 0;
}

static void audio_sw_event_handle(uvoice_event_t *event, void *data)
{
    struct audio_device *adev = (struct audio_device *)data;

    if (!adev) {
        snd_err("adev null !\n");
        return;
    }

    if (event->type != UVOICE_EV_SW) {
        snd_warn("event type not match\n");
        return;
    }

    if (event->code == UVOICE_CODE_HEADPHONE) {
        if (event->value == 1)
            device_select(adev, SND_DEVICE_OUT_HEADPHONE, false);
        else if (event->value == 0)
            device_select(adev, SND_DEVICE_OUT_SPEAKER, false);

        if (!adev->in_device_fix)
            device_select(adev, SND_DEVICE_IN_PRIMARY_MIC, false);
    } else if (event->code == UVOICE_CODE_HEADSET) {
        if (event->value == 1) {
            device_select(adev, SND_DEVICE_OUT_HEADSET, false);
            if (!adev->in_device_fix)
                device_select(adev, SND_DEVICE_IN_HEADSET_MIC, false);
        } else if (event->value == 0) {
            device_select(adev, SND_DEVICE_OUT_SPEAKER, false);
            if (!adev->in_device_fix)
                device_select(adev, SND_DEVICE_IN_PRIMARY_MIC, false);
        }
    }
}

int audio_device_init(void)
{
    struct audio_device *adev;
    int ret;

    adev = snd_zalloc(sizeof(struct audio_device), AFM_EXTN);
    if (!adev) {
        snd_err("alloc audio device failed !\n");
        return -1;
    }

    adev->out_device = OUT_DEVICE_DEFAULT;
    adev->in_device = IN_DEVICE_DEFAULT;
    adev->in_device_fix = 1;

    ret = volume_get(adev->out_device);
    if (ret < 0) {
        adev->out_volume = OUT_VOLUME_DEFAULT;
        snd_debug("default volume %d\n", adev->out_volume);
    } else {
        adev->out_volume = ret;
    }

    g_adev = adev;
    uvoice_event_register(UVOICE_EV_SW, audio_sw_event_handle, adev);

    uvoice_pcm_init();
    return 0;
}

int audio_device_deinit(void)
{
    struct audio_device *adev = g_adev;

    if (!adev) {
        snd_err("adev null !\n");
        return -1;
    }

    uvoice_event_unregister(UVOICE_EV_SW, audio_sw_event_handle, NULL);
    snd_free(adev);
    g_adev = NULL;

    return 0;
}

