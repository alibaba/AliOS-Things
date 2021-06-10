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
#include <errno.h>

#include "uvoice_types.h"
#include "uvoice_event.h"
#include "uvoice_player.h"

#include "uvoice_os.h"
#include "uvoice_config.h"
#include "uvoice_list.h"
#include "uvoice_audio.h"
#include "uvoice_common.h"
#include "uvoice_ringbuffer.h"
#include "uvoice_play.h"
#include "uvoice_resampler.h"

#include "audio_common.h"
#include "audio_stream.h"


#define PLAYER_VOLUME_LEVEL_KEY                "player_volume_level"
#define PLAYER_VOLUME_LEVEL_DEFAULT            40

#define FADE_OUT_PERIOD_DEFAULT                10
#define FADE_IN_PERIOD_DEFAULT                 20
#define PLAYER_FADE_PROC_ENABLE                1

#ifdef __os_linux__
#define PLAYER_SW_VOLUME_ENABLE                1
#else
#define PLAYER_SW_VOLUME_ENABLE                0
#endif

#define PLAYER_TRACE(fmt, ...)    printf("%s: "fmt, __func__, ##__VA_ARGS__)

static uvoice_player_t *g_mplayer;
static int player_clr_source(void);

int http_cache_config(cache_config_t *config);

static inline bool player_can_set_source(player_t *player)
{
    if (player->state == PLAYER_STAT_IDLE ||
        player->state == PLAYER_STAT_COMPLETE)
        return true;
    return false;
}

static inline bool player_can_clr_source(player_t *player)
{
    if (player->state == PLAYER_STAT_STOP ||
        player->state == PLAYER_STAT_READY)
        return true;
    return false;
}

static inline bool player_can_set_config(player_t *player)
{
    if (!player->stream_flowing &&
        (player->state == PLAYER_STAT_READY ||
        player->state == PLAYER_STAT_COMPLETE))
        return true;

    if (player->stream_mgr &&
        player->stream_mgr->state == STREAM_MGR_STAT_READY)
        return true;

    return false;
}

static inline bool player_can_pause(player_t *player)
{
    if (player->state == PLAYER_STAT_RUNNING ||
        player->state == PLAYER_STAT_COMPLETE)
        return true;
    return false;
}

static inline bool player_can_resume(player_t *player)
{
    if (player->state == PLAYER_STAT_PAUSED ||
        player->state == PLAYER_STAT_RUNNING ||
        player->state == PLAYER_STAT_COMPLETE)
        return true;
    return false;
}

static inline bool player_can_complete(player_t *player)
{
    if (player->state == PLAYER_STAT_RUNNING)
        return true;
    return false;
}

static inline bool player_can_seek(player_t *player)
{
    if (player->state == PLAYER_STAT_RUNNING ||
        player->state == PLAYER_STAT_PAUSED)
        return true;
    return false;
}

static inline bool player_can_download(player_t *player)
{
    if (player->state == PLAYER_STAT_RUNNING ||
        player->state == PLAYER_STAT_PAUSED)
        return true;
    return false;
}

static inline bool player_can_wait_complete(player_t *player)
{
    if (player->state == PLAYER_STAT_COMPLETE &&
        !player->complete)
        return true;
    if (player->state == PLAYER_STAT_RUNNING)
        return true;
    return false;
}

static inline bool player_can_stop(player_t *player)
{
    if (player->state == PLAYER_STAT_RUNNING ||
        player->state == PLAYER_STAT_PAUSED ||
        player->state == PLAYER_STAT_COMPLETE)
        return true;
    return false;
}

static inline bool player_can_set_stream(player_t *player)
{
    if (player->state == PLAYER_STAT_IDLE ||
        player->state == PLAYER_STAT_PAUSED ||
        player->state == PLAYER_STAT_STOP)
        return true;
    return false;
}

static int player_state_post(player_t *player, player_state_t state)
{
    return uvoice_event_post(UVOICE_EV_PLAYER,
        UVOICE_CODE_PLAYER_STATE, state);
}

static int player_pcminfo_update(player_t *player,
    media_pcminfo_t *info)
{
    media_pcminfo_t *pcminfo;
    int changed = 0;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    if (!info) {
        M_LOGE("info null !\n");
        return -1;
    }

    if (player->stream_flowing)
        pcminfo = &player->stream_mgr->pcm_info;
    else
        pcminfo = &player->pcm_info;

    if (pcm_rate_valid(info->rate) &&
        pcminfo->rate != info->rate) {
        if (pcm_rate_valid(pcminfo->rate))
            changed = 1;
        pcminfo->rate = info->rate;

        player->fade.out_scope = (pcminfo->rate / 100) *
            player->fade.out_period_ms;
        player->fade.in_scope = (pcminfo->rate / 100) *
            player->fade.in_period_ms;
    }

    if (pcm_channel_valid(info->channels) &&
            pcminfo->channels != info->channels) {
        if (pcm_channel_valid(pcminfo->channels))
            changed = 1;
        pcminfo->channels = info->channels;
    }

    if (pcm_bits_valid(info->bits) &&
            pcminfo->bits != info->bits) {
        if (pcm_bits_valid(pcminfo->bits))
            changed = 1;
        pcminfo->bits = info->bits;
    }

    if (info->frames > 0 &&
            pcminfo->frames != info->frames)
        pcminfo->frames = info->frames;

    M_LOGD("rate %d channels %d bits %d frames %d changed %d\n",
        pcminfo->rate,
        pcminfo->channels,
        pcminfo->bits, pcminfo->frames, changed);
    return changed;
}

static int player_mediainfo_update(player_t *player,
    media_info_t *info)
{
    media_info_t *media_info;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    if (!info) {
        M_LOGE("info null !\n");
        return -1;
    }

    media_info = player->media_info;
    if (!media_info) {
        M_LOGE("player media info null !\n");
        return -1;
    }

    if (strlen(info->name) > 0)
        memcpy(media_info->name,
            info->name, strlen(info->name) + 1);

    if (strlen(info->author) > 0)
        memcpy(media_info->author,
            info->author, strlen(info->author) + 1);

    if (strlen(info->album) > 0)
        memcpy(media_info->album,
            info->album, strlen(info->album) + 1);

    if (info->media_size > 0)
        media_info->media_size = info->media_size;

    if (info->duration > 0)
        media_info->duration = info->duration;

    if (media_info->media_size > 0 && media_info->duration > 0)
        media_info->bitrate = (media_info->media_size /
            media_info->duration) * 8;

    if (info->bitrate > 0)
        media_info->bitrate = info->bitrate;

    if (media_info->bitrate > 0 && media_info->media_size > 0)
        player_state_post(player, PLAYER_STAT_MEDIA_INFO);

    M_LOGD("bitrate %d media_size %d\n",
        media_info->bitrate, media_info->media_size);
    return 0;
}

static int player_message_handle(void *priv,
    player_message_t msg, void *arg)
{
    player_t *player = (player_t *)priv;
    int ret;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    switch (msg) {
        case PLAYER_MSG_PCM_INFO:
        ret = player_pcminfo_update(player, arg);
        if (ret < 0) {
            M_LOGE("update pcm info failed !\n");
            return -1;
        }
        player->pcm_reset = !!ret;
        break;
        case PLAYER_MSG_MEDIA_INFO:
        if (!player->stream_flowing) {
            if (player_mediainfo_update(player, arg)) {
                M_LOGE("update media info failed !\n");
                return -1;
            }
        }
        break;
        case PLAYER_MSG_SEEK_DONE:
        if (player->seek.state == SEEK_START)
            player->seek.state = SEEK_COMPLETE;
        player->consumed_len += (int)arg;
        player_state_post(player, PLAYER_STAT_SEEK_CPLT);
        break;
        case PLAYER_MSG_LOAD_BLOCK:
        os_mutex_lock(player->lock, OS_WAIT_FOREVER);
        player->blocking = (int)arg;
        os_mutex_unlock(player->lock);
        break;
        default:
        break;
    }

    return 0;
}

static int player_ampl_coeff(player_t *player, uint8_t *buffer,
    int nbytes)
{
    media_pcminfo_t *pcminfo;
    int coeff = 0;
    int step = 0;
    int channels;
    static int curr_coeff = -1;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    if (player->stream_flowing)
        pcminfo = &player->stream_mgr->pcm_info;
    else
        pcminfo = &player->pcm_info;

    channels = pcminfo->channels;

    os_mutex_lock(player->vol_lock, OS_WAIT_FOREVER);
    coeff = player->vol_level * 100;
    if (player->vol_level <= 30 && player->vol_level > 0)
        coeff -= 80;
    else if (player->vol_level <= 50)
        coeff -= 50;
    else if (player->vol_level <= 80)
        coeff -= 20;
    os_mutex_unlock(player->vol_lock);

    if (coeff != curr_coeff) {
        if (curr_coeff != -1)
            step = coeff > curr_coeff ? 1 : -1;
        else
            curr_coeff = coeff;
    }

    if (pcminfo->bits == 32 || pcminfo->bits == 24) {
        int *temp_buffer = (int *)buffer;
        double temp_value;
        int sample_count = nbytes / 4;
        for (int sample_idx = 0; sample_idx < sample_count;
            sample_idx += channels) {
            if (curr_coeff != coeff)
                curr_coeff += step;
            for (int channel_idx = 0; channel_idx < channels;
                channel_idx++) {
                temp_value = temp_buffer[sample_idx + channel_idx];
                temp_value *= curr_coeff;
                temp_value /= 10000;
                temp_buffer[sample_idx + channel_idx] = (int)temp_value;
            }
        }
    } else if (pcminfo->bits == 16) {
        short *temp_buffer = (short *)buffer;
        double temp_value;
        int sample_count = nbytes / 2;
        for (int sample_idx = 0; sample_idx < sample_count;
            sample_idx += channels) {
            if (curr_coeff != coeff)
                curr_coeff += step;
            for (int channel_idx = 0; channel_idx < channels;
                channel_idx++) {
                temp_value = temp_buffer[sample_idx + channel_idx];
                temp_value *= curr_coeff;
                temp_value /= 10000;
                temp_buffer[sample_idx + channel_idx] = (short)temp_value;
            }
        }
    } else if (pcminfo->bits == 8) {
        double temp_value;
        for (int sample_idx = 0; sample_idx < nbytes;
            sample_idx += channels) {
            if (curr_coeff != coeff)
                curr_coeff += step;
            for (int channel_idx = 0; channel_idx < channels;
                channel_idx++) {
                temp_value = buffer[sample_idx + channel_idx];
                temp_value *= curr_coeff;
                temp_value /= 10000;
                buffer[sample_idx + channel_idx] = (char)temp_value;
            }
        }
    }

    return 0;
}

static int player_fade_config_unlock(player_t *player,
    int out_period, int in_period)
{
    fade_process_t *fade;
    media_pcminfo_t *pcminfo;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    fade = &player->fade;

    if (in_period != FADE_IN_PERIOD_DEFAULT ||
        out_period != FADE_OUT_PERIOD_DEFAULT)
        fade->reset = 1;

    if (fade->in_period_ms == in_period &&
        fade->out_period_ms == out_period) {
        M_LOGD("fade period no change\n");
        return 0;
    }

    if (player->stream_flowing)
        pcminfo = &player->stream_mgr->pcm_info;
    else
        pcminfo = &player->pcm_info;

    fade->in_period_ms = in_period;
    fade->out_period_ms = out_period;

    fade->out_scope = (pcminfo->rate / 100) *
            fade->out_period_ms;
    fade->in_scope = (pcminfo->rate / 100) *
            fade->in_period_ms;

    return 0;
}

static int player_fade_config(player_t *player, int out_period,
    int in_period)
{
    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    os_mutex_lock(player->fade.lock, OS_WAIT_FOREVER);

    if (!player->fade_enable) {
        M_LOGW("fade process not enable\n");
        os_mutex_unlock(player->fade.lock);
        return -1;
    }

    player_fade_config_unlock(player, out_period, in_period);

    os_mutex_unlock(player->fade.lock);
    return 0;
}

static int player_fade_process(player_t *player,
    uint8_t *buffer, int nbytes)
{
    media_pcminfo_t *pcminfo;
    fade_process_t *fade;

    fade = &player->fade;

    os_mutex_lock(fade->lock, OS_WAIT_FOREVER);

    if (player->stream_flowing)
        pcminfo = &player->stream_mgr->pcm_info;
    else
        pcminfo = &player->pcm_info;

    if (fade->state == FADE_OUT) {
        if (fade->out_scope > 0) {
            if (pcminfo->bits == 16)
                fade_out_proc_16bit(buffer, nbytes,
                    &fade->pos,
                    fade->out_scope, pcminfo->channels);
            else if (pcminfo->bits == 8)
                fade_out_proc_8bit(buffer, nbytes,
                    &fade->pos,
                    fade->out_scope, pcminfo->channels);
            else
                fade->pos = 0;
            if (fade->pos == 0) {
                M_LOGD("fade out end\n");
                fade->state = FADE_OUT_END;
                if (fade->reset) {
                    player_fade_config_unlock(player,
                        FADE_OUT_PERIOD_DEFAULT, FADE_IN_PERIOD_DEFAULT);
                    fade->reset = 0;
                }
            }
        } else {
            fade->state = FADE_OUT_END;
            M_LOGD("fade out end\n");
            if (fade->reset) {
                player_fade_config_unlock(player,
                    FADE_OUT_PERIOD_DEFAULT, FADE_IN_PERIOD_DEFAULT);
                fade->reset = 0;
            }
        }
    } else if (fade->state == FADE_IN) {
        if (fade->in_scope > 0) {
            if (pcminfo->bits == 16)
                fade_in_proc_16bit(buffer, nbytes, &fade->pos,
                    fade->in_scope, pcminfo->channels);
            else if (pcminfo->bits == 8)
                fade_in_proc_8bit(buffer, nbytes, &fade->pos,
                    fade->in_scope, pcminfo->channels);
            else
                fade->pos = fade->in_scope;
            if (fade->pos >= fade->in_scope) {
                M_LOGD("fade in end\n");
                fade->state = FADE_NOP;
                if (fade->reset) {
                    player_fade_config_unlock(player,
                        FADE_OUT_PERIOD_DEFAULT, FADE_IN_PERIOD_DEFAULT);
                    fade->reset = 0;
                }
            }
        } else {
            fade->state = FADE_NOP;
            M_LOGD("fade in end\n");
            if (fade->reset) {
                player_fade_config_unlock(player,
                    FADE_OUT_PERIOD_DEFAULT, FADE_IN_PERIOD_DEFAULT);
                fade->reset = 0;
            }
        }
    }

    os_mutex_unlock(fade->lock);
    return 0;
}

