/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#ifndef __UVOICE_AIYUNTTS_H__
#define __UVOICE_AIYUNTTS_H__

typedef struct {
    char app_key[20]; /* get it form the cloud service */
    char token[36];   /* get it form the cloud service */
    media_format_t format;      /* tts output format, now only support wav, pcm, mp3 */
    int sample_rate; /* support 8000Hz¡¢16000Hz */
    char voice[8];     /* voice people */
    int volume;      /* 0 ~ 100 */
    int speech_rate; /* -500 ~ 500 */
    int pitch_rate;  /* -500 ~ 500 */
    tts_encode_type_e text_encode_type;
} aliyun_tts_config_t;


int uvoice_tts_aliyun_init(tts_config_t *config);
int uvoice_tts_aliyun_request(char *text, tts_recv_type_e recv_type, tts_recv_callback_t *recv_cb);
int uvoice_tts_aliyun_stop();

#endif /* __UVOICE_AIYUNTTS_H__ */