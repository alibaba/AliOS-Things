/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <ulog/ulog.h>
#include <string.h>
#include <stdio.h>
#include <aos/list.h>
#include "audio_rtos.h"
#include "control.h"
#include "drv/gpio.h"
#include "audio_internel.h"

#define LOG_TAG    "[haas200_audio]"

pcm_stream_handler_t audio_stream_hdl = NULL;
pcm_stream_handler_t playback_stream_hdl = NULL;
pcm_stream_handler_t capture_stream_hdl = NULL;
static int codec_hw_vol_get(struct audio_kcontrol *kcontrol, struct audio_ctl_elem_value *ucontrol);
static int codec_hw_vol_put(struct audio_kcontrol *kcontrol, struct audio_ctl_elem_value *ucontrol);
static int codec_hw_mute_state_get(struct audio_kcontrol *kcontrol, struct audio_ctl_elem_value *ucontrol);
static int codec_hw_mute_state_put(struct audio_kcontrol *kcontrol, struct audio_ctl_elem_value *ucontrol);


static const struct audio_kcontrol_new master_codec_controls[] = {
    SOC_SINGLE_EXT("Master Volume TX",     /* master volume attribute name: defined in sound_mixer.h */
            0,
            0,                         /* codec volume minimum value */
            100,                        /* codec volume maxmum value */
            0,
            codec_hw_vol_get,          /* get codec volume api */
            codec_hw_vol_put),         /* set codec volume api */
    SOC_SINGLE_EXT("Master Mute State",    /* master mute attribute name: defined in sound_mixer.h */
            0,
            0,                         /* codec mute state minimum value */
            1,                         /* codec mute state maxmum value */
            0,
            codec_hw_mute_state_get,   /* get codec mute state */
            codec_hw_mute_state_put),  /* put codec mute state */
};

static int codec_hw_mute_state_get(struct audio_kcontrol *kcontrol, struct audio_ctl_elem_value *ucontrol)
{
    int mute = 0;
    if(!kcontrol || !ucontrol) {
        LOGE(LOG_TAG, "%s:%d: invalid params. \r\n", __func__, __LINE__);
        return -1;
    }
    mute = rl6548_get_mute();
    LOGD(LOG_TAG, "%s:%d: get mute state %d \r\n", __func__, __LINE__, mute);
    ucontrol->value.integer.value[0] = mute;
    return 0;
}

static int codec_hw_mute_state_put(struct audio_kcontrol *kcontrol, struct audio_ctl_elem_value *ucontrol)
{
    int mute = 0;
    if(!kcontrol || !ucontrol) {
        LOGE(LOG_TAG, "%s:%d: invalid params. \r\n", __func__, __LINE__);
        return -1;
    }
    mute = ucontrol->value.integer.value[0];
    LOGD(LOG_TAG, "%s:%d: set mute state %d \r\n", __func__, __LINE__, mute);
    rl6548_set_mute(mute);
    return 0;
}


static int codec_hw_vol_get(struct audio_kcontrol *kcontrol, struct audio_ctl_elem_value *ucontrol)
{
    int volume = 0;
    if(!kcontrol || !ucontrol) {
        LOGE(LOG_TAG, "%s:%d: invalid params. \r\n", __func__, __LINE__);
        return -1;
    }
    // TBD: get codec volume, e.g. volume = ac97_get_vol();
    volume = rl6548_volume_get();
    LOGD(LOG_TAG, "%s:%d: get volume %d \r\n", __func__, __LINE__, volume);
    ucontrol->value.integer.value[0] = volume;
    return 0;
}

static int codec_hw_vol_put(struct audio_kcontrol *kcontrol, struct audio_ctl_elem_value *ucontrol)
{
    int volume = 0, dac_vol = 0;
    if(!kcontrol || !ucontrol) {
        LOGE(LOG_TAG, "%s:%d: invalid params. \r\n", __func__, __LINE__);
        return -1;
    }
    volume = ucontrol->value.integer.value[0];
    if (volume < 0)
    {
        volume = 0;
    }
    if (volume > 100)
    {
        volume = 100;
    }
    LOGD(LOG_TAG, "%s:%d: set volume %d \r\n", __func__, __LINE__, volume);
    // dac_vol = volume_step[volume];
    // alsa_volume_set(volume, dac_vol);
    rl6548_volume_set(volume);
    return 0;
}