static int player_data_detect(player_t *player,
    uint8_t *buffer, int nbytes)
{
    int sample_count = nbytes / sizeof(short);
    short *pdata = (short *)buffer;
    short ampl_prev = 0;
    short ampl_curr = 0;
    short ampl_next = 0;
    int i;

    for (i = 0; i < sample_count; i += 2) {
        ampl_curr = pdata[i];
        ampl_next = i < sample_count - 2 ? pdata[i + 2] : -1;
        if (ampl_curr == 0 && ampl_prev != 0 && ampl_next != 0)
            M_LOGI("%d, %d, %d\n", ampl_prev, ampl_curr, ampl_next);
        ampl_prev = ampl_curr;
    }
    return 0;
}

static int player_data_format_init(player_t *player,
    int src_ch, int dst_ch, int src_bits, int dst_bits)
{
    int dst;
    int src;

    if (src_ch == dst_ch && src_bits == dst_bits) {
        if (player->convertor) {
            snd_free(player->convertor);
            player->convertor = NULL;
        }
        return 0;
    }

    if (player->convertor) {
        snd_free(player->convertor);
        player->convertor = NULL;
    }

    player->convertor = snd_zalloc(sizeof(format_convert_t), AFM_EXTN);
    if (!player->convertor) {
        M_LOGE("alloc format convertor failed !\n");
        return -1;
    }

    player->convertor->src_bits = src_bits;
    player->convertor->dst_bits = dst_bits;
    player->convertor->src_channels = src_ch;
    player->convertor->dst_channels = dst_ch;

    dst = dst_ch * dst_bits;
    src = src_ch * src_bits;

    if (dst <= src)
        player->convertor->coeff = 1;
    else
        player->convertor->coeff = (dst / src) + ((dst % src) > 0) ? 1 : 0;

    player->convertor->coeff = player->convertor->coeff > 0 ?
        player->convertor->coeff : 1;

    return 0;

}

static int player_data_format_proc(player_t *player,
    uint8_t *input, int input_size, uint8_t **output, int *output_size)
{
    format_convert_t *convertor;
    int buffer_size;

    convertor = player->convertor;
    if (!convertor) {
        *output = input;
        *output_size = input_size;
        return 0;
    }

    buffer_size = input_size * convertor->coeff;
    if (convertor->buffer_size < buffer_size) {
        if (convertor->buffer) {
            snd_free(convertor->buffer);
            convertor->buffer = NULL;
        }
    }

    if (!convertor->buffer) {
        convertor->buffer = snd_zalloc(buffer_size, AFM_MAIN);
        if (!convertor->buffer) {
            convertor->buffer_size = 0;
            M_LOGE("alloc buffer failed !\n");
            return -1;
        }
        convertor->buffer_size = buffer_size;
    }

    if (convertor->src_channels == 2 && convertor->dst_channels == 1) {
        short *out_data = (short *)convertor->buffer;
        short *in_data = (short *)input;
        int samples = input_size / 4;
        int i;

        for (i = 0; i < samples; i++)
            out_data[i] = in_data[i * 2];
        *output = convertor->buffer;
        *output_size = input_size / 2;
    } else if (convertor->src_channels == 1 && convertor->dst_channels == 2) {
        short *out_data = (short *)convertor->buffer;
        short *in_data = (short *)input;
        int samples = input_size / 2;
        int i;

        for (i = 0; i < samples; i++) {
            out_data[i * 2] = in_data[i];
            out_data[i * 2 + 1] = in_data[i];
        }
        *output = convertor->buffer;
        *output_size = input_size;
    }

    return 0;
}

static int player_data_format_free(player_t *player)
{
    if (player->convertor) {
        snd_free(player->convertor);
        player->convertor = NULL;
    }

    return 0;
}

static int player_output_render(void *priv, uint8_t *buffer,
    int nbytes)
{
    player_t *player = (player_t *)priv;
    uint8_t *resampler_data;
    uint8_t *format_data;
    int resampler_data_size;
    int format_data_size;
    void *out;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    if (player->stream_flowing)
        out = player->stream_mgr->out;
    else
        out = player->out;

    if (player->pcm_reset) {
        out_stream_reset(player->out);
        player->pcm_reset = 0;
    }

    if (!out_stream_configured(out)) {
        media_pcminfo_t *info;
        media_pcminfo_t pcminfo;
        if (player->stream_flowing)
            info = &player->stream_mgr->pcm_info;
        else
            info = &player->pcm_info;

        memcpy(&pcminfo, info, sizeof(pcminfo));

        if (out_stream_configure(out, &pcminfo)) {
            M_LOGE("configure stream failed !\n");
            return -1;
        }

        if (pcminfo.rate != info->rate) {
#ifdef UVOICE_RESAMPLE_ENABLE
            if (!player->resampler) {
                uvoice_resampler_create(&player->resampler, info->rate,
                    pcminfo.rate, info->channels, info->bits);
                if (!player->resampler) {
                    M_LOGE("create resampler failed !\n");
                    return -1;
                }
            } else {
                uvoice_resampler_update(player->resampler, info->rate,
                    pcminfo.rate, info->channels, info->bits);
            }
#else
            M_LOGE("resampler not enable !\n");
            return -1;
#endif
        } else {
#ifdef UVOICE_RESAMPLE_ENABLE
            if (player->resampler) {
                uvoice_resampler_release(player->resampler);
                player->resampler = NULL;
            }
#endif
        }

        player_data_format_init(player, info->channels, pcminfo.channels,
            info->bits, pcminfo.bits);

        player->idle_period = out_stream_period_duration(out);
        if (player->idle_period < 0) {
            M_LOGE("get idle period failed !\n");
            return -1;
        }
        M_LOGD("idle period %d\n", player->idle_period);
    }

    if (player->sw_volume)
        player_ampl_coeff(player, buffer, nbytes);

    if (player->fade_enable)
        player_fade_process(player, buffer, nbytes);

    if (!player->out_bypass) {

#ifdef UVOICE_RESAMPLE_ENABLE
        if (uvoice_resampler_process(player->resampler, buffer, nbytes,
            &resampler_data, &resampler_data_size)) {
            M_LOGE("resample error\n");
        }

        if (resampler_data_size <= 0) {
            M_LOGE("resampler_data_size err %d\n", resampler_data_size);
            return -1;
        }
#else
        resampler_data = buffer;
        resampler_data_size = nbytes;
#endif

        format_data = resampler_data;
        format_data_size = resampler_data_size;

        player_data_format_proc(player, resampler_data, resampler_data_size,
            &format_data, &format_data_size);

        if (out_stream_write(out, format_data, format_data_size)) {
            M_LOGE("output render failed, size %d !\n", format_data_size);
            return -1;
        }
    } else {
        M_LOGD("bypass %d\n", nbytes);
    }

    os_mutex_lock(player->fade.lock, OS_WAIT_FOREVER);
    if (player->fade.state == FADE_OUT_END && !player->out_bypass) {
        /* fade out complete, discard remaining output */
        player->out_bypass = 1;
    }
    os_mutex_unlock(player->fade.lock);

    return 0;
}

static int player_decode_default(void *priv, uint8_t *buffer,
    int nbytes)
{
    media_decoder_t *mdecoder = (media_decoder_t *)priv;

    if (!mdecoder) {
        M_LOGE("mdecoder null !\n");
        return -1;
    }

    if (mdecoder->output(mdecoder->priv, buffer, nbytes)) {
        M_LOGE("output failed !\n");
        return -1;
    }
    return 0;
}

static int player_action_default(void *priv,
    player_action_t action, void *arg)
{
    return 0;
}

static media_type_t player_type_parse(char *source)
{
    if (!source)
        return MEDIA_TYPE_UNKNOWN;
    else if (!strncmp(source, "http://", strlen("http://")))
        return MEDIA_TYPE_HTTP;
    else if (!strncmp(source, "https://", strlen("https://")))
        return MEDIA_TYPE_HTTP;
    else if (!strncmp(source, "fs:", strlen("fs:")))
        return MEDIA_TYPE_FILE;
    else if (!strncmp(source, "pt:", strlen("pt:")))
        return MEDIA_TYPE_FLASH;
    return MEDIA_TYPE_UNKNOWN;
}

static bool player_format_assert(media_format_t format)
{
    switch (format) {
        case MEDIA_FMT_MP3:
        case MEDIA_FMT_WAV:
        case MEDIA_FMT_PCM:
        case MEDIA_FMT_AAC:
        case MEDIA_FMT_SPX:
        case MEDIA_FMT_M4A:
        case MEDIA_FMT_OGG:
        case MEDIA_FMT_WMA:
        case MEDIA_FMT_AMR:
        case MEDIA_FMT_AMRWB:
        case MEDIA_FMT_OPS:
        case MEDIA_FMT_FLAC:
        return true;
        default:
        return false;
    }
    return false;
}

static int player_stack_size(media_format_t format)
{
    int size = 8192;

    switch (format) {
        case MEDIA_FMT_AAC:
#ifdef DECODER_FAAD2_AAC_ENABLE
        size = 48 * 1024;
#endif
        break;
        case MEDIA_FMT_M4A:
#ifdef DECODER_FAAD2_M4A_ENABLE
        size = 48 * 1024;
#endif
        break;
        default:
        break;
    }

#ifdef UVOICE_RESAMPLE_ENABLE
    size += 20 * 1024;
#endif

    return size;
}

static void player_reset(player_t *player)
{
    player->silent_time = 0;
    player->consumed_len = 0;
    player->pause = 0;
    player->stop = 0;
    player->error = 0;
    player->blocking = 0;
    player->fade_ignore = 0;
    player->frame_skip = 0;
    player->complete = 0;
    player->pcm_reset = 0;
    player->out_bypass = 0;
    player->fade.state = FADE_NOP;
    player->fade.pos = 0;
    player->fade.reset = 0;
    player->seek.state = SEEK_NOP;
    player->seek.fade_request = 0;
    player->seek.offset = 0;
    player->type = MEDIA_TYPE_UNKNOWN;
    player->format = MEDIA_FMT_UNKNOWN;
    player->format_preferable = MEDIA_FMT_UNKNOWN;
    memset(&player->pcm_info, 0, sizeof(player->pcm_info));
}

