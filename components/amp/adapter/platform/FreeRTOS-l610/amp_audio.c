/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "amp_system.h"
#include "amp_defines.h"
#include "amp_audio.h"

#include "ivTTS.h"
#include "fibocom.h"

#ifndef true
#define true  1
#define false 0
#endif

#define MOD_STR        "AMPAudio"


#define CTTS_STRING_ENCODE_TYPE_UTF8 (0)
#define CTTS_STRING_ENCODE_TYPE_GB2312 (1)
#define CTTS_TEXT_MAX_LEN (512)

enum
{
    ML_ISO8859_1,
    ML_UTF8,
    ML_UTF16BE,
    ML_UTF16LE,
    ML_GSM,
    ML_CP936
};

int32_t amp_hal_audio_play(const char *source)
{
    int ret;

    ret = fibo_audio_play(1, source);
    if (ret < 0) {
        amp_error(MOD_STR, "play audio error %d !", ret);
        return -1;
    }

    return 0;
}

int32_t amp_hal_audio_stop(void)
{
    int ret;

    ret = fibo_audio_stop();
    if (ret < 0) {
        amp_error(MOD_STR, "stop audio play error %d !", ret);
        return -1;
    }

    return 0;
}

int32_t amp_hal_audio_pause(void)
{
    int ret;

    ret = fibo_audio_pause();
    if (ret < 0) {
        amp_error(MOD_STR, "pause audio play error %d !", ret);
        return -1;
    }

    return 0;
}

int32_t amp_hal_audio_resume(void)
{
    int ret;

    ret = fibo_audio_resume();
    if (ret < 0) {
        amp_error(MOD_STR, "resume audio play error %d !", ret);
        return -1;
    }

    return 0;
}

int32_t amp_hal_audio_volume_set(int device, int volume)
{
    int ret;

    ret = fibo_set_audio_volume(device, volume);
    if (ret < 0) {
        amp_error(MOD_STR, "set audio play volume error %d !\n", ret);
        return -1;
    }

    return 0;
}

int32_t amp_hal_audio_mute(int state)
{
    if (state < 0 || state > 3) {
        amp_warn(MOD_STR, "state [%d] is invalid\n", state);
        return -1;
    }

    if (state == 0) {
        if (!audevSetVoiceUplinkMute(false)) {
            amp_warn(MOD_STR, "%s,%d\n", __func__,__LINE__);
            return -1;
        }

        if(!audevSetOutputMute(false)) {
            amp_warn(MOD_STR, "%s,%d\n", __func__,__LINE__);
            return -1;
        }
    } else if(state == 1) {
        if (!audevSetVoiceUplinkMute(true)) {
            amp_warn(MOD_STR, "%s,%d\n", __func__,__LINE__);
            return -1;
        }

        if(!audevSetOutputMute(false)) {
            amp_warn(MOD_STR, "%s,%d\n", __func__,__LINE__);
            return -1;
        }
    } else if(state == 2) {
        if(!audevSetVoiceUplinkMute(false)) {
            amp_warn(MOD_STR, "%s,%d\n", __func__,__LINE__);
            return -1;
        }

        if(!audevSetOutputMute(true)) {
            amp_warn(MOD_STR, "%s,%d\n", __func__,__LINE__);
            return -1;
        }
    } else if(state == 3) {
        if(!audevSetVoiceUplinkMute(true)) {
            amp_warn(MOD_STR, "%s,%d\n", __func__,__LINE__);
            return -1;
        }
        if(!audevSetOutputMute(true)) {
            amp_warn(MOD_STR, "%s,%d\n", __func__,__LINE__);
            return -1;
        }
    }  

    return 0;
}

int32_t amp_hal_mic_gain_set(int gain)
{
    int ret;

    ret = fibo_set_microphone_gain_value((uint8_t)gain);
    if (ret < 0) {
        amp_error(MOD_STR, "set mic gain error %d !", ret);
        return -1;
    }

    return 0;
}

int32_t amp_hal_mic_gain_get(int *gain)
{
    uint8_t val = 0;
    int ret;

    ret = fibo_get_microphone_gain_value(&val);
    if (ret < 0) {
        amp_error(MOD_STR, "get mic gain error %d !", ret);
        return -1;
    }

    *gain = val;
    return 0;
}

int32_t amp_hal_tts_play(const char *text, amp_text_encode_type_t type)
{
    unsigned int ml_encoding = ML_UTF8;
	unsigned int len;

    if (!text) {
        amp_error(MOD_STR, "tts text is null !");
        return -1;
    }

    len = strlen(text);
    if (len == 0 || len > CTTS_TEXT_MAX_LEN) {
        amp_error(MOD_STR, "str len is invalid  !!!!\r\n");
        return -1;
    }

    if (type == TEXT_ENCODE_TYPE_GB2312)
        ml_encoding = ML_CP936;

    if (!ttsPlayText(text, strlen(text), ml_encoding)) {
        amp_warn(MOD_STR, "ttsPlayText failed");
        return -1;
    }

    return 0;
}

int32_t amp_hal_tts_stop(void)
{
    ttsStop();
    return 0;
}

int32_t amp_hal_tts_volume_set(int volume)
{
    int ret;

    if (volume < ivTTS_VOLUME_MIN || volume > ivTTS_VOLUME_MAX) {
        amp_error(MOD_STR, "volume [%d] is invalid\n", volume);
        return -1;
    }

    ret = iVTTS_Voice_Volume(volume);
    if (ret < 0) {
        amp_error(MOD_STR, "set tts amp_error volume error %d !\n", ret);
        return -1;
    }

    return 0;
}

int32_t amp_hal_tts_pitch_set(int pitch)
{
    int ret;

    if (pitch < ivTTS_PITCH_MIN || pitch > ivTTS_PITCH_MAX) {
        amp_error(MOD_STR, "pitch [%d] is invalid\n", pitch);
        return -1;
    }

    ret = iVTTS_voice_Pitch(pitch);
    if (ret < 0) {
        amp_error(MOD_STR, "set tts voice pitch error %d !\n", ret);
        return -1;
    }

    return 0;
}

int32_t amp_hal_tts_speed_set(int speed)
{
    int ret;

    if (speed < ivTTS_SPEED_MIN || speed > ivTTS_SPEED_MAX) {
        amp_error(MOD_STR, "speed [%d] is invalid\n", speed);
        return -1;
    }


    ret = iVTTS_Voice_Speed(speed);
    if (ret < 0) {
        amp_error(MOD_STR, "set tts voice speed error %d !\n", ret);
        return -1;
    }

    return 0;
}

int32_t amp_hal_tts_is_playing(void)
{
    if (ttsIsPlaying()) {
        return 1;
    }

    return 0;
}
