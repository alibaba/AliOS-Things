/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_AIYUNTTS_INTF_H__
#define __UVOICE_AIYUNTTS_INTF_H__

#define UVOICE_TTS_MAX_TEXT_LEN      200
#define UVOICE_TTS_MAX_TEXT_LEN_STR "200"

#define UVOICE_TTS_HTTPGET_URL_LENGTH 1024

#define TTS_ALIYUN_SAMPLE_RATE_8K  8000
#define TTS_ALIYUN_SAMPLE_RATE_16K 16000
#define TTS_ALIYUN_SAMPLE_RATE_24K 24000

#define TTS_SPEECH_RATE_MIN (-500)
#define TTS_SPEECH_RATE_MAX (500)

#define TTS_PITCH_RATE_MIN  (-500)
#define TTS_PITCH_RATE_MAX  (500)

#define TTS_VOLUME_MIN      (0)
#define TTS_VOLUME_MAX      (100)

#define ALIYUN_TTS_STATE_NULL   0
#define ALIYUN_TTS_STATE_INITED 1
#define ALIYUN_TTS_STATE_STOP   2

typedef enum
{
    VOICE_CHINESE   = 0,    /* Chinese */
    VOICE_ENGLISH   = 1,    /* English */
    VOICE_CN_EN_MIX = 2,  /* Mixed Chinese & English */
    VOICE_NULL = 0xFFFF,
} voice_language_e;

typedef struct {
    char *voice_people;
    voice_language_e lang[3];
    int sample_rate[3];
} voice_spec_t;

#endif /* end of __UVOICE_AIYUNTTS_INTF_H__ */