static void player_task(void *arg)
{
    player_t *player = (player_t *)arg;
    media_decoder_t *mdecoder;
    media_loader_t *mloader;
    int ret = 0;

    if (!player) {
        M_LOGE("player null !\n");
        return;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);

    if (player->state != PLAYER_STAT_READY &&
        player->state != PLAYER_STAT_STOP) {
        M_LOGE("player can't start ! state %d\n",
            player->state);
        os_mutex_unlock(player->lock);
        return;
    }

    mdecoder = player->mdecoder;
    if (!mdecoder) {
        M_LOGE("mdecoder null !\n");
        os_mutex_unlock(player->lock);
        return;
    }

    mloader = player->mloader;
    if (!mloader) {
        M_LOGE("mloader null !\n");
        os_mutex_unlock(player->lock);
        return;
    }

    if (mloader->action(mloader, PLAYER_START, NULL)) {
        M_LOGE("start mloader failed !\n");
        if (player->start_waiting) {
            os_sem_signal(player->start_sem);
            player->start_waiting = 0;
        }
        player_state_post(player, PLAYER_STAT_ERROR);
        os_mutex_unlock(player->lock);
        player_clr_source();
        return;
    }

    if (mdecoder->action(mdecoder, PLAYER_START, NULL)) {
        M_LOGE("start mdecoder failed !\n");
        if (player->start_waiting) {
            os_sem_signal(player->start_sem);
            player->start_waiting = 0;
        }
        player_state_post(player, PLAYER_STAT_ERROR);
        mloader->action(mloader, PLAYER_STOP, NULL);
        os_mutex_unlock(player->lock);
        player_clr_source();
        return;
    }

    if (player->state == PLAYER_STAT_READY)
        PLAYER_TRACE("ready->running\n");
    else
        PLAYER_TRACE("stop->running\n");

    player->state = PLAYER_STAT_RUNNING;
    player_state_post(player, player->state);

    mloader->buffer_dirty_size = 0;

    if (player->start_waiting) {
        os_sem_signal(player->start_sem);
        player->start_waiting = 0;
    }
    os_mutex_unlock(player->lock);

    while (1) {
        os_mutex_lock(player->lock, OS_WAIT_FOREVER);
        if (player->pause && !player->stop &&
            player->fade.state != FADE_OUT) {
            if (player->state == PLAYER_STAT_RUNNING) {
                if (player->fade_enable && !player->fade_ignore) {
                    if (player->fade.state == FADE_NOP ||
                        player->fade.state == FADE_IN ||
                        player->fade.state == FADE_IN_END) {
                        if (player->fade.state == FADE_NOP)
                            player->fade.pos = player->fade.out_scope;
                        else
                            player_fade_config(player,
                                player->fade.in_period_ms,
                                player->fade.in_period_ms);
                        player->fade.state = FADE_OUT;
                        M_LOGD("fade out start, pos %d\n",
                            player->fade.pos);
                        os_mutex_unlock(player->lock);
                        continue;
                    }
                }

                if (mdecoder->action(mdecoder, PLAYER_PAUSE, NULL)) {
                    M_LOGE("pause mdecoder failed !\n");
                    player->stop = 1;
                    player->error = 1;
                    player->pause = 0;
                    if (player->pause_waiting) {
                        player->pause_waiting = 0;
                        os_sem_signal(player->pause_sem);
                    }
                    os_mutex_unlock(player->lock);
                    continue;
                }

                if (mloader->action(mloader, PLAYER_PAUSE, NULL)) {
                    M_LOGE("pause mloader failed !\n");
                    player->stop = 1;
                    player->error = 1;
                    player->pause = 0;
                    if (player->pause_waiting) {
                        player->pause_waiting = 0;
                        os_sem_signal(player->pause_sem);
                    }
                    os_mutex_unlock(player->lock);
                    continue;
                }

                if (player->frame_skip) {
                    out_stream_mute(player->out, 1);
                    player->frame_skip = 0;
                }

                out_stream_stop(player->out);
                out_stream_release(player->out);

                media_decoder_release(mdecoder);

                #ifdef UVOICE_RESAMPLE_ENABLE
                uvoice_resampler_release(player->resampler);
                #endif

                player_data_format_free(player);

                player->mdecoder = NULL;
                player->resampler = NULL;
                player->out = NULL;
                player->out_bypass = 0;
                player->fade_ignore = 0;
                player->fade.state = FADE_NOP;

                if (player->seek.state != SEEK_NOP) {
                    player->seek.state = SEEK_NOP;
                    if (player->fade_enable && player->fade.reset) {
                        player_fade_config(player,
                            FADE_OUT_PERIOD_DEFAULT,
                            FADE_IN_PERIOD_DEFAULT);
                        player->fade.reset = 0;
                    }
                }

                if (player->seek.offset)
                    player->seek.offset = 0;

                player->state = PLAYER_STAT_PAUSED;
                player_state_post(player, player->state);
                PLAYER_TRACE("running->pause\n");
                if (player->pause_waiting) {
                    player->pause_waiting = 0;
                    os_sem_signal(player->pause_sem);
                }
            }

            if (player->state == PLAYER_STAT_PAUSED && !player->stop) {
                os_mutex_unlock(player->lock);
                os_msleep(50);
                continue;
            }
        } else if (player->state == PLAYER_STAT_PAUSED &&
            !player->stop) {
            player->out = out_stream_create();
            if (!player->out) {
                M_LOGE("alloc out stream failed !\n");
                player->stop = 1;
                if (player->resume_waiting) {
                    player->resume_waiting = 0;
                    os_sem_signal(player->resume_sem);
                }
                os_mutex_unlock(player->lock);
                continue;
            }

            mdecoder = media_decoder_create(player->format);
            if (!mdecoder) {
                M_LOGE("create mdecoder failed !\n");
                out_stream_release(player->out);
                player->out = NULL;
                player->stop = 1;
                if (player->resume_waiting) {
                    player->resume_waiting = 0;
                    os_sem_signal(player->resume_sem);
                }
                os_mutex_unlock(player->lock);
                continue;
            }

            mdecoder->output = player_output_render;
            mdecoder->message = player_message_handle;
            mdecoder->priv = player;

            if (!mdecoder->initialized) {
                mdecoder->decode = player_decode_default;
                mdecoder->action = player_action_default;
                mdecoder->initialized = 1;
            }

            player->mdecoder = mdecoder;

            if (mloader->action(mloader, PLAYER_RESUME, NULL)) {
                M_LOGE("mloader resume failed !\n");
                media_decoder_release(mdecoder);
                out_stream_release(player->out);
                player->mdecoder = NULL;
                player->out = NULL;
                player->stop = 1;
                if (player->resume_waiting) {
                    player->resume_waiting = 0;
                    os_sem_signal(player->resume_sem);
                }
                os_mutex_unlock(player->lock);
                continue;
            }

            if (mdecoder->action(mdecoder, PLAYER_RESUME, NULL)) {
                M_LOGE("mdecoder resume failed !\n");
                media_decoder_release(player->mdecoder);
                out_stream_release(player->out);
                player->mdecoder = NULL;
                player->out = NULL;
                player->stop = 1;
                if (player->resume_waiting) {
                    player->resume_waiting = 0;
                    os_sem_signal(player->resume_sem);
                }
                os_mutex_unlock(player->lock);
                continue;
            }

            if (player->fade_enable) {
                if (player->fade.state == FADE_NOP) {
                    player->fade.state = FADE_IN;
                    player->fade.pos = 0;
                    M_LOGD("fade in start\n");
                } else if (player->fade.state == FADE_IN_END) {
                    player->fade.state = FADE_NOP;
                }
            }

            player->state = PLAYER_STAT_RUNNING;
            player_state_post(player, player->state);
            PLAYER_TRACE("pause->resume\n");
            if (player->resume_waiting) {
                player->resume_waiting = 0;
                os_sem_signal(player->resume_sem);
            }
        }

        if (player->stop && player->fade.state != FADE_OUT) {
            if (player->state == PLAYER_STAT_RUNNING) {
                if (player->fade_enable && !player->fade_ignore &&
                    !player->error) {
                    if (player->fade.state == FADE_NOP) {
                        player->fade.state = FADE_OUT;
                        player->fade.pos = player->fade.out_scope;
                        M_LOGD("fade out start\n");
                        os_mutex_unlock(player->lock);
                        continue;
                    }
                }

                mdecoder->action(mdecoder, PLAYER_STOP, NULL);
                mloader->action(mloader, PLAYER_STOP, NULL);
                mloader->buffer_dirty_size = 0;

                if (player->frame_skip) {
                    out_stream_mute(player->out, 1);
                    player->frame_skip = 0;
                }
                out_stream_stop(player->out);

                #ifdef UVOICE_RESAMPLE_ENABLE
                uvoice_resampler_release(player->resampler);
                player->resampler = NULL;
                #endif

                media_decoder_reset(mdecoder);
                player->consumed_len = 0;
                player->out_bypass = 0;
                player->fade.state = FADE_NOP;

                if (player->seek.state != SEEK_NOP) {
                    player->seek.state = SEEK_NOP;
                    if (player->fade_enable && player->fade.reset) {
                        player_fade_config(player,
                            FADE_OUT_PERIOD_DEFAULT,
                            FADE_IN_PERIOD_DEFAULT);
                        player->fade.reset = 0;
                    }
                }

                if (player->seek.offset)
                    player->seek.offset = 0;

                player->stop = 0;
                if (player->pause)
                    player->pause = 0;
                player->fade_ignore = 0;
                player->state = PLAYER_STAT_STOP;
                player_state_post(player, player->state);

                if (player->cplt_waiting) {
                    os_sem_signal_all(player->cplt_sem);
                    player->cplt_waiting = 0;
                }

                os_mutex_unlock(player->lock);
                PLAYER_TRACE("running->stop\n");
                if (player->error) {
                    M_LOGI("player error, release it\n");
                    player_clr_source();
                }
                break;
            } else if (player->state == PLAYER_STAT_PAUSED) {
                mloader->action(mloader, PLAYER_STOP, NULL);
                snd_free(mloader->buffer);
                media_loader_release(mloader);
                snd_free(player->media_info);
                player->mloader = NULL;
                player->media_info = NULL;
                player_reset(player);
                player->state = PLAYER_STAT_IDLE;
                player_state_post(player, player->state);
                if (player->cplt_waiting) {
                    os_sem_signal_all(player->cplt_sem);
                    player->cplt_waiting = 0;
                }
                os_mutex_unlock(player->lock);
                PLAYER_TRACE("pause->idle\n");
                break;
            } else if (player->state == PLAYER_STAT_COMPLETE) {
                out_stream_stop(player->out);
                player->stop = 0;
                player->complete = 0;
                player->consumed_len = 0;
                player->state = PLAYER_STAT_STOP;
                player_state_post(player, player->state);
                os_mutex_unlock(player->lock);
                PLAYER_TRACE("complete->stop\n");
                if (player->error)
                    player_clr_source();
                if (player->cplt_waiting) {
                    os_sem_signal_all(player->cplt_sem);
                    player->cplt_waiting = 0;
                }
                break;
            } else {
                player->stop = 0;
                M_LOGE("stop failed !\n");
            }
        }

        if (player->complete && player->fade.state != FADE_OUT &&
            player->state == PLAYER_STAT_RUNNING) {
            mdecoder->action(mdecoder, PLAYER_COMPLETE, NULL);
            mloader->action(mloader, PLAYER_COMPLETE, NULL);
            mloader->buffer_dirty_size = 0;

            if (player->frame_skip)
                player->frame_skip = 0;

            if (player->deep_cplt) {
                media_decoder_release(mdecoder);
                if (mloader->buffer)
                    snd_free(mloader->buffer);
                media_loader_release(mloader);
                player->mdecoder = NULL;
                player->mloader = NULL;
            }

            player->fade.state = FADE_NOP;
            player->fade_ignore = 0;
            player->format_preferable = MEDIA_FMT_UNKNOWN;
            player->state = PLAYER_STAT_COMPLETE;
            player_state_post(player, player->state);
            player->silent_time = os_current_time();
            if (player->cplt_waiting) {
                os_sem_signal_all(player->cplt_sem);
                player->cplt_waiting = 0;
            }
            PLAYER_TRACE("running->complete\n");
        } else if (!player->complete &&
                player->state == PLAYER_STAT_COMPLETE) {
            if (mloader != player->mloader) {
                mloader = player->mloader;
                M_LOGD("mloader update\n");
            }

            if (mdecoder != player->mdecoder) {
                mdecoder = player->mdecoder;
                M_LOGD("mdecoder update\n");
            }

            #ifdef ALIGENIE_ENABLE
            if (mloader->action(mloader, PLAYER_START, NULL)) {
                M_LOGE("mloader start failed !\n");
                player->stop = 1;
                player->error = 1;
                if (player->start_waiting) {
                    player->start_waiting = 0;
                    os_sem_signal_all(player->start_sem);
                }
                os_mutex_unlock(player->lock);
                continue;
            }

            if (mdecoder->action(mdecoder, PLAYER_START, NULL)) {
                M_LOGE("mdecoder start failed !\n");
                mloader->action(mloader, PLAYER_STOP, NULL);
                player->stop = 1;
                player->error = 1;
                if (player->start_waiting) {
                    player->start_waiting = 0;
                    os_sem_signal_all(player->start_sem);
                }
                os_mutex_unlock(player->lock);
                continue;
            }
            #endif

            player->consumed_len = 0;
            player->state = PLAYER_STAT_RUNNING;
            player_state_post(player, player->state);
            if (player->start_waiting) {
                player->start_waiting = 0;
                os_sem_signal_all(player->start_sem);
            }
            PLAYER_TRACE("complete->running\n");
            if (player->pause || player->stop) {
                os_mutex_unlock(player->lock);
                continue;
            }
        }

        if (player->state == PLAYER_STAT_RUNNING) {
            if (player->seek.state == SEEK_PREPARE) {
                if (player->seek.fade_request &&
                    player->fade_enable) {
                    if (player->fade.state == FADE_NOP) {
                        player->fade.state = FADE_OUT;
                        player->fade.pos = player->fade.out_scope;
                        M_LOGD("fade out start\n");
                    }

                    if (player->fade.state == FADE_OUT_END) {
                        player->seek.state = SEEK_START;
                        player->fade.state = FADE_IN;
                        player->fade.pos = 0;
                        mloader->action(mloader, PLAYER_SEEK,
                            (void *)player->seek.offset);
                        M_LOGD("fade in start\n");
                    }
                } else {
                    player->seek.state = SEEK_START;
                    mloader->action(mloader, PLAYER_SEEK,
                        (void *)player->seek.offset);
                }
            } else if (player->seek.state == SEEK_COMPLETE &&
                player->fade.state == FADE_NOP) {
                M_LOGD("seek complete\n");
                player->seek.state = SEEK_NOP;
                if (player->fade_enable) {
                    player_fade_config(player,
                        FADE_OUT_PERIOD_DEFAULT,
                        FADE_IN_PERIOD_DEFAULT);
                    player->fade.reset = 0;
                }
            }

            os_mutex_unlock(player->lock);

            ret = mloader->read(mloader,
                mloader->buffer + mloader->buffer_dirty_size,
                mloader->buffer_size - mloader->buffer_dirty_size);
            if (ret < 0) {
                M_LOGE("load failed %d!\n", ret);
                os_mutex_lock(player->lock, OS_WAIT_FOREVER);
                player->stop = 1;
                player->error = 1;
                os_mutex_unlock(player->lock);
                continue;
            } else if (ret == 0) {
                if (mdecoder->pos_rebase) {
                    mloader->action(mloader, PLAYER_RELOAD, (void *)mdecoder->rebase_offset);
                    mdecoder->pos_rebase = 0;
                    mloader->buffer_dirty_size = 0;
                    player->consumed_len = (int)mdecoder->rebase_offset;
                    mdecoder->unproc_size = 0;
                    continue;
                }
                os_mutex_lock(player->lock, OS_WAIT_FOREVER);
                if (player->pause || player->stop) {
                    if (player->fade.state == FADE_OUT)
                        player->fade.state = FADE_OUT_END;
                    if (player->fade.state == FADE_IN)
                        player->fade.state = FADE_IN_END;
                    os_mutex_unlock(player->lock);
                    continue;
                }

                if (mloader->buffer_dirty_size < mloader->buffer_size) {
                    M_LOGD("play complete\n");
                    player->complete = 1;
                    if (player->fade.state == FADE_OUT) {
                        M_LOGD("cancel fade out\n");
                        player->fade.state = FADE_NOP;
                    }
                    os_mutex_unlock(player->lock);
                    continue;
                }
                os_mutex_unlock(player->lock);
            } else if (ret !=
                mloader->buffer_size - mloader->buffer_dirty_size) {
                M_LOGD("load %d ret %d\n",
                    mloader->buffer_size - mloader->buffer_dirty_size, ret);
            }
            mloader->buffer_dirty_size += ret;

            os_mutex_lock(player->lock, OS_WAIT_FOREVER);
            if (player->frame_skip) {
                M_LOGD("frame skipping\n");
                os_mutex_unlock(player->lock);
                continue;
            }
            os_mutex_unlock(player->lock);

            if (mdecoder->decode(mdecoder,
                    mloader->buffer, mloader->buffer_dirty_size)) {
                M_LOGE("decode failed !\n");
                os_mutex_lock(player->lock, OS_WAIT_FOREVER);
                player->stop = 1;
                player->error = 1;
                os_mutex_unlock(player->lock);
                continue;
            }

            player->consumed_len += mloader->buffer_dirty_size -
                    mdecoder->unproc_size;

            if (mdecoder->unproc_size >= mloader->buffer_dirty_size) {
                M_LOGD("decoder maybe error\n");
                mdecoder->unproc_size = 0;
            }

            if (mdecoder->unproc_size > 0) {
                snd_memmove(mloader->buffer,
                    mloader->buffer +
                    mloader->buffer_dirty_size - mdecoder->unproc_size,
                    mdecoder->unproc_size);
                mloader->buffer_dirty_size = mdecoder->unproc_size;
                mdecoder->unproc_size = 0;
            } else {
                mloader->buffer_dirty_size = 0;
            }

            if (player->out_bypass)
                player->out_bypass = 0;

            continue;
        } else if (player->state == PLAYER_STAT_COMPLETE) {
            if (os_current_time() - player->silent_time >
                player->standby_msec) {
                M_LOGD("standby end\n");
                player->fade.state = FADE_NOP;
                player->stop = 1;
                player->error = 1;
                os_mutex_unlock(player->lock);
                continue;
            }

            if (out_stream_silent(player->out)) {
                M_LOGE("standby failed !\n");
                player->stop = 1;
                player->error = 1;
                os_mutex_unlock(player->lock);
                continue;
            }
        }
        os_mutex_unlock(player->lock);
        os_msleep(player->idle_period);
    }

    if (player->stop_waiting) {
        player->stop_waiting = 0;
        os_sem_signal(player->stop_sem);
    }
    M_LOGD("exit\n");
}

