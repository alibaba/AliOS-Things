/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <ulog/ulog.h>
#include <string.h>
#include <aos/list.h>
#include "audio_rtos.h"
#include "control.h"
#include "alsa.h"
#include "mcu_audio.h"

#define LOG_TAG    "[haas1000_audio]"

static const int32_t volume_step[] = {
    2,   // 0    mute    codec_dac_vol[2] =  -99 db
    53,  // 1    0       codec_dac_vol[53] = -48 db
    53,  // 2            codec_dac_vol[53] = -48 db
    54,  // 3            codec_dac_vol[54] = -47 db
    54,  // 4            codec_dac_vol[54] = -47 db
    55,  // 5            codec_dac_vol[55] = -46 db
    55,  // 6            codec_dac_vol[55] = -46 db
    56,  // 7    1       codec_dac_vol[56] = -45 db
    56,  // 8            codec_dac_vol[56] = -45 db
    57,  // 9            codec_dac_vol[57] = -44 db
    57,  // 10           codec_dac_vol[57] = -44 db
    58,  // 11           codec_dac_vol[58] = -43 db
    58,  // 12           codec_dac_vol[58] = -43 db
    59,  // 13   2       codec_dac_vol[59] = -42 db
    59,  // 14           codec_dac_vol[59] = -42 db
    60,  // 15           codec_dac_vol[60] = -41 db
    60,  // 16           codec_dac_vol[60] = -41 db
    61,  // 17           codec_dac_vol[61] = -40 db
    61,  // 18           codec_dac_vol[61] = -40 db
    62,  // 19   3       codec_dac_vol[62] = -39 db
    62,  // 20           codec_dac_vol[62] = -39 db
    63,  // 21           codec_dac_vol[63] = -38 db
    63,  // 22           codec_dac_vol[63] = -38 db
    64,  // 23           codec_dac_vol[64] = -37 db
    64,  // 24           codec_dac_vol[64] = -37 db
    65,  // 25   4       codec_dac_vol[65] = -36 db
    65,  // 26           codec_dac_vol[65] = -36 db
    66,  // 27           codec_dac_vol[66] = -35 db
    66,  // 28           codec_dac_vol[66] = -35 db
    67,  // 29           codec_dac_vol[67] = -34 db
    67,  // 30           codec_dac_vol[67] = -34 db
    68,  // 31   5       codec_dac_vol[68] = -33 db
    68,  // 32           codec_dac_vol[68] = -33 db
    69,  // 33           codec_dac_vol[69] = -32 db
    69,  // 34           codec_dac_vol[69] = -32 db
    70,  // 35           codec_dac_vol[70] = -31 db
    70,  // 36           codec_dac_vol[70] = -31 db
    71,  // 37   6       codec_dac_vol[71] = -30 db
    71,  // 38           codec_dac_vol[71] = -30 db
    72,  // 39           codec_dac_vol[72] = -29 db
    72,  // 40           codec_dac_vol[72] = -29 db
    73,  // 41           codec_dac_vol[73] = -28 db
    73,  // 42           codec_dac_vol[73] = -28 db
    74,  // 43           codec_dac_vol[74] = -27 db
    74,  // 44   7       codec_dac_vol[74] = -27 db
    75,  // 45           codec_dac_vol[75] = -26 db
    75,  // 46           codec_dac_vol[75] = -26 db
    76,  // 47           codec_dac_vol[76] = -25 db
    76,  // 48           codec_dac_vol[76] = -25 db
    77,  // 49           codec_dac_vol[77] = -24 db
    77,  // 50           codec_dac_vol[77] = -24 db
    78,  // 51   8       codec_dac_vol[78] = -23 db
    78,  // 52           codec_dac_vol[78] = -23 db
    79,  // 53           codec_dac_vol[79] = -22 db
    79,  // 54           codec_dac_vol[79] = -22 db
    80,  // 55           codec_dac_vol[80] = -21 db
    80,  // 56           codec_dac_vol[80] = -21 db
    81,  // 57           codec_dac_vol[81] = -20 db
    81,  // 58   9       codec_dac_vol[81] = -20 db
    82,  // 59           codec_dac_vol[82] = -19 db
    82,  // 60           codec_dac_vol[82] = -19 db
    83,  // 61           codec_dac_vol[83] = -18 db
    83,  // 62           codec_dac_vol[83] = -18 db
    84,  // 63           codec_dac_vol[84] = -17 db
    84,  // 64           codec_dac_vol[84] = -17 db
    85,  // 65   10      codec_dac_vol[85] = -16 db
    85,  // 66           codec_dac_vol[85] = -16 db
    86,  // 67           codec_dac_vol[86] = -15 db
    86,  // 68           codec_dac_vol[86] = -15 db
    87,  // 69           codec_dac_vol[87] = -14 db
    87,  // 70           codec_dac_vol[87] = -14 db
    88,  // 71   11      codec_dac_vol[88] = -13 db
    88,  // 72           codec_dac_vol[88] = -13 db
    89,  // 73           codec_dac_vol[89] = -12 db
    89,  // 74           codec_dac_vol[89] = -12 db
    90,  // 75           codec_dac_vol[90] = -11 db
    90,  // 76           codec_dac_vol[90] = -11 db
    91,  // 77   12      codec_dac_vol[91] = -10 db
    91,  // 78           codec_dac_vol[91] = -10 db
    92,  // 79           codec_dac_vol[92] =  -9 db
    92,  // 80           codec_dac_vol[92] =  -9 db
    93,  // 81           codec_dac_vol[93] =  -8 db
    93,  // 82           codec_dac_vol[93] =  -8 db
    94,  // 83   13      codec_dac_vol[94] =  -7 db
    94,  // 84           codec_dac_vol[94] =  -7 db
    95,  // 85           codec_dac_vol[95] =  -6 db
    95,  // 86           codec_dac_vol[95] =  -6 db
    96,  // 87           codec_dac_vol[96] =  -5 db
    96,  // 88           codec_dac_vol[96] =  -5 db
    97,  // 89   14      codec_dac_vol[97] =  -4 db
    97,  // 90           codec_dac_vol[97] =  -4 db
    98,  // 91           codec_dac_vol[98] =  -3 db
    98,  // 92           codec_dac_vol[98] =  -3 db
    99,  // 93           codec_dac_vol[99] =  -2 db
    99,  // 94           codec_dac_vol[99] =  -2 db
    99,  // 95           codec_dac_vol[99] =  -2 db
    100, // 96           codec_dac_vol[100] = -1 db
    100, // 97           codec_dac_vol[100] = -1 db
    100, // 98           codec_dac_vol[100] = -1 db
    100, // 99           codec_dac_vol[100] = -1 db
    101, // 100  15      codec_dac_vol[101] =  0 db
};

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
    int mute = 0;
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
    // TBD: set codec mute state, e.g. ac97_set_mute(mute);
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
    volume = alsa_volume_percent_get();
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
    dac_vol = volume_step[volume];
    alsa_volume_set(volume, dac_vol);
    return 0;
}

