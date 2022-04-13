/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <ulog/ulog.h>
#include <string.h>
#include <aos/list.h>
#include "audio_rtos.h"
#include "control.h"
#include <drv/codec.h>
#include <aos/aos.h>

#define LOG_TAG    "[template]"
typedef struct {
    csi_codec_output_t hdl;
    int vol;
    int mute;
} mixer_playback_t;

static mixer_playback_t mixer;

static const int playback_stream_hdl = 0;
static const int capture_stream_hdl = 1;
static csi_codec_t codec_a;
static csi_dma_ch_t dma_ch_output_handle;
static int codec_hw_vol_get(struct audio_kcontrol *kcontrol, struct audio_ctl_elem_value *ucontrol);
static int codec_hw_vol_put(struct audio_kcontrol *kcontrol, struct audio_ctl_elem_value *ucontrol);
static int codec_hw_mute_state_get(struct audio_kcontrol *kcontrol, struct audio_ctl_elem_value *ucontrol);
static int codec_hw_mute_state_put(struct audio_kcontrol *kcontrol, struct audio_ctl_elem_value *ucontrol);

static const struct audio_kcontrol_new master_codec_controls[] = {
	SOC_SINGLE_EXT("Master Volume TX",     /* master volume attribute name: defined in sound_mixer.h */
			    0,
			    0,                         /* codec volume minimum value */
			    63,                        /* codec volume maxmum value */
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
    int mute = mixer.mute;
    if(!kcontrol || !ucontrol) {
        LOGE(LOG_TAG, "%s:%d: invalid params. \r\n", __func__, __LINE__);
        return -1;
    }
    // TBD: get codec mute state, e.g. mute = ac97_get_mute();
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
    
    if (mixer.hdl.callback != NULL && mute != mixer.mute) {
        csi_codec_output_t *p = &mixer.hdl;

        csi_codec_output_mute(p, mute);
    }

    mixer.mute = mute;
    // TBD: set codec mute state, e.g. ac97_set_mute(mute);
    return 0;
}

static int codec_hw_vol_get(struct audio_kcontrol *kcontrol, struct audio_ctl_elem_value *ucontrol)
{
    int volume = mixer.vol;
    if(!kcontrol || !ucontrol) {
        LOGE(LOG_TAG, "%s:%d: invalid params. \r\n", __func__, __LINE__);
        return -1;
    }
    // TBD: get codec volume, e.g. volume = ac97_get_vol();
    LOGD(LOG_TAG, "%s:%d: get volume %d \r\n", __func__, __LINE__, volume);
    ucontrol->value.integer.value[0] = volume;
    return 0;
}

static int codec_hw_vol_put(struct audio_kcontrol *kcontrol, struct audio_ctl_elem_value *ucontrol)
{
    int volume = 0;
    if(!kcontrol || !ucontrol) {
        LOGE(LOG_TAG, "%s:%d: invalid params. \r\n", __func__, __LINE__);
        return -1;
    }
    volume = ucontrol->value.integer.value[0];
    LOGD(LOG_TAG, "%s:%d: set volume %d \r\n", __func__, __LINE__, volume);
    // TBD: set codec volume, e.g. ac97_set_vol(volume);

    mixer.vol = volume;

    if (mixer.hdl.callback != NULL) {
        csi_codec_output_t *p = &mixer.hdl;
        if (mixer.mute) {
            csi_codec_output_mute(p, 0);
            mixer.mute = 0;
        }
        csi_codec_output_analog_gain(p, volume < -31 ? -31 : volume);
        csi_codec_output_mix_gain(p, volume);
    }

    return 0;
}

typedef struct {
    int mode;
    csi_codec_output_t *hdl;
    int rate;
    int channels;
    pcm_stream_format_t format;
    aos_hdl_t *event_hdl;
    int state;
} playback_t;

static void codec_event_cb(csi_codec_input_t *codec, csi_codec_event_t event, void *arg)
{
    // playback_t *playback = (playback_t *)arg;

    if (event == CODEC_EVENT_PERIOD_WRITE_COMPLETE) {
        // pcm->event.cb(pcm, PCM_EVT_WRITE, pcm->event.priv);
    } else if (event == CODEC_EVENT_PERIOD_READ_COMPLETE) {
        // pcm->event.cb(pcm, PCM_EVT_READ, pcm->event.priv);
    } else if (event == CODEC_EVENT_READ_BUFFER_FULL) {
        // pcm->event.cb(pcm, PCM_EVT_XRUN, pcm->event.priv);
    } else {
        // pcm->event.cb(pcm, PCM_EVT_XRUN, pcm->event.priv);
    }
}

#define OUTPUT_BUF_SIZE 11520
#define OUTPUT_PERIOD_SIZE 1280
uint8_t output_buf[OUTPUT_BUF_SIZE];

static pcm_stream_handler_t playback_open(int mode, int sampleRate, int channels, int format, aos_hdl_t *event_hdl)
{
#define PLAYBACK_MS (100)
    playback_t *playback = aos_zalloc(sizeof(playback_t));
    csi_codec_output_t *codec = aos_zalloc(sizeof(csi_codec_output_t));
    ringbuffer_t *ring_buf = aos_zalloc(sizeof(ringbuffer_t));

    int format_byte = 16/8;
    // int format_byte = pcm_formate_to_bytes(format);
    int send_size   = PLAYBACK_MS * channels * format_byte;
    uint8_t *send = aos_malloc(send_size);


    if (playback == NULL || codec == NULL || send == NULL || ring_buf == NULL) {
        goto playback_err;
    }

    playback->hdl = codec;
    playback->mode = mode;
    playback->rate = sampleRate;
    playback->channels = channels;
    playback->format = format;
    playback->event_hdl = event_hdl;

    codec->ring_buf = ring_buf;
    csi_codec_output_open(&codec_a, codec, 0);
    csi_codec_output_attach_callback(codec, codec_event_cb, playback);

    csi_codec_output_config_t output_config;
    output_config.bit_width = 16;
    output_config.sample_rate = sampleRate;
    output_config.buffer = output_buf;
    output_config.buffer_size = OUTPUT_BUF_SIZE;
    output_config.period = OUTPUT_PERIOD_SIZE;
    output_config.mode = CODEC_OUTPUT_SINGLE_ENDED;
    output_config.sound_channel_num = channels;
    int ret = csi_codec_output_config(codec, &output_config);
    if (ret != 0) {
        goto playback_err;
    }

    csi_codec_output_analog_gain(codec, 0xaf);
    csi_codec_output_buffer_reset(codec);
    csi_codec_output_link_dma(codec, &dma_ch_output_handle);

    return (pcm_stream_handler_t)playback;

playback_err:
    if (playback) {
        aos_free(playback);
    }

    if (codec) {
        aos_free(codec);
    }

    if (ring_buf) {
        aos_free(ring_buf);
    }

    if (send) {
        aos_free(send);
    }

    return NULL;
}

static pcm_stream_handler_t codec_pcm_stream_open(int mode, int sampleRate, int channels, int format, aos_hdl_t *event_hdl)
{
    pcm_stream_handler_t hdl = NULL;
    if(mode == PCM_STREAM_IN) {
        hdl = (pcm_stream_handler_t)&capture_stream_hdl;
        // TBD: initializa capture codec
    } else if (mode == PCM_STREAM_OUT) {
        hdl = (pcm_stream_handler_t)&playback_stream_hdl;
        // TBD: initializa playback codec
        hdl = playback_open(mode, sampleRate, channels, format, event_hdl);
    }
    return hdl;
}

static int playback_start(void *hdl)
{
    playback_t *playback = (playback_t *)hdl;
    csi_codec_output_t *codec = playback->hdl;
    

    csi_codec_output_start(codec);

    if (mixer.mute){
        // csi_codec_output_mute(codec, 1);
    } else {
        // csi_codec_output_mute(codec, 0);
        // csi_codec_output_analog_gain(codec, mixer.vol);
    }

    return 0;
}
static int codec_pcm_stream_start(pcm_stream_handler_t hdl)
{
    int ret = 0;
    int *mode = (int *)hdl;

    if(*mode == PCM_STREAM_IN) {
        // TBD: start capture stream

    } else if (*mode == PCM_STREAM_OUT) {
        playback_start(hdl);
    }
    return ret;
}

static int codec_pcm_stream_write(pcm_stream_handler_t hdl, void *buf, unsigned int len)
{
    playback_t *playback = (playback_t *)hdl;

    // LOGE("drv", "write enter(%lld)\r\n", aos_now_ms());
    int ret = csi_codec_output_write_async(playback->hdl, (uint8_t *)buf, len);
    // int ret = csi_codec_output_write(playback->hdl, (uint8_t *)buf, len);

    // LOGE("drv", "write(%d)(%d)\r\n", len, ret);
    return ret;
}

static int codec_pcm_stream_read(pcm_stream_handler_t hdl, void *buf, unsigned int len)
{
    int ret = 0;
    if(hdl == &playback_stream_hdl) {
        LOGE(LOG_TAG, "%s:%d, read operation not allowed on playback stream.", __func__, __LINE__);
    } else if (hdl == &capture_stream_hdl) {
        // TBD: read capture stream
    }
    return ret;
}

static int playback_pause(void *hdl, int enable)
{
    playback_t *playback = (playback_t *)hdl;
    
    if (enable) {
        // csi_codec_output_pause(playback->hdl);
    } else {
        // csi_codec_output_resume(playback->hdl);
    }

    return 0;
}

static int codec_pcm_stream_pause(pcm_stream_handler_t hdl, int enable)
{
    int ret = 0;

    int *mode = (int *)hdl;

    if(*mode == PCM_STREAM_IN) {
        // TBD: start capture stream

    } else if (*mode == PCM_STREAM_OUT) {
        playback_pause(hdl, enable);
    }


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
    int *mode = (int *)hdl;

    if(*mode == PCM_STREAM_IN) {
        // TBD: start capture stream

    } else if (*mode == PCM_STREAM_OUT) {
        playback_t *playback = (playback_t *)hdl;
        csi_codec_output_stop(playback->hdl);
    }
    return ret;
}

static int codec_pcm_stream_close(pcm_stream_handler_t hdl)
{
    int ret = 0;
    int *mode = (int *)hdl;

    if(*mode == PCM_STREAM_IN) {
        // TBD: start capture stream

    } else if (*mode == PCM_STREAM_OUT) {
        playback_t *playback = (playback_t *)hdl;
        csi_codec_output_close(playback->hdl);
    }
    return ret;
}

static int codec_pcm_stream_recover(pcm_stream_handler_t hdl)
{
    int ret = 0;
    if(hdl == &playback_stream_hdl) {
        // TBD: recover playback stream
    } else if (hdl == &capture_stream_hdl) {
        // TBD: recover capture stream
    }
    return ret;
}

pcm_stream_ops_t codec_pcm_ops = {
    .open = codec_pcm_stream_open,
    .start = codec_pcm_stream_start,
    .read = codec_pcm_stream_read,
    .write = codec_pcm_stream_write,
    .pause = codec_pcm_stream_pause,
    .stop = codec_pcm_stream_stop,
    .close = codec_pcm_stream_close,
    .recover = codec_pcm_stream_recover,
    .suspend = codec_pcm_stream_suspend,
    .resume = codec_pcm_stream_resume,
};

/* Application shall call this API to install codec driver instance. */
int audio_install_codec_driver()
{
    int pb_stream_num = 1;
    int cap_stream_num = 1;
    csi_codec_init(&codec_a, 0);
    LOGD(LOG_TAG, "%s:%d, install RTOS codec driver %d Capture %d Playback", __func__, __LINE__, cap_stream_num, pb_stream_num);
    return audio_native_card_register(cap_stream_num, pb_stream_num, &codec_pcm_ops, master_codec_controls, sizeof(master_codec_controls)/sizeof(master_codec_controls[0]));
}
//FINSH_FUNCTION_EXPORT_CMD(audio_install_codec_driver, insmod_audio_drv,  RTOS Codec Driver Test)