static int player_start(void)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);

    if (player->stream_flowing) {
        M_LOGE("player stream flowing !\n");
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (player->state == PLAYER_STAT_READY ||
        player->state == PLAYER_STAT_STOP) {
        os_task_create(&player->task,
            "uvoice_play_task",
            player_task, player,
            player_stack_size(player->format),
            UVOICE_TASK_PRI_HIGHER);
        player->start_waiting = 1;
        os_mutex_unlock(player->lock);
        os_sem_wait(player->start_sem, OS_WAIT_FOREVER);
        os_mutex_lock(player->lock, OS_WAIT_FOREVER);
        if (player->state != PLAYER_STAT_RUNNING) {
            M_LOGE("player start failed !\n");
            os_mutex_unlock(player->lock);
            return -1;
        }
    } else if (player->state == PLAYER_STAT_RUNNING &&
        player->stop) {
        player->stop = 0;
        if (player->frame_skip)
            player->frame_skip = 0;

        if (player->fade_ignore)
            player->fade_ignore = 0;

        if (player->stop_waiting) {
            M_LOGD("cancel stop waiting\n");
            player->stop_waiting = 0;
            os_sem_signal(player->stop_sem);
        }

        os_mutex_lock(player->fade.lock, OS_WAIT_FOREVER);
        if (player->fade.state == FADE_OUT ||
            player->fade.state == FADE_OUT_END) {
            M_LOGD("fade out cancel, fade in start, pos %d\n",
                player->fade.pos);
            player->fade.state = FADE_IN;
            player_fade_config_unlock(player,
                player->fade.out_period_ms,
                player->fade.out_period_ms);
        }
        os_mutex_unlock(player->fade.lock);
    } else if (player->state == PLAYER_STAT_COMPLETE && !player->stop) {
        #ifdef ALIGENIE_ENABLE
        player->complete = 0;
        #else
        os_mutex_unlock(player->lock);
        if (player->mloader->action(player->mloader,
            PLAYER_START, NULL)) {
            os_mutex_lock(player->lock, OS_WAIT_FOREVER);
            M_LOGE("mloader start failed !\n");
            if (player->start_waiting) {
                player->start_waiting = 0;
                os_sem_signal(player->start_sem);
            }
            os_mutex_unlock(player->lock);
            return -1;
        }

        if (player->mdecoder->action(player->mdecoder,
            PLAYER_START, NULL)) {
            os_mutex_lock(player->lock, OS_WAIT_FOREVER);
            M_LOGE("mdecoder start failed !\n");
            player->mloader->action(player->mloader, PLAYER_STOP, NULL);
            if (player->start_waiting) {
                player->start_waiting = 0;
                os_sem_signal(player->start_sem);
            }
            os_mutex_unlock(player->lock);
            return -1;
        }

        os_mutex_lock(player->lock, OS_WAIT_FOREVER);
        player->complete = 0;
        player->start_waiting = 1;
        os_mutex_unlock(player->lock);
        os_sem_wait(player->start_sem, OS_WAIT_FOREVER);
        os_mutex_lock(player->lock, OS_WAIT_FOREVER);
        if (player->state != PLAYER_STAT_RUNNING) {
            M_LOGE("player not running !\n");
            os_mutex_unlock(player->lock);
            return -1;
        }
        #endif
    }

    os_mutex_unlock(player->lock);
    return 0;
}

