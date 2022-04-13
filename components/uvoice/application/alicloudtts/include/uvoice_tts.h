/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_TTS_H__
#define __UVOICE_TTS_H__

typedef enum {
    TTS_AICLOUD_ALIYUN = 0, /* aliyun tts service */
    /*if use aliyun tts service, please get app_key、token from aliyun，see https://help.aliyun.com/document_detail/84435.html?spm=a2c4g.11186623.2.30.355b5275kgR9PT
     to get the config guide*/
} tts_aicloud_type_e;

typedef enum {
    TTS_ENCODE_GBK = 0,
    TTS_ENCODE_UTF8,
} tts_encode_type_e;

typedef struct {
    char *app_key; /* get it form the cloud service */
    char *token;   /* get it form the cloud service */
    media_format_t format;      /* tts output format, now only support wav, pcm, mp3 */
    int sample_rate; /* support 8000Hz、16000Hz */
    char *voice;     /* voice people */
    int volume;      /* 0 ~ 100 */
    int speech_rate; /* -500 ~ 500 */
    int pitch_rate;  /* -500 ~ 500 */
    tts_encode_type_e text_encode_type;
} tts_config_t;

typedef enum
{
    TTS_RECV_URL  = 0,
    TTS_RECV_DATA = 1,
} tts_recv_type_e;

typedef enum
{
    TTS_TRANS_START    = 0,
    TTS_TRANS_COMPLETE = 1,
    TTS_RECV_START     = 2,
    TTS_RECV_COMPLETE  = 3,
    TTS_INFO           = 4,
    TTS_WARNING        = 5,
    TTS_ERROR          = 6,
} tts_event_e;

typedef struct {
    int (*recv_url)(char *tts_url);
    int (*recv_data)(uint8_t *buffer, int nbytes, int index);
    int (*event)(tts_event_e event, char *info);
} tts_recv_callback_t;

typedef struct {
    int (*tts_init)(tts_aicloud_type_e aicloud_type, tts_config_t *config);
    int (*tts_request)(char *text, tts_recv_type_e recv_type, tts_recv_callback_t *recv_cb);
    int (*tts_stop)();
    void *priv;
} uvoice_tts_t;

#ifdef UVOICE_TTS_ENABLE
uvoice_tts_t *uvoice_tts_create(void);
int uvoice_tts_release(uvoice_tts_t *uvoice_tts);
#endif

#endif /* __UVOICE_TTS_H__ */