static pcm_stream_handler_t codec_pcm_stream_open(int mode, int sampleRate, int channels, pcm_stream_format_t format, aos_hdl_t *event_hdl)
{
    if(mode == PCM_STREAM_IN) {
        // TBD: initializa capture codec
        int large_buf = (sampleRate & 0xFF000000) >> 24;
        int _kfifo_len = KFIFO_CAPTURE_SIZE;
        if (large_buf)
        {
            _kfifo_len *= large_buf;
        }
        capture_stream_hdl = data_dump_open(aud_record_subscribe, _kfifo_len / 1024, sampleRate, 0);
        LOGD(LOG_TAG, "%s:%d: capture_stream_hdl 0x%x, mode %d, sampleRate %d, channels %d, format %d \r\n", __func__, __LINE__,
             playback_stream_hdl, PCM_STREAM_OUT, sampleRate, channels, format);
        return capture_stream_hdl;
    } else if (mode == PCM_STREAM_OUT) {
        playback_stream_hdl = (pcm_stream_handler_t)alsa_open(ALSA_MODE_OUT, sampleRate, channels, format);
        LOGD(LOG_TAG, "%s:%d: playback_stream_hdl 0x%x, mode %d, sampleRate %d, channels %d, format %d \r\n", __func__, __LINE__,
             playback_stream_hdl, PCM_STREAM_OUT, sampleRate, channels, format);
        return playback_stream_hdl;
    }
}