static int player_complete(void)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);
    if (player->stream_flowing) {
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (!player_can_complete(player)) {
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (player->complete) {
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (player->pause_waiting || player->pause) {
        M_LOGW("pause pending\n");
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (player->stop_waiting || player->stop) {
        M_LOGW("stop pending\n");
        os_mutex_unlock(player->lock);
        return -1;
    }

    player->complete = 1;
    player->cplt_waiting = 1;

    if (player->state == PLAYER_STAT_RUNNING) {
        if (player->blocking) {
            M_LOGI("cancel blocking\n");
            player->fade_ignore = 1;
            player->frame_skip = 1;
            player->mloader->action(player->mloader,
                PLAYER_UNBLOCK, NULL);
        }
    }

    os_mutex_lock(player->fade.lock, OS_WAIT_FOREVER);
    if (player->fade_enable && !player->fade_ignore &&
        player->fade.state == FADE_NOP) {
        player->fade.state = FADE_OUT;
        player->fade.pos = player->fade.out_scope;
        M_LOGD("fade out start\n");
    }
    os_mutex_unlock(player->fade.lock);

    os_mutex_unlock(player->lock);
    os_sem_wait(player->cplt_sem, OS_WAIT_FOREVER);
    os_mutex_lock(player->lock, OS_WAIT_FOREVER);
    if (player->state != PLAYER_STAT_COMPLETE &&
        player->state != PLAYER_STAT_RUNNING &&
        player->state != PLAYER_STAT_STOP &&
        player->state != PLAYER_STAT_IDLE) {
        M_LOGE("player complete failed ! state %d\n",
            player->state);
        os_mutex_unlock(player->lock);
        return -1;
    } else {
        M_LOGD("player complete\n");
    }

    os_mutex_unlock(player->lock);
    return 0;
}

static int player_pause(void)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);
    if (player->stream_flowing) {
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (!player_can_pause(player)) {
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (player->pause || player->stop) {
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (player->state == PLAYER_STAT_COMPLETE) {
        M_LOGD("play complete, stop\n");
        if (!player->stop) {
            player->stop = 1;
            player->error = 1;
            player->cplt_waiting = 1;
            os_mutex_unlock(player->lock);
            if (os_sem_wait(player->cplt_sem, 5000)) {
                M_LOGE("wait stop timeout !\n");
                return -1;
            }
            os_mutex_lock(player->lock, OS_WAIT_FOREVER);
            if (player->state == PLAYER_STAT_STOP) {
                M_LOGD("player stop\n");
            } else if (player->state == PLAYER_STAT_IDLE) {
                M_LOGD("player idle\n");
            } else {
                M_LOGE("player stop failed ! state %d\n",
                    player->state);
                os_mutex_unlock(player->lock);
                return -1;
            }
        }
    } else if (player->state == PLAYER_STAT_RUNNING) {
        if (!player->pause) {
            player->pause = 1;
            player->pause_waiting = 1;

            if (player->blocking) {
                M_LOGI("cancel blocking\n");
                player->fade_ignore = 1;
                player->frame_skip = 1;
                player->mloader->action(player->mloader,
                    PLAYER_UNBLOCK, NULL);
            }

            os_mutex_unlock(player->lock);
            if (os_sem_wait(player->pause_sem, 20000)) {
                M_LOGE("wait pause timeout !\n");
                return -1;
            }

            os_mutex_lock(player->lock, OS_WAIT_FOREVER);

            if (player->state != PLAYER_STAT_PAUSED) {
                M_LOGE("pause failed ?!\n");
                os_mutex_unlock(player->lock);
                return -1;
            } else {
                M_LOGD("player pause\n");
            }
        }
    }

    os_mutex_unlock(player->lock);
    return 0;
}

static int player_resume(void)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;
    bool resume_sync = false;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);
    if (player->stream_flowing) {
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (!player_can_resume(player)) {
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (!player->pause) {
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (player->state == PLAYER_STAT_PAUSED)
        resume_sync = true;

    if (player->frame_skip)
        player->frame_skip = 0;

    if (player->fade_ignore)
        player->fade_ignore = 0;

    os_mutex_lock(player->fade.lock, OS_WAIT_FOREVER);
    if (player->fade.state == FADE_OUT ||
        player->fade.state == FADE_OUT_END) {
        M_LOGD("fade out cancel, fade in start, pos %d\n",
            player->fade.pos);
        player->fade.state = FADE_IN;
        player_fade_config_unlock(player,
            player->fade.out_period_ms,
            player->fade.out_period_ms);
    }
    os_mutex_unlock(player->fade.lock);

    player->pause = 0;
    if (player->pause_waiting) {
        M_LOGI("cancel pause waiting\n");
        player->pause_waiting = 0;
        os_sem_signal(player->pause_sem);
    }

    if (resume_sync) {
        player->resume_waiting = 1;
        os_mutex_unlock(player->lock);
        if (os_sem_wait(player->resume_sem, OS_WAIT_FOREVER)) {
            M_LOGE("wait resume timeout !\n");
            return -1;
        }

        os_mutex_lock(player->lock, OS_WAIT_FOREVER);
        if (player->state != PLAYER_STAT_RUNNING &&
            player->state != PLAYER_STAT_COMPLETE) {
            M_LOGE("player resume failed ! state %d\n",
                player->state);
            os_mutex_unlock(player->lock);
            return -1;
        } else {
            M_LOGD("player resume\n");
        }
    }

    os_mutex_unlock(player->lock);
    return 0;
}

static int player_stop(void)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);
    if (!player_can_stop(player)) {
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (player->stop) {
        os_mutex_unlock(player->lock);
        return -1;
    }

    player->stop = 1;
    player->stop_waiting = 1;
    if (player->state == PLAYER_STAT_RUNNING) {
        if (player->blocking) {
            M_LOGI("cancel blocking\n");
            player->fade_ignore = 1;
            player->frame_skip = 1;
            player->mloader->action(player->mloader,
                PLAYER_UNBLOCK, NULL);
        }
    }

    os_mutex_unlock(player->lock);
    if (os_sem_wait(player->stop_sem, 5000)) {
        M_LOGE("wait stop timeout !\n");
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);
    if (player->state == PLAYER_STAT_STOP) {
        M_LOGD("player stop\n");
    } else if (player->state == PLAYER_STAT_IDLE) {
        M_LOGD("player idle\n");
    } else {
        M_LOGE("player stop failed ! state %d\n",
            player->state);
        os_mutex_unlock(player->lock);
        return -1;
    }

    os_mutex_unlock(player->lock);
    return 0;
}

static int player_pause_async(void)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);
    if (!player_can_pause(player)) {
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (player->pause || player->stop) {
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (player->state == PLAYER_STAT_RUNNING) {
        player->pause = 1;
        if (player->blocking) {
            M_LOGI("cancel blocking\n");
            player->fade_ignore = 1;
            player->frame_skip = 1;
            player->mloader->action(player->mloader,
                PLAYER_UNBLOCK, NULL);
        }
    } else if (player->state == PLAYER_STAT_COMPLETE) {
        player->stop = 1;
        player->error = 1;
    }

    os_mutex_unlock(player->lock);
    return 0;
}

static int player_resume_async(void)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);
    if (!player_can_resume(player)) {
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (!player->pause) {
        os_mutex_unlock(player->lock);
        return -1;
    }

    player->pause = 0;

    if (player->pause_waiting) {
        M_LOGI("cancel pause waiting\n");
        player->pause_waiting = 0;
        os_sem_signal(player->pause_sem);
    }

    os_mutex_unlock(player->lock);
    return 0;
}

static int player_stop_async(void)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);
    if (!player_can_stop(player)) {
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (!player->stop) {
        player->stop = 1;
        if (player->state == PLAYER_STAT_RUNNING) {
            if (player->blocking) {
                M_LOGI("cancel blocking\n");
                player->fade_ignore = 1;
                player->frame_skip = 1;
                player->mloader->action(player->mloader,
                    PLAYER_UNBLOCK, NULL);
            }
        }
    }

    os_mutex_unlock(player->lock);
    return 0;
}

static void player_stream_task(void *arg)
{
    stream_mgr_t *mgr = (stream_mgr_t *)arg;
    media_decoder_t *mdecoder;
    int rd_size = 0;
    int ret = 0;

    if (!mgr) {
        M_LOGE("mgr null !\n");
        return;
    }

    mdecoder = mgr->mdecoder;
    if (!mdecoder) {
        M_LOGE("mdecoder null !\n");
        return;
    }

    if (mdecoder->action(mdecoder, STREAM_MGR_START, NULL)) {
        M_LOGE("start mdecoder failed !\n");
        return;
    }

    mgr->buffer_dirty_size = 0;
    M_LOGD("start\n");

    while (1) {
        os_mutex_lock(mgr->lock, OS_WAIT_FOREVER);

        if (mgr->abort) {
            mdecoder->action(mdecoder, STREAM_MGR_STOP, NULL);
            out_stream_stop(mgr->out);
            os_mutex_unlock(mgr->lock);
            M_LOGD("stream abort\n");
            goto __exit;
        }

        rd_size = mgr->buffer_size - mgr->buffer_dirty_size;
        while (uvoice_ringbuff_dirtysize(&mgr->rb) < rd_size) {
            if (mgr->abort) {
                mdecoder->action(mdecoder, STREAM_MGR_STOP, NULL);
                out_stream_stop(mgr->out);
                os_mutex_unlock(mgr->lock);
                M_LOGD("stream abort\n");
                goto __exit;
            }

            if (mgr->stop) {
                rd_size = uvoice_ringbuff_dirtysize(&mgr->rb);
                if (rd_size > 0) {
                    ret = uvoice_ringbuff_read(&mgr->rb,
                        mgr->buffer + mgr->buffer_dirty_size,
                        rd_size);
                    if (ret != rd_size)
                        M_LOGW("read %d ret %d\n", rd_size, ret);

                    mgr->buffer_dirty_size += ret;
                    M_LOGD("flush %d\n", mgr->buffer_dirty_size);
                    os_mutex_unlock(mgr->lock);
                    if (mdecoder->decode(mdecoder,
                            mgr->buffer, mgr->buffer_dirty_size))
                        M_LOGE("decode error !\n");
                    os_mutex_lock(mgr->lock, OS_WAIT_FOREVER);
                }
                mdecoder->action(mdecoder, STREAM_MGR_STOP, NULL);
                out_stream_stop(mgr->out);
                os_mutex_unlock(mgr->lock);
                M_LOGD("stream end\n");
                goto __exit;
            }

            mgr->rd_waiting = 1;
            mgr->rd_len = rd_size;
            if (mgr->wr_waiting) {
                os_sem_signal(mgr->wr_sem);
                mgr->wr_waiting = 0;
            }
            os_mutex_unlock(mgr->lock);
            M_LOGD("waiting data %d dirtysize %d freesize %d\n",
                mgr->rd_len,
                uvoice_ringbuff_dirtysize(&mgr->rb),
                uvoice_ringbuff_freesize(&mgr->rb));
            os_sem_wait(mgr->rd_sem, OS_WAIT_FOREVER);
            os_mutex_lock(mgr->lock, OS_WAIT_FOREVER);
            mgr->rd_waiting = 0;
        }

        if (mgr->abort) {
            mdecoder->action(mdecoder, STREAM_MGR_STOP, NULL);
            out_stream_stop(mgr->out);
            os_mutex_unlock(mgr->lock);
            M_LOGD("stream abort\n");
            goto __exit;
        }

        ret = uvoice_ringbuff_read(&mgr->rb,
            mgr->buffer + mgr->buffer_dirty_size, rd_size);
        if (ret < 0) {
            M_LOGE("read failed !\n");
            os_mutex_unlock(mgr->lock);
            goto __exit;
        } else if (ret != rd_size) {
            M_LOGW("read %d ret %d\n", rd_size, ret);
        }

        if (mgr->wr_waiting &&
            uvoice_ringbuff_freesize(&mgr->rb) >= mgr->wr_len) {
            mgr->wr_waiting = 0;
            os_sem_signal(mgr->wr_sem);
        }

        mgr->buffer_dirty_size += ret;
        os_mutex_unlock(mgr->lock);

        if (mdecoder->decode(mdecoder,
                mgr->buffer, mgr->buffer_dirty_size)) {
            M_LOGE("decode failed !\n");
            break;
        }

        if (mdecoder->unproc_size >= mgr->buffer_dirty_size) {
            M_LOGW("decoder error\n");
            mdecoder->unproc_size = 0;
        }

        if (mdecoder->unproc_size > 0) {
            snd_memmove(mgr->buffer,
                mgr->buffer + mgr->buffer_dirty_size - mdecoder->unproc_size,
                mdecoder->unproc_size);
            mgr->buffer_dirty_size = mdecoder->unproc_size;
            mdecoder->unproc_size = 0;
        } else {
            mgr->buffer_dirty_size = 0;
        }
    }

__exit:
    if (mgr->wr_waiting) {
        M_LOGD("wake up wr waiting\n");
        os_sem_signal(mgr->wr_sem);
        mgr->wr_waiting = 0;
    } else {
        mgr->stop = 0;
        mgr->abort = 0;
        if (mgr->cplt_waiting) {
            mgr->cplt_waiting = 0;
            os_sem_signal(mgr->cplt_sem);
        }
    }
    M_LOGD("exit\n");
}

static int player_stream_fill(stream_mgr_t *mgr,
    const uint8_t *buffer, int nbytes)
{
    if (!mgr) {
        M_LOGE("mgr null !\n");
        return -1;
    }

    os_mutex_lock(mgr->lock, OS_WAIT_FOREVER);

    if (mgr->stop || mgr->abort) {
        os_mutex_unlock(mgr->lock);
        return -1;
    }

    while (uvoice_ringbuff_freesize(&mgr->rb) < nbytes) {
        if (mgr->rd_waiting) {
            M_LOGD("wake rd waiting, nbytes %d dirtysize %d freesize %d\n",
                nbytes,
                uvoice_ringbuff_dirtysize(&mgr->rb),
                uvoice_ringbuff_freesize(&mgr->rb));
            os_sem_signal(mgr->rd_sem);
            mgr->rd_waiting = 0;
        }
        mgr->wr_waiting = 1;
        mgr->wr_len = nbytes;

        if (mgr->state == STREAM_MGR_STAT_READY) {
            mgr->state = STREAM_MGR_STAT_RUNNING;
            os_task_create(&mgr->task,
                "uvoice_stream_task",
                player_stream_task, mgr,
                player_stack_size(mgr->format),
                UVOICE_TASK_PRI_HIGHER);
        }

        os_mutex_unlock(mgr->lock);

        os_sem_wait(mgr->wr_sem, OS_WAIT_FOREVER);
        os_mutex_lock(mgr->lock, OS_WAIT_FOREVER);
        mgr->wr_waiting = 0;
        if (mgr->stop || mgr->abort) {
            M_LOGD("stop\n");
            if (mgr->state != STREAM_MGR_STAT_RUNNING) {
                if (mgr->cplt_waiting) {
                    mgr->cplt_waiting = 0;
                    os_sem_signal(mgr->cplt_sem);
                }
            } else {
                if (mgr->rd_waiting) {
                    mgr->rd_waiting = 0;
                    os_sem_signal(mgr->rd_sem);
                }
            }
            goto __exit;
        }
    }

    if (uvoice_ringbuff_fill(&mgr->rb, buffer, nbytes) < 0) {
        M_LOGE("fill failed !\n");
        os_mutex_unlock(mgr->lock);
        return -1;
    }

    if (mgr->rd_waiting &&
        uvoice_ringbuff_dirtysize(&mgr->rb) >= mgr->rd_len) {
        mgr->rd_waiting = 0;
        os_sem_signal(mgr->rd_sem);
    }

__exit:
    os_mutex_unlock(mgr->lock);
    return 0;
}

static int player_stream_handle(stream_mgr_t *mgr,
    const uint8_t *buffer, int nbytes)
{
    media_decoder_t *mdecoder;
    int remaining_size = nbytes;
    int avail_size;
    int copy_size;

    if (!mgr) {
        M_LOGE("mgr null !\n");
        return -1;
    }

    mdecoder = mgr->mdecoder;
    if (!mdecoder) {
        M_LOGE("mdecoder null !\n");
        return -1;
    }

    avail_size = remaining_size + mgr->buffer_dirty_size;

    if (avail_size < mgr->buffer_size) {
        snd_memcpy(mgr->buffer + mgr->buffer_dirty_size,
            buffer, remaining_size);
        mgr->buffer_dirty_size += remaining_size;
        goto __exit;
    }

    while (1) {
        if (avail_size < mgr->buffer_size) {
            if (remaining_size > 0) {
                snd_memcpy(mgr->buffer + mgr->buffer_dirty_size,
                    buffer + nbytes - remaining_size,
                    remaining_size);
                mgr->buffer_dirty_size += remaining_size;
            }
            break;
        }
        copy_size = MIN(remaining_size,
            mgr->buffer_size - mgr->buffer_dirty_size);
        snd_memcpy(mgr->buffer + mgr->buffer_dirty_size,
            buffer + nbytes - remaining_size, copy_size);
        remaining_size -= copy_size;
        mgr->buffer_dirty_size += copy_size;

        if (mdecoder->decode(mdecoder,
            mgr->buffer, mgr->buffer_dirty_size)) {
            M_LOGE("decode failed !\n");
            return -1;
        }

        if (mdecoder->unproc_size >= mgr->buffer_dirty_size) {
            M_LOGW("decoder error\n");
            mdecoder->unproc_size = 0;
        }

        if (mdecoder->unproc_size > 0) {
            snd_memmove(mgr->buffer,
                mgr->buffer +
                mgr->buffer_dirty_size - mdecoder->unproc_size,
                mdecoder->unproc_size);
            mgr->buffer_dirty_size = mdecoder->unproc_size;
            mdecoder->unproc_size = 0;
        } else {
            mgr->buffer_dirty_size = 0;
        }
        avail_size = remaining_size + mgr->buffer_dirty_size;
    }

__exit:
    if (mgr->state == STREAM_MGR_STAT_READY) {
        M_LOGD("stream handle running\n");
        mgr->state = STREAM_MGR_STAT_RUNNING;
    }

    return 0;
}

static int player_put_stream(const uint8_t *buffer, int nbytes)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;
    stream_mgr_t *mgr;
    const uint8_t *pbuffer = buffer;
    bool need_split = false;
    int remaining_size;
    int ret;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    if (!buffer) {
        M_LOGE("buffer null !\n");
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);
    if (player->state == PLAYER_STAT_RUNNING ||
            player->state == PLAYER_STAT_COMPLETE) {
        M_LOGE("player conflict !\n");
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (!player->stream_flowing) {
        M_LOGW("stream mgr not running\n");
        os_mutex_unlock(player->lock);
        return -1;
    }

    mgr = player->stream_mgr;
    if (!mgr) {
        M_LOGE("stream mgr not initialized !\n");
        os_mutex_unlock(player->lock);
        return -1;
    }
    os_mutex_unlock(player->lock);

    os_mutex_lock(mgr->lock, OS_WAIT_FOREVER);

    if (mgr->state == STREAM_MGR_STAT_IDLE ||
        mgr->state == STREAM_MGR_STAT_STOP) {
        M_LOGE("stream mgr released/stop !\n");
        os_mutex_unlock(mgr->lock);
        return -1;
    }

    if (mgr->stop || mgr->abort) {
        M_LOGE("drop %d\n", nbytes);
        os_mutex_unlock(mgr->lock);
        return -1;
    }

    if (!mgr->cache_enable) {
        ret = player_stream_handle(mgr, buffer, nbytes);
        os_mutex_unlock(mgr->lock);
        return ret;
    }

    remaining_size = nbytes;
    if (remaining_size > (mgr->stream_pool_size / 2))
        need_split = true;

    os_mutex_unlock(mgr->lock);

    if (need_split) {
        while (remaining_size >= mgr->buffer_size) {
            player_stream_fill(mgr, pbuffer, mgr->buffer_size);
            pbuffer += mgr->buffer_size;
            remaining_size -= mgr->buffer_size;
        }
    }

    if (remaining_size > 0)
        player_stream_fill(mgr, pbuffer, remaining_size);

    return 0;
}

static int player_wait_complete(void)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);
    if (player->stream_flowing) {
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (player->state == PLAYER_STAT_READY ||
        player->state == PLAYER_STAT_STOP) {
        os_mutex_unlock(player->lock);
        os_msleep(100);
        os_mutex_lock(player->lock, OS_WAIT_FOREVER);
    }

    if (!player_can_wait_complete(player)) {
        M_LOGE("state %d can't wait complete !\n",
            player->state);
        os_mutex_unlock(player->lock);
        return -1;
    }

    player->cplt_waiting = 1;
    os_mutex_unlock(player->lock);

    os_sem_wait(player->cplt_sem, OS_WAIT_FOREVER);
    M_LOGD("player complete\n");

    return 0;
}

static int player_set_source(char *source)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;
    media_format_t format = MEDIA_FMT_UNKNOWN;
    media_type_t type;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    if (!source) {
        M_LOGE("source null !\n");
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);

    if (player->stream_flowing || player->stop || player->pause) {
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (!player_can_set_source(player)) {
        M_LOGE("state %d can't set source !\n", player->state);
        os_mutex_unlock(player->lock);
        return -1;
    }

    M_LOGD("source %s\n", source);

    type = player_type_parse(source);
    if (type == MEDIA_TYPE_UNKNOWN) {
        M_LOGE("media type unsupport !\n");
        player_state_post(player, PLAYER_STAT_FORMAT_UNSUPPORT);
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (player->type != type) {
        if (player->mloader) {
            M_LOGD("release prev mloader\n");
            M_LOGD("free prev mloader buffer %d\n",
                player->mloader->buffer_size);
            snd_free(player->mloader->buffer);
            media_loader_release(player->mloader);
            player->mloader = NULL;
        } else if (player->type != MEDIA_TYPE_UNKNOWN) {
            M_LOGE("prev mloader null !\n");
            player->type = MEDIA_TYPE_UNKNOWN;
            os_mutex_unlock(player->lock);
            return -1;
        }
        player->type = type;
    }

    if (!player->mloader) {
        player->mloader = media_loader_create(player->type, source);
        if (!player->mloader) {
            M_LOGE("create mloader failed !\n");
            player->type = MEDIA_TYPE_UNKNOWN;
            os_mutex_unlock(player->lock);
            return -1;
        }

        player->mloader->message = player_message_handle;
        player->mloader->priv = player;
    } else {
        media_loader_reset(player->mloader);
        if (player->mloader->action(player->mloader,
                PLAYER_NEXT, source)) {
            M_LOGE("mloader update failed !\n");
            os_mutex_unlock(player->lock);
            return -1;
        }
    }

    if (!player_format_assert(player->format_preferable)) {
        format_parse_byname(source, &format);

        if (format == MEDIA_FMT_UNKNOWN) {
            if (player->mloader->get_format(player->mloader,
                &format)) {
                M_LOGE("mloader get source format failed !\n");
                if (player->state == PLAYER_STAT_IDLE)
                    goto __exit;
                os_mutex_unlock(player->lock);
                return -1;
            }
        }

        if (!player_format_assert(format)) {
            M_LOGE("unknown format !\n");
            player_state_post(player, PLAYER_STAT_FORMAT_UNSUPPORT);
            if (player->state == PLAYER_STAT_IDLE)
                goto __exit;
            os_mutex_unlock(player->lock);
            return -1;
        }
    } else {
        format = player->format_preferable;
    }

    if (player->format != format) {
        if (player->mdecoder) {
            media_decoder_release(player->mdecoder);
            player->mdecoder = NULL;
        } else if (player->format != MEDIA_FMT_UNKNOWN) {
            M_LOGE("prev mdecoder null !\n");
            goto __exit;
        }
        player->format = format;
    }

    if (!player->mdecoder) {
        player->mdecoder = media_decoder_create(player->format);
        if (!player->mdecoder) {
            M_LOGE("create mdecoder failed !\n");
            if (player->state == PLAYER_STAT_IDLE)
                goto __exit;
            os_mutex_unlock(player->lock);
            return -1;
        }

        player->mdecoder->output = player_output_render;
        player->mdecoder->message = player_message_handle;
        player->mdecoder->priv = player;

        if (!player->mdecoder->initialized) {
            player->mdecoder->decode = player_decode_default;
            player->mdecoder->action = player_action_default;
            player->mdecoder->initialized = 1;
        }

        if (!player->mloader->buffer ||
            player->mloader->buffer_size != player->mdecoder->input_size) {
            if (player->mloader->buffer) {
                M_LOGD("free prev mloader buffer %d\n",
                    player->mloader->buffer_size);
                snd_free(player->mloader->buffer);
            }

            player->mloader->buffer_size = player->mdecoder->input_size;
            player->mloader->buffer = snd_zalloc(
                    player->mloader->buffer_size, AFM_MAIN);
            if (!player->mloader->buffer) {
                M_LOGE("alloc mloader buffer failed !\n");
                if (player->state == PLAYER_STAT_IDLE)
                    goto __exit1;
                os_mutex_unlock(player->lock);
                return -1;
            }
        }
    } else {
        media_decoder_reset(player->mdecoder);
        if (!player->mloader->buffer) {
            M_LOGD("alloc mloader buffer %d\n",
                player->mdecoder->input_size);
            player->mloader->buffer_size = player->mdecoder->input_size;
            player->mloader->buffer = snd_zalloc(
                    player->mloader->buffer_size, AFM_MAIN);
            if (!player->mloader->buffer) {
                M_LOGE("alloc mloader buffer failed !\n");
                os_mutex_unlock(player->lock);
                return -1;
            }
        }
    }

    if (!player->out) {
        player->out = out_stream_create();
        if (!player->out) {
            M_LOGE("alloc out stream failed !\n");
            goto __exit2;
        }
    }

    if (!player->media_info) {
        player->media_info = snd_zalloc(
                sizeof(media_info_t), AFM_EXTN);
        if (!player->media_info) {
            M_LOGE("alloc media info failed !\n");
            goto __exit3;
        }
    } else {
        memset(player->media_info, 0, sizeof(media_info_t));
    }

    player->mloader->get_mediainfo(player->mloader,
        player->media_info, player->format);

    if (player->state == PLAYER_STAT_IDLE) {
        player->state = PLAYER_STAT_READY;
        player_state_post(player, player->state);
        PLAYER_TRACE("idle->ready\n");
    } else if (player->state == PLAYER_STAT_COMPLETE) {
        os_mutex_unlock(player->lock);
        player_start();
        return 0;
    }

    os_mutex_unlock(player->lock);
    return 0;

__exit3:
    out_stream_release(player->out);
    player->out = NULL;
__exit2:
    snd_free(player->mloader->buffer);
__exit1:
    media_decoder_release(player->mdecoder);
    player->mdecoder = NULL;
__exit:
    media_loader_release(player->mloader);
    player->mloader = NULL;
    player_reset(player);
    player->state = PLAYER_STAT_IDLE;
    player_state_post(player, player->state);
    os_mutex_unlock(player->lock);
    return -1;
}

static int player_clr_source(void)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);

    if (player->state == PLAYER_STAT_IDLE) {
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (!player_can_clr_source(player)) {
        M_LOGE("state %d can't clr source !\n", player->state);
        os_mutex_unlock(player->lock);
        return -1;
    }

    out_stream_release(player->out);
    snd_free(player->media_info);

    player->media_info = NULL;
    player->out = NULL;

#ifdef UVOICE_RESAMPLE_ENABLE
    if (player->resampler) {
        uvoice_resampler_release(player->resampler);
        player->resampler = NULL;
    }
#endif

    if (player->mdecoder) {
        media_decoder_release(player->mdecoder);
        player->mdecoder = NULL;
    }

    if (player->mloader) {
        if (player->mloader->buffer)
            snd_free(player->mloader->buffer);
        media_loader_release(player->mloader);
        player->mloader = NULL;
    }

    player_data_format_free(player);

    player_reset(player);
    if (player->state == PLAYER_STAT_READY)
        PLAYER_TRACE("ready->idle\n");
    else
        PLAYER_TRACE("stop->idle\n");
    player->state = PLAYER_STAT_IDLE;
    player_state_post(player, player->state);

    os_mutex_unlock(player->lock);
    return 0;
}

static int player_set_stream(media_format_t format,
    int cache_enable, int cache_size)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;
    stream_mgr_t *mgr;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    if (!player_format_assert(format)) {
        M_LOGE("format unsupport !\n");
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);

    if (player->stream_flowing) {
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (!player_can_set_stream(player)) {
        M_LOGE("player conflict ! state %d\n", player->state);
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (player->stream_mgr) {
        M_LOGE("stream mgr not release !\n");
        os_mutex_unlock(player->lock);
        return -1;
    }

    mgr = snd_zalloc(sizeof(stream_mgr_t), AFM_EXTN);
    if (!mgr) {
        M_LOGE("alloc stream mgr failed !\n");
        os_mutex_unlock(player->lock);
        return -1;
    }

    mgr->format = format;
    M_LOGD("format %d\n", mgr->format);

    mgr->out = out_stream_create();
    if (!mgr->out) {
        M_LOGE("create out stream failed !\n");
        goto __exit;
    }

    mgr->mdecoder = media_decoder_create(mgr->format);
    if (!mgr->mdecoder) {
        M_LOGE("create mdecoder failed !\n");
        goto __exit1;
    }

    mgr->mdecoder->output = player_output_render;
    mgr->mdecoder->message = player_message_handle;
    mgr->mdecoder->priv = player;

    if (!mgr->mdecoder->initialized) {
        mgr->mdecoder->decode = player_decode_default;
        mgr->mdecoder->action = player_action_default;
        mgr->mdecoder->initialized = 1;
    }

    mgr->buffer_size = mgr->mdecoder->input_size;
    mgr->buffer = snd_zalloc(mgr->buffer_size, AFM_MAIN);
    if (!mgr->buffer) {
        M_LOGE("alloc buffer failed !\n");
        goto __exit2;
    }

    mgr->cache_enable = cache_enable;
    if (mgr->cache_enable) {
        mgr->stream_pool_size = MAX(mgr->buffer_size, cache_size);
        if (mgr->stream_pool_size < mgr->buffer_size * 2) {
            M_LOGE("stream pool size %d not enough !\n",
                mgr->stream_pool_size);
            goto __exit3;
        }
        M_LOGD("stream pool size %d\n", mgr->stream_pool_size);
        mgr->stream_pool = snd_zalloc(mgr->stream_pool_size + 4,
            AFM_EXTN);
        if (!mgr->stream_pool) {
            M_LOGE("alloc stream pool failed !\n");
            goto __exit3;
        }

        if (uvoice_ringbuff_init(&mgr->rb,
                mgr->stream_pool, mgr->stream_pool_size)) {
            M_LOGE("init stream pool failed !\n");
            goto __exit4;
        }
    }

    mgr->cplt_sem = os_sem_new(0);
    mgr->wr_sem = os_sem_new(0);
    mgr->rd_sem = os_sem_new(0);
    mgr->lock = os_mutex_new();

    player->stream_mgr = mgr;
    player->stream_flowing = 1;
    mgr->state = STREAM_MGR_STAT_READY;
    os_mutex_unlock(player->lock);

    M_LOGI("stream mgr ready\n");
    return 0;

__exit4:
    snd_free(mgr->stream_pool);
__exit3:
    snd_free(mgr->buffer);
__exit2:
    media_decoder_release(mgr->mdecoder);
    mgr->mdecoder = NULL;
__exit1:
    out_stream_release(mgr->out);
__exit:
    snd_free(mgr);
    os_mutex_unlock(player->lock);
    return -1;
}

static int player_clr_stream(int immediate)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;
    stream_mgr_t *mgr;
    media_decoder_t *mdecoder;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);

    mgr = player->stream_mgr;
    if (!mgr) {
        M_LOGI("no stream mgr\n");
        goto __exit;
    }

    os_mutex_lock(mgr->lock, OS_WAIT_FOREVER);

    mdecoder = mgr->mdecoder;
    if (!mdecoder) {
        M_LOGE("mdecoder null !\n");
        os_mutex_unlock(mgr->lock);
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (mgr->cache_enable) {
        if (mgr->state == STREAM_MGR_STAT_READY &&
            uvoice_ringbuff_dirtysize(&mgr->rb) > 0) {
            if (!immediate) {
                os_task_create(&mgr->task,
                    "uvoice_stream_task",
                    player_stream_task, mgr,
                    player_stack_size(mgr->format),
                    UVOICE_TASK_PRI_HIGHER);
                mgr->state = STREAM_MGR_STAT_RUNNING;
            }
        }

        if (mgr->state == STREAM_MGR_STAT_RUNNING) {
            if (immediate)
                mgr->abort = 1;
            else
                mgr->stop = 1;
            if (mgr->rd_waiting) {
                os_sem_signal(mgr->rd_sem);
                mgr->rd_waiting = 0;
            }

            if (mgr->wr_waiting) {
                os_sem_signal(mgr->wr_sem);
                mgr->wr_waiting = 0;
            }

            mgr->cplt_waiting = 1;

            os_mutex_unlock(mgr->lock);

            M_LOGD("waiting stream stop\n");
            os_sem_wait(mgr->cplt_sem, OS_WAIT_FOREVER);

            M_LOGD("stream stop\n");
            os_mutex_lock(mgr->lock, OS_WAIT_FOREVER);

            if (uvoice_ringbuff_dirtysize(&mgr->rb) > 0)
                M_LOGW("not flush %d\n",
                    uvoice_ringbuff_dirtysize(&mgr->rb));
        }
    } else {
        mdecoder->action(mdecoder, STREAM_MGR_STOP, NULL);
        out_stream_stop(mgr->out);
    }

    player->stream_flowing = 0;
    player->stream_mgr = NULL;
    os_mutex_unlock(mgr->lock);

    out_stream_release(mgr->out);
    media_decoder_release(mgr->mdecoder);

#ifdef UVOICE_RESAMPLE_ENABLE
    uvoice_resampler_release(player->resampler);
    player->resampler = NULL;
#endif

    player_data_format_free(player);

    os_sem_free(mgr->rd_sem);
    os_sem_free(mgr->wr_sem);
    os_sem_free(mgr->cplt_sem);
    os_mutex_free(mgr->lock);

    snd_free(mgr->buffer);
    if (mgr->stream_pool)
        snd_free(mgr->stream_pool);
    snd_free(mgr);

    M_LOGI("stream mgr release\n");

__exit:
    os_mutex_unlock(player->lock);
    return 0;
}

static int player_set_pcminfo(int rate, int channels, int bits,
    int frames)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;
    media_pcminfo_t *pcminfo;
    int changed = 0;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    if (!pcm_rate_valid(rate)) {
        M_LOGE("rate %d invalid !\n", rate);
        return -1;
    }

    if (!pcm_channel_valid(channels)) {
        M_LOGE("channels %d invalid !\n", channels);
        return -1;
    }

    if (!pcm_bits_valid(bits)) {
        M_LOGE("bits %d invalid !\n", bits);
        return -1;
    }

    if (frames <= 0) {
        M_LOGE("frames %d invalid !\n", frames);
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);
    if (!player_can_set_config(player)) {
        M_LOGE("can't set config !\n");
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (player->stream_mgr)
        pcminfo = &player->stream_mgr->pcm_info;
    else
        pcminfo = &player->pcm_info;

    if (pcminfo->rate != rate) {
        if (pcm_rate_valid(pcminfo->rate))
            changed = 1;
        pcminfo->rate = rate;
    }

    if (pcminfo->channels != channels) {
        if (pcm_channel_valid(pcminfo->channels))
            changed = 1;
        pcminfo->channels = channels;
    }

    if (pcminfo->bits != bits) {
        if (pcm_bits_valid(pcminfo->bits))
            changed = 1;
        pcminfo->bits = bits;
    }

    if (!player->stream_flowing &&
            player->state == PLAYER_STAT_COMPLETE)
        player->pcm_reset = changed;

    pcminfo->frames = frames;

    if (player->mdecoder && player->mdecoder->action)
        player->mdecoder->action(player->mdecoder,
            PLAYER_CONFIGURE, pcminfo);

    if (player->stream_mgr &&
        player->stream_mgr->mdecoder &&
        player->stream_mgr->mdecoder->action) {
        player->stream_mgr->mdecoder->action(
            player->stream_mgr->mdecoder,
            PLAYER_CONFIGURE, pcminfo);
    }

    M_LOGI("rate %d channels %d bits %d frames %d change %u\n",
        pcminfo->rate,
        pcminfo->channels,
        pcminfo->bits, pcminfo->frames, player->pcm_reset);

    os_mutex_unlock(player->lock);
    return 0;
}

static int player_set_fade(int out_period, int in_period)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    if (out_period < 0 || out_period > 100) {
        M_LOGE("out_period %d invalid !\n", out_period);
        return -1;
    }

    if (in_period < 0 || in_period > 100) {
        M_LOGE("in_period %d invalid !\n", in_period);
        return -1;
    }

    player_fade_config(player, out_period, in_period);
    return 0;
}