static pcm_stream_handler_t codec_pcm_stream_open(int mode, int sampleRate, int channels, pcm_stream_format_t format, aos_hdl_t *event_hdl)
{
    int word_len = WL_16;
    if(format == PCM_STREAM_FORMAT_S8 ) {
        word_len = WL_8;
    } else if(format == PCM_STREAM_FORMAT_S16_LE) {
        word_len = WL_16;
    } else if(format == PCM_STREAM_FORMAT_S24_LE) {
        word_len = WL_24;
    }

    if(audio_stream_hdl == NULL) {
        audio_stream_hdl = (pcm_stream_handler_t) rl6548_audio_init(sampleRate, channels, word_len);   
    }

    if(mode == PCM_STREAM_IN) {
        capture_stream_hdl = (char*)audio_stream_hdl + 1;
        LOGD(LOG_TAG, "%s:%d: capture_stream_hdl 0x%x, mode %d, sampleRate %d, channels %d, format %d \r\n", __func__, __LINE__,
                capture_stream_hdl, PCM_STREAM_OUT, sampleRate, channels, format);
    } else if (mode == PCM_STREAM_OUT) {
        playback_stream_hdl = (char*)audio_stream_hdl + 2;
        LOGD(LOG_TAG, "%s:%d: playback_stream_hdl 0x%x, mode %d, sampleRate %d, channels %d, format %d \r\n", __func__, __LINE__,
                playback_stream_hdl, PCM_STREAM_OUT, sampleRate, channels, format);
    }
    return audio_stream_hdl;
}



static int codec_pcm_stream_start(pcm_stream_handler_t hdl)
{
    int ret = 0;
    if(NULL == hdl) {
        LOGE(LOG_TAG, "%s:%d, invalid hdl.", __func__, __LINE__);
        return ret;
    }

    if(hdl == capture_stream_hdl) {
        rl6548_capture_start();
        LOGD(LOG_TAG, "rx codec_pcm_stream_start");
    } else if (hdl == playback_stream_hdl) {
        rl6548_playback_start();
        LOGD(LOG_TAG, "tx codec_pcm_stream_start");
    }

    // LOGD(LOG_TAG, "%s:%d, ret = %d.", __func__, __LINE__, ret);
    return ret;
}

static int codec_pcm_stream_write(pcm_stream_handler_t hdl, void *buf, unsigned int len)
{
    int ret = -1;
    if(NULL == hdl) {
        LOGE(LOG_TAG, "%s:%d, invalid hdl.", __func__, __LINE__);
        return ret;
    }
    if(hdl == capture_stream_hdl) {
        LOGE(LOG_TAG, "%s:%d, write operation not allowed on capture stream.", __func__, __LINE__);
    } else 
        if (hdl == playback_stream_hdl) {
            ret = rl6548_audio_write(playback_stream_hdl, (uint8_t *)buf, len);
        }
    return ret;
}

static int codec_pcm_stream_read(pcm_stream_handler_t hdl, void *buf, unsigned int len)
{
    int ret = -1, i = 0, j = 0;
    int channel_num = 3, sample_bytes = 2, frame_size;
    char *tempBuf = NULL;
    if(NULL == hdl) {
        LOGE(LOG_TAG, "%s:%d, invalid hdl.", __func__, __LINE__);
        return ret;
    }
    if(hdl == playback_stream_hdl) {
        LOGE(LOG_TAG, "%s:%d, read operation not allowed on playback stream.", __func__, __LINE__);
    } else 
        if (hdl == capture_stream_hdl) {
            // TBD: read capture stream, get last channel data
            frame_size = channel_num * sample_bytes;
            ret = rl6548_data_read(hdl, buf, len);
            LOGD(LOG_TAG, "%s:%d， data_dump_read %d bytes", __func__, __LINE__, ret);
        }
    return ret;
}