static int codec_pcm_stream_start(pcm_stream_handler_t hdl)
{
    int ret = 0;
    if(NULL == hdl) {
        LOGE(LOG_TAG, "%s:%d, invalid hdl.", __func__, __LINE__);
        return ret;
    }
    if(hdl == capture_stream_hdl) {
        // TBD: start capture stream
        data_dump_start(hdl);
    } else if (hdl == &playback_stream_hdl) {
        // TBD: start playback stream
    }
    LOGD(LOG_TAG, "%s:%d, ret = %d.", __func__, __LINE__, ret);
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
    } else if (hdl == playback_stream_hdl) {
        ret = alsa_write(playback_stream_hdl, (uint8_t *)buf, len);
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
    } else if (hdl == capture_stream_hdl) {
        // TBD: read capture stream
        frame_size = channel_num * sample_bytes;
        tempBuf = (char *)malloc(len * channel_num);
        if(tempBuf) {
            ret = data_dump_read(hdl, tempBuf, len * channel_num);
            for(i = 0; i < ret/frame_size; i++) {
                for(j = 0; j < sample_bytes; j++) {
                    /*  MIC阵列数据：MIC1_B1, MIC1_B0, MIC2_B1, MIC2_B0, MIC3_B1, MIC3_B0 ... */
                    // ((char *)buf)[i * sample_bytes + j] = tempBuf[i * frame_size + j]; // MIC1
                    // ((char *)buf)[i * sample_bytes + j] = tempBuf[i * frame_size + sample_bytes + j]; // MIC2
                    ((char *)buf)[i * sample_bytes + j] = tempBuf[(i+1)*frame_size - sample_bytes + j]; // MIC3*/
                }
            }
            ret = ret / channel_num;
        }
        //LOGD(LOG_TAG, "%s:%d， data_dump_read %d bytes", __func__, __LINE__, ret);
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
        // TBD: stop capture stream
        data_dump_stop(hdl);
    } else if (hdl == playback_stream_hdl) {
        ret = alsa_stop(playback_stream_hdl);
    }
    LOGD(LOG_TAG, "%s:%d, ret = %d.", __func__, __LINE__, ret);
    return ret;
}

static int codec_pcm_stream_close(pcm_stream_handler_t hdl)
{
    int ret = 0;
    if(NULL == hdl) {
        LOGE(LOG_TAG, "%s:%d, invalid hdl.", __func__, __LINE__);
        return ret;
    }
    if(hdl == capture_stream_hdl) {
        // TBD: close capture stream
        data_dump_close(hdl, aud_record_unsubscribe);
    } else if (hdl == playback_stream_hdl) {
        ret = alsa_close(playback_stream_hdl);
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
    LOGD(LOG_TAG, "%s:%d, install RTOS codec driver %d Capture %d Playback", __func__, __LINE__, cap_stream_num, pb_stream_num);
    return audio_native_card_register(cap_stream_num, pb_stream_num, &codec_pcm_ops, master_codec_controls, sizeof(master_codec_controls)/sizeof(master_codec_controls[0]));
}
//FINSH_FUNCTION_EXPORT_CMD(audio_install_codec_driver, insmod_audio_drv,  RTOS Codec Driver Test)