static int player_set_format(media_format_t format)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    if (!player_format_assert(format)) {
        M_LOGE("format %d unsupport !\n", format);
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);

    if (player->format_preferable != format)
        player->format_preferable = format;

    os_mutex_unlock(player->lock);
    return 0;
}

static int player_set_outdevice(audio_out_device_t device)
{
    snd_device_t _device = SND_DEVICE_NONE;

    switch (device) {
        case AUDIO_OUT_DEVICE_SPEAKER:
        _device = SND_DEVICE_OUT_SPEAKER;
        break;
        case AUDIO_OUT_DEVICE_HEADPHONE:
        _device = SND_DEVICE_OUT_HEADPHONE;
        break;
        case AUDIO_OUT_DEVICE_HEADSET:
        _device = SND_DEVICE_OUT_HEADSET;
        break;
        case AUDIO_OUT_DEVICE_RECEIVER:
        _device = SND_DEVICE_OUT_RECEIVER;
        break;
        case AUDIO_OUT_DEVICE_SPEAKER_AND_HEADPHONE:
        _device = SND_DEVICE_OUT_SPEAKER_AND_HEADPHONE;
        break;
        case AUDIO_OUT_DEVICE_SPEAKER_AND_HEADSET:
        _device = SND_DEVICE_OUT_SPEAKER_AND_HEADSET;
        break;
        default:
        return -1;
    }

    return out_device_set(_device);
}