static int codec_pcm_stream_pause(pcm_stream_handler_t hdl, int enable)
{
    int ret = -1;
    if(NULL == hdl) {
        LOGE(LOG_TAG, "%s:%d, invalid hdl.", __func__, __LINE__);
        return ret;
    }
    if(hdl == playback_stream_hdl) {
        // TBD: pause playback stream
    } else if (hdl == capture_stream_hdl) {
        // TBD: pause capture stream
    }
    LOGD(LOG_TAG, "%s:%d, ret = %d.", __func__, __LINE__, ret);
    return ret;
}

static int codec_pcm_stream_suspend(pcm_stream_handler_t hdl)
{
    int ret = 0;
    if(hdl == &playback_stream_hdl) {
        // TBD: put playback stream into lowpower/suspend mode
    } else if (hdl == &capture_stream_hdl) {
        // TBD: put capture stream into lowpower/suspend mode
    }
    return ret;
}

static int codec_pcm_stream_resume(pcm_stream_handler_t hdl)
{
    int ret = 0;
    if(hdl == &playback_stream_hdl) {
        // TBD: put playback stream into active mode
    } else if (hdl == &capture_stream_hdl) {
        // TBD: put playback stream into active mode
    }
    return ret;
}

static int codec_pcm_stream_stop(pcm_stream_handler_t hdl)
{
    int ret = 0;
    if(NULL == hdl) {
        LOGE(LOG_TAG, "%s:%d, invalid hdl.", __func__, __LINE__);
        return ret;
    }
    if(hdl == capture_stream_hdl) {
        rl6548_capture_stop();
    } else if (hdl == playback_stream_hdl) {
        rl6548_playback_stop();
    }
    // LOGD(LOG_TAG, "%s:%d, ret = %d.", __func__, __LINE__, ret);
    LOGD(LOG_TAG, "codec_pcm_stream_stop");
    return ret;
}

static int codec_pcm_stream_close(pcm_stream_handler_t hdl)
{
    int ret = 0;
    if(NULL == hdl) {
        LOGE(LOG_TAG, "%s:%d, invalid hdl.", __func__, __LINE__);
        return ret;
    }

    if(audio_stream_hdl == NULL) {
        playback_stream_hdl = capture_stream_hdl = NULL;
        return ret;
    }

    if(hdl == capture_stream_hdl) {
        rl6548_audio_deinit();
        capture_stream_hdl = audio_stream_hdl = NULL;
    } else if (hdl == playback_stream_hdl) {
        rl6548_audio_deinit();
        playback_stream_hdl = audio_stream_hdl = NULL;
    }
    LOGD(LOG_TAG, "%s:%d, ret = %d.", __func__, __LINE__, ret);
    return ret;
}

static int codec_pcm_stream_recover(pcm_stream_handler_t hdl)
{
    int ret = -1;
    if(NULL == hdl) {
        LOGE(LOG_TAG, "%s:%d, invalid hdl.", __func__, __LINE__);
        return ret;
    }
    if(hdl == capture_stream_hdl) {
        // TBD: recover capture stream
    } else if (hdl == playback_stream_hdl) {
        // TBD: recover playback stream
    }
    LOGD(LOG_TAG, "%s:%d, ret = %d.", __func__, __LINE__, ret);
    return ret;
}

pcm_stream_ops_t codec_pcm_ops = {
    .open = codec_pcm_stream_open,
    .start = codec_pcm_stream_start,
    .read = codec_pcm_stream_read,
    .write = codec_pcm_stream_write,
    // .pause = codec_pcm_stream_pause,
    .stop = codec_pcm_stream_stop,
    .close = codec_pcm_stream_close,
    // .recover = codec_pcm_stream_recover,
    // .suspend = codec_pcm_stream_suspend,
    // .resume = codec_pcm_stream_resume,
};

/* Application shall call this API to install codec driver instance. */
int audio_install_codec_driver()
{
    int pb_stream_num = 1;
    int cap_stream_num = 1;
    LOGD(LOG_TAG, "%s:%d, install RTOS codec driver %d Capture %d Playback", __func__, __LINE__, cap_stream_num, pb_stream_num);
    return audio_native_card_register(cap_stream_num, pb_stream_num, &codec_pcm_ops, master_codec_controls, sizeof(master_codec_controls)/sizeof(master_codec_controls[0]));
}
//FINSH_FUNCTION_EXPORT_CMD(audio_install_codec_driver, insmod_audio_drv,  RTOS Codec Driver Test)