static int player_set_external_pa(audio_extpa_info_t *info)
{
    return out_extpa_set(info);
}

static int player_set_standby(int msec)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    if (msec < 0) {
        M_LOGE("arg %d invalid !\n", msec);
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);

    if (player->standby_msec != msec)
        player->standby_msec = msec;

    os_mutex_unlock(player->lock);
    return 0;
}

static int player_get_state(player_state_t *state)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    if (!state) {
        M_LOGE("arg null !\n");
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);

    *state = player->state;

    os_mutex_unlock(player->lock);
    return 0;
}

static int player_get_delay(int *delay_ms)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    if (!delay_ms) {
        M_LOGE("arg null !\n");
        return -1;
    }

    *delay_ms = player->idle_period;
    return 0;
}

static int player_get_mediainfo(media_info_t *info)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    if (!info) {
        M_LOGE("arg null !\n");
        return -1;
    }

    if (!player->media_info) {
        M_LOGE("media info not found\n");
        return -1;
    }

    if (player->media_info->valid) {
        memcpy(info, player->media_info, sizeof(media_info_t));
    } else {
        if (!player->mloader) {
            M_LOGW("mloader released, ignore\n");
            return -1;
        }
        player->mloader->get_mediainfo(player->mloader,
            player->media_info, player->format);
        if (player->media_info->valid)
            memcpy(info, player->media_info, sizeof(media_info_t));
    }

    return 0;
}

static int player_get_cacheinfo(int *cache_size)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;
    media_loader_t *mloader;
    cache_info_t cache_info;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    if (!cache_size) {
        M_LOGE("arg null !\n");
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);

    if (player->type == MEDIA_TYPE_HTTP) {
        mloader = player->mloader;
        if (!mloader) {
            M_LOGE("mloader null !\n");
            os_mutex_unlock(player->lock);
            return -1;
        }

        memset(&cache_info, 0, sizeof(cache_info));
        if (mloader->get_cacheinfo &&
            mloader->get_cacheinfo(mloader, &cache_info)) {
            M_LOGE("get cache info failed !\n");
            os_mutex_unlock(player->lock);
            return -1;
        }
        *cache_size = cache_info.avail_cache_size;
    }

    os_mutex_unlock(player->lock);
    return 0;
}

static int player_get_duration(int *duration)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    if (!duration) {
        M_LOGE("arg null !\n");
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);

    if (player->state == PLAYER_STAT_RUNNING ||
        player->state == PLAYER_STAT_PAUSED ||
        player->state == PLAYER_STAT_COMPLETE ||
        player->state == PLAYER_STAT_STOP) {
        if (player->media_info) {
            if (player->media_info->duration > 0)
                *duration = player->media_info->duration;
            else if (player->media_info->bitrate > 0)
                *duration = (player->media_info->media_size * 8) /
                    player->media_info->bitrate;
        }
    }

    os_mutex_unlock(player->lock);
    return 0;
}

static int player_get_position(int *position)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    if (!position) {
        M_LOGE("arg null !\n");
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);

    if (player->state == PLAYER_STAT_RUNNING ||
        player->state == PLAYER_STAT_PAUSED ||
        player->state == PLAYER_STAT_COMPLETE) {
        if (player->media_info &&
                    player->media_info->bitrate > 0) {
            *position = (player->consumed_len * 8) /
                player->media_info->bitrate;
            if (player->state == PLAYER_STAT_COMPLETE) {
                int duration = (player->media_info->media_size * 8) /
                    player->media_info->bitrate;
                if (abs(*position - duration) < 3)
                    *position = duration;
            }
        }
    }

    os_mutex_unlock(player->lock);
    return 0;
}

static int player_set_volume(int volume)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;
    int ret;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    if (player->sw_volume) {
        if (volume > 100 && player->vol_level < 100)
            volume = 100;

        if (volume < 0 || volume > 100) {
            M_LOGE("volume %d invalid !\n", volume);
            return -1;
        }
        os_mutex_lock(player->vol_lock, OS_WAIT_FOREVER);
        if (player->vol_level != volume) {
            player->vol_level = volume;
            ret = os_kv_set(PLAYER_VOLUME_LEVEL_KEY,
                    &player->vol_level,
                    sizeof(player->vol_level), 0);
            if (ret) {
                M_LOGE("kv set failed %d!\n", ret);
                os_mutex_unlock(player->vol_lock);
                return -1;
            }
        }
        os_mutex_unlock(player->vol_lock);
    } else {
        if (out_volume_set(volume)) {
            M_LOGE("set volume failed !\n");
            return -1;
        }
    }

    return 0;
}

static int player_get_volume(int *volume)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;
    int vol;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    if (!volume) {
        M_LOGE("arg null !\n");
        return -1;
    }

    if (player->sw_volume) {
        os_mutex_lock(player->vol_lock, OS_WAIT_FOREVER);
        *volume = player->vol_level;
        os_mutex_unlock(player->vol_lock);
    } else {
        vol = out_volume_get();
        if (vol < 0) {
            M_LOGE("get volume failed !\n");
            return -1;
        }
        *volume = vol;
    }

    return 0;
}

static int player_cache_config(cache_config_t *config)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    if (!config) {
        M_LOGE("config null !\n");
        return -1;
    }

    if (config->place < 0 || config->place > 2) {
        M_LOGE("place %d unsupport !\n", config->place);
        return -1;
    }

    if ((config->place == 1) ||
        (config->place == 2 && config->mem_size < 40)) {
        M_LOGE("config invalid !\n");
        return -1;
    }

#ifdef UVOICE_HTTP_ENABLE
    http_cache_config(config);
#endif

    return 0;
}

static int player_volume_range(int *max, int *min)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    if (player->sw_volume) {
        if (min)
            *min = 0;
        if (max)
            *max = 100;
    } else {
        out_volume_range(max, min);
    }

    return 0;
}

static int player_playback(char *source)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;
    int standby_ms;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);

    if (player->stream_flowing) {
        M_LOGE("player stream flowing !\n");
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (player->state != PLAYER_STAT_IDLE) {
        M_LOGE("player state not idle !\n");
        os_mutex_unlock(player->lock);
        return -1;
    }

    standby_ms = player->standby_msec;
    player->standby_msec = -1;
    os_mutex_unlock(player->lock);

    if (player_set_source(source)) {
        M_LOGE("set source failed !\n");
        return -1;
    }

    player_task(player);

    player->standby_msec = standby_ms;
    return 0;
}

static int player_seek(int position)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;
    media_loader_t *mloader;
    media_info_t *media_info;
    cache_info_t cache_info;
    int seek_interval;
    int seek_offset;
    int val = 0;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    if (position < 0) {
        M_LOGE("arg %d invalid !\n", position);
        return -1;
    }

    if (player_get_duration(&val)) {
        M_LOGE("get duration failed !\n");
        return -1;
    }

    if (val <= 0) {
        M_LOGE("not seekable\n");
        return -1;
    }

    if (position >= val) {
        M_LOGE("arg %d overrange !\n", position);
        return -1;
    }

    val = 0;
    if (player_get_position(&val)) {
        M_LOGE("get position failed !\n");
        return -1;
    }

    seek_interval = position - val;
    if (seek_interval == 0) {
        M_LOGW("no seek interval\n");
        goto __exit;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);

    if (!player_can_seek(player)) {
        M_LOGE("player state %d can't seek\n",
            player->state);
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (player->seek.state != SEEK_NOP) {
        M_LOGE("seek state %d can't seek\n",
            player->seek.state);
        os_mutex_unlock(player->lock);
        return -1;
    }

    mloader = player->mloader;
    if (!mloader) {
        M_LOGE("mloader null\n");
        os_mutex_unlock(player->lock);
        return -1;
    }

    media_info = player->media_info;
    if (!media_info) {
        M_LOGE("media info null\n");
        os_mutex_unlock(player->lock);
        return -1;
    }

    seek_offset = (media_info->bitrate / 8) * seek_interval;
    if (seek_offset == 0) {
        M_LOGW("not seekable\n");
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (!mloader->get_cacheinfo) {
        M_LOGD("seek %ds offset %d\n", seek_interval, seek_offset);
        if (player->state == PLAYER_STAT_RUNNING &&
            player->fade_enable) {
            player_fade_config(player, 30, 60);
            player->fade.reset = 0;
            player->seek.fade_request = 1;
        } else {
            player->seek.fade_request = 0;
        }
        player->seek.state = SEEK_PREPARE;
        player->seek.offset = seek_offset;

        os_mutex_unlock(player->lock);
        goto __exit;
    }

    os_mutex_unlock(player->lock);

    memset(&cache_info, 0, sizeof(cache_info));

    if (mloader->get_cacheinfo(mloader, &cache_info)) {
        M_LOGE("get cache info failed\n");
        return -1;
    }

    M_LOGD("seek %ds offset %d\n", seek_interval, seek_offset);

    M_LOGD("avail_cache_size %d dirty_cache_size %d\n",
        cache_info.avail_cache_size, cache_info.dirty_cache_size);

    if ((seek_offset > 0 &&
        cache_info.avail_cache_size >= seek_offset) ||
        (seek_offset < 0 &&
        cache_info.dirty_cache_size >= abs(seek_offset))) {
        os_mutex_lock(player->lock, OS_WAIT_FOREVER);
        if (player->state == PLAYER_STAT_RUNNING &&
            player->fade_enable) {
            player_fade_config(player, 30, 60);
            player->fade.reset = 0;
            player->seek.fade_request = 1;
        } else {
            player->seek.fade_request = 0;
        }
        player->seek.state = SEEK_PREPARE;
        player->seek.offset = seek_offset;
        os_mutex_unlock(player->lock);
        goto __exit;
    } else {
        if ((seek_offset > 0 &&
            cache_info.seek_forward_limit >= seek_offset) ||
            (seek_offset < 0 &&
            cache_info.seek_backward_limit >= abs(seek_offset))) {
            if (player->state == PLAYER_STAT_RUNNING) {
                mloader->action(mloader,
                    PLAYER_SEEK_BEGIN, NULL);
                player_pause();
                mloader->action(mloader,
                    PLAYER_SEEK, (void *)seek_offset);
                player_resume();
                mloader->action(mloader,
                    PLAYER_SEEK_END, NULL);
            } else if (player->state == PLAYER_STAT_PAUSED) {
                mloader->action(mloader,
                    PLAYER_SEEK, (void *)seek_offset);
            }
        } else {
            M_LOGW("avail_cache_size %d dirty_cache_size %d\n",
                cache_info.avail_cache_size,
                cache_info.dirty_cache_size);
            M_LOGW("forward_limit %d backward_limit %d\n",
                cache_info.seek_forward_limit,
                cache_info.seek_backward_limit);
        }
    }

__exit:
    return 0;
}

static int player_eq_enable(int enable)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;
    int ret = 0;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    if (enable)
        ret = audio_param_set(PARAM_KEY_EFFECT_EQUALIZER,
            PARAM_VAL_ENABLE);
    else
        ret = audio_param_set(PARAM_KEY_EFFECT_EQUALIZER,
            PARAM_VAL_DISABLE);

    return ret;
}

int player_pcmdump_enable(int enable)
{
    int val = enable ? PARAM_VAL_ENABLE : PARAM_VAL_DISABLE;
    return audio_param_set(PARAM_KEY_OUT_PCM_DUMP, val);
}

static int player_state_dump(void)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);

    M_LOGI("state %d\n", player->state);
    M_LOGI("pause %u\n", player->pause);
    M_LOGI("stop %u\n", player->stop);
    M_LOGI("complete %u\n", player->complete);
    M_LOGI("pcm_reset %u\n", player->pcm_reset);
    M_LOGI("error %u\n", player->error);
    M_LOGI("stream_flowing %u\n", player->stream_flowing);
    M_LOGI("cplt_waiting %u\n", player->cplt_waiting);
    M_LOGI("start_waiting %u\n", player->start_waiting);
    M_LOGI("pause_waiting %u\n", player->pause_waiting);
    M_LOGI("resume_waiting %u\n", player->resume_waiting);
    M_LOGI("stop_waiting %u\n", player->stop_waiting);
    M_LOGI("blocking %u\n", player->blocking);
    M_LOGI("deep_cplt %u\n", player->deep_cplt);
    M_LOGI("frame_skip %u\n", player->frame_skip);
    M_LOGI("sw_volume %u\n", player->sw_volume);
    M_LOGI("out_bypass %u\n", player->out_bypass);
    M_LOGI("type %d format %d\n", player->type, player->format);
    M_LOGI("format_preferable %d\n", player->format_preferable);
    M_LOGI("silent_time %lld\n", player->silent_time);
    M_LOGI("consumed_len %d\n", player->consumed_len);
    M_LOGI("idle_period %d\n", player->idle_period);
    M_LOGI("standby_msec %d\n", player->standby_msec);
    M_LOGI("reference_count %d\n", player->reference_count);
    M_LOGI("seek state %d\n", player->seek.state);
    M_LOGI("seek offset %d\n", player->seek.offset);
    M_LOGI("seek fade_request %d\n", player->seek.fade_request);
    M_LOGI("fade_enable %d\n", player->fade_enable);
    M_LOGI("fade_ignore %d\n", player->fade_ignore);
    M_LOGI("fade state %d\n", player->fade.state);
    M_LOGI("fade pos %d\n", player->fade.pos);
    M_LOGI("fade out_scope %d out_period_ms %d\n",
        player->fade.out_scope, player->fade.out_period_ms);
    M_LOGI("fade in_scope %d in_period_ms %d\n",
        player->fade.in_scope, player->fade.in_period_ms);
    M_LOGI("pcm rate %d\n", player->pcm_info.rate);
    M_LOGI("pcm bits %d\n", player->pcm_info.bits);
    M_LOGI("pcm channels %d\n", player->pcm_info.channels);
    M_LOGI("pcm frames %d\n", player->pcm_info.frames);
    M_LOGI("out stream 0x%x\n", (uint32_t)player->out);
    M_LOGI("mloader 0x%x\n", (uint32_t)player->mloader);
    M_LOGI("mdecoder 0x%x\n", (uint32_t)player->mdecoder);

    if (player->stream_mgr) {
        os_mutex_lock(player->stream_mgr->lock, OS_WAIT_FOREVER);
        M_LOGI("stream mgr state %d\n",
                player->stream_mgr->state);
        M_LOGI("stream mgr cache_enable %u\n",
                player->stream_mgr->cache_enable);
        M_LOGI("stream mgr buffer_size %d\n",
                player->stream_mgr->buffer_size);
        M_LOGI("stream mgr buffer_dirty_size %d\n",
                player->stream_mgr->buffer_dirty_size);
        M_LOGI("stream mgr wr_len %d\n",
                player->stream_mgr->wr_len);
        M_LOGI("stream mgr rd_len %d\n",
                player->stream_mgr->rd_len);
        M_LOGI("stream mgr wr_waiting %u\n",
                player->stream_mgr->wr_waiting);
        M_LOGI("stream mgr rd_waiting %u\n",
                player->stream_mgr->rd_waiting);
        M_LOGI("stream mgr stop %u\n",
                player->stream_mgr->stop);
        M_LOGI("stream mgr abort %u\n",
                player->stream_mgr->abort);
        M_LOGI("stream mgr format %d\n",
                player->stream_mgr->format);
        M_LOGI("stream mgr pcm rate %d\n",
                player->stream_mgr->pcm_info.rate);
        M_LOGI("stream mgr pcm bits %d\n",
                player->stream_mgr->pcm_info.bits);
        M_LOGI("stream mgr pcm channels %d\n",
                player->stream_mgr->pcm_info.channels);
        M_LOGI("stream mgr pcm frames %d\n",
                player->stream_mgr->pcm_info.frames);
        os_mutex_unlock(player->stream_mgr->lock);
    }

    os_mutex_unlock(player->lock);
    return 0;
}

static int player_download(char *name)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;
    media_loader_t *mloader;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);

    if (!player_can_download(player)) {
        os_mutex_unlock(player->lock);
        return -1;
    }

    mloader = player->mloader;
    if (!mloader) {
        M_LOGE("mloader null !\n");
        os_mutex_unlock(player->lock);
        return -1;
    }

    if (mloader->action(mloader, PLAYER_DLOAD, name)) {
        M_LOGE("download failed !\n");
        os_mutex_unlock(player->lock);
        return -1;
    }

    os_mutex_unlock(player->lock);
    return 0;
}

static int player_download_abort(void)
{
    player_t *player = g_mplayer ? g_mplayer->priv : NULL;
    media_loader_t *mloader;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    os_mutex_lock(player->lock, OS_WAIT_FOREVER);

    if (!player_can_download(player)) {
        os_mutex_unlock(player->lock);
        return -1;
    }

    mloader = player->mloader;
    if (!mloader) {
        M_LOGE("mloader null !\n");
        os_mutex_unlock(player->lock);
        return -1;
    }

    os_mutex_unlock(player->lock);

    if (mloader->action(mloader, PLAYER_DLOAD_ABORT, NULL)) {
        M_LOGE("abort download failed !\n");
        return -1;
    }

    return 0;
}

static int player_format_support(media_format_t format)
{
    return player_format_assert(format);
}

static int player_volume_init(player_t *player)
{
    int kv_len;
    int ret;

    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    out_volume_set(8);

    kv_len = sizeof(player->vol_level);
    ret = os_kv_get(PLAYER_VOLUME_LEVEL_KEY,
            &player->vol_level, &kv_len);
    if (ret == -ENOENT) {
        player->vol_level = PLAYER_VOLUME_LEVEL_DEFAULT;
        M_LOGW("set default volume %d\n", player->vol_level);
    } else if (ret) {
        M_LOGE("get %s failed !\n", PLAYER_VOLUME_LEVEL_KEY);
        return -1;
    }

    player->vol_lock = os_mutex_new();
    return 0;
}

static int player_volume_deinit(player_t *player)
{
    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    os_mutex_free(player->vol_lock);
    return 0;
}

static int player_reference(player_t *player)
{
    if (!player)
        return -1;
    player->reference_count++;
    return player->reference_count;
}

static int player_dereference(player_t *player)
{
    if (!player)
        return -1;
    player->reference_count--;
    return player->reference_count;
}

static player_t *player_init(void)
{
    player_t *player;

    player = snd_zalloc(sizeof(player_t), AFM_EXTN);
    if (!player) {
        M_LOGE("alloc player failed !\n");
        return NULL;
    }

#if (UVOICE_MLIST_ENABLE == 1)
    mlist_init();
#endif

    player->sw_volume = PLAYER_SW_VOLUME_ENABLE;
    if (player->sw_volume) {
        M_LOGD("sw volume enable\n");
        player_volume_init(player);
    }

    player->cplt_sem = os_sem_new(0);
    player->resume_sem = os_sem_new(0);
    player->pause_sem = os_sem_new(0);
    player->start_sem  = os_sem_new(0);
    player->reset_sem = os_sem_new(0);
    player->stop_sem = os_sem_new(0);
    player->lock = os_mutex_new();
    player->fade.lock = os_mutex_new();

#ifdef UVOICE_DEEP_COMPLETE
    player->deep_cplt = 1;
#endif

    player->fade.out_period_ms = FADE_OUT_PERIOD_DEFAULT;
    player->fade.in_period_ms = FADE_IN_PERIOD_DEFAULT;
    player->fade_enable = PLAYER_FADE_PROC_ENABLE;
    player_reset(player);
    player->standby_msec = 5000;
    player->reference_count = 1;
    player->state = PLAYER_STAT_IDLE;
    return player;
}

static int player_deinit(player_t *player)
{
    if (!player) {
        M_LOGE("player null !\n");
        return -1;
    }

    if (player->state != PLAYER_STAT_IDLE) {
        M_LOGE("player not release !\n");
        return -1;
    }

#if (UVOICE_MLIST_ENABLE == 1)
    mlist_deinit();
#endif

    if (player->sw_volume)
        player_volume_deinit(player);

    os_mutex_free(player->fade.lock);
    os_mutex_free(player->lock);
    os_sem_free(player->stop_sem);
    os_sem_free(player->reset_sem);
    os_sem_free(player->start_sem);
    os_sem_free(player->pause_sem);
    os_sem_free(player->resume_sem);
    os_sem_free(player->cplt_sem);
    snd_free(player);

    return 0;
}

uvoice_player_t *uvoice_player_create(void)
{
    uvoice_player_t *mplayer = g_mplayer;

    if (mplayer) {
        if (player_reference(mplayer->priv) < 0) {
            M_LOGE("get uvoice player failed !\n");
            return NULL;
        }
        return mplayer;
    }

    mplayer = snd_zalloc(sizeof(uvoice_player_t), AFM_EXTN);
    if (!mplayer) {
        M_LOGE("alloc uvoice player failed !\n");
        return NULL;
    }

    mplayer->priv = player_init();
    if (!mplayer->priv) {
        M_LOGE("init player failed !\n");
        snd_free(mplayer);
        return NULL;
    }

    g_mplayer = mplayer;

    mplayer->start = player_start;
    mplayer->stop = player_stop;
    mplayer->pause = player_pause;
    mplayer->resume = player_resume;
    mplayer->complete = player_complete;
    mplayer->playback = player_playback;
    mplayer->stop_async = player_stop_async;
    mplayer->pause_async = player_pause_async;
    mplayer->resume_async = player_resume_async;
    mplayer->set_source = player_set_source;
    mplayer->clr_source = player_clr_source;
    mplayer->set_stream = player_set_stream;
    mplayer->put_stream = player_put_stream;
    mplayer->clr_stream = player_clr_stream;
    mplayer->set_pcminfo = player_set_pcminfo;
    mplayer->get_position = player_get_position;
    mplayer->get_duration = player_get_duration;
    mplayer->wait_complete = player_wait_complete;
    mplayer->get_mediainfo = player_get_mediainfo;
    mplayer->get_cacheinfo = player_get_cacheinfo;
    mplayer->get_state = player_get_state;
    mplayer->get_delay = player_get_delay;
    mplayer->set_fade = player_set_fade;
    mplayer->seek = player_seek;
    mplayer->set_format = player_set_format;
    mplayer->set_out_device = player_set_outdevice;
    mplayer->set_external_pa = player_set_external_pa;
    mplayer->set_standby = player_set_standby;
    mplayer->eq_enable = player_eq_enable;
    mplayer->set_volume = player_set_volume;
    mplayer->get_volume = player_get_volume;
    mplayer->cache_config = player_cache_config;
    mplayer->volume_range = player_volume_range;
    mplayer->state_dump = player_state_dump;
    mplayer->pcmdump_enable = player_pcmdump_enable;
    mplayer->download = player_download;
    mplayer->download_abort = player_download_abort;
    mplayer->format_support = player_format_support;

    M_LOGI("uvoice player create\n");
    return mplayer;
}

int uvoice_player_release(uvoice_player_t *mplayer)
{
    if (!mplayer) {
        M_LOGE("uvoice player null !\n");
        return -1;
    }

    if (mplayer != g_mplayer) {
        M_LOGE("uvoice player invalid !\n");
        return -1;
    }

    if (player_dereference(mplayer->priv) == 0) {
        if (player_deinit(mplayer->priv)) {
            M_LOGE("free player failed !\n");
            return -1;
        }

        snd_free(mplayer);
        g_mplayer = NULL;
        M_LOGI("uvoice player release\n");
    }

    return 0;